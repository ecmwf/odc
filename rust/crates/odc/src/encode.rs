//! Encoding a Polars `DataFrame` into ODB-2.

use std::collections::{BTreeMap, HashMap};
use std::path::Path;
use std::sync::atomic::{AtomicBool, Ordering};

use odc_sys::{Bit, ColumnType, SettingsWrapper};
use polars::prelude::*;

use crate::error::{Error, Result};
use crate::init;

/// Options for [`write_odb`].
///
/// # Example
///
/// Encode an integer column as a BITFIELD with named bit groups:
///
/// ```
/// use odc::{Bit, ColumnType, WriteOptions};
///
/// let mut options = WriteOptions::default();
/// options.types.insert("flags@body".into(), ColumnType::Bitfield);
/// options.bitfields.insert(
///     "flags@body".into(),
///     vec![
///         Bit { name: "active".into(), size: 1, offset: 0 },
///         Bit { name: "blacklisted".into(), size: 1, offset: 1 },
///     ],
/// );
/// ```
#[derive(Debug, Clone)]
pub struct WriteOptions {
    /// Maximum number of rows per physical output frame.
    pub rows_per_frame: usize,
    /// Per-column overrides of the dtype-derived ODB column type. Supported:
    /// `Integer` → `Bitfield` (requires a [`bitfields`](Self::bitfields)
    /// entry) and `Double` ↔ `Real`.
    pub types: HashMap<String, ColumnType>,
    /// Key/value properties attached to every output frame.
    pub properties: BTreeMap<String, String>,
    /// Bit group layout for columns encoded as `Bitfield`.
    pub bitfields: HashMap<String, Vec<Bit>>,
}

impl Default for WriteOptions {
    fn default() -> Self {
        Self {
            rows_per_frame: 10_000,
            types: HashMap::new(),
            properties: BTreeMap::new(),
            bitfields: HashMap::new(),
        }
    }
}

/// Encode a `DataFrame` into an ODB-2 file.
///
/// Column types derive from dtypes: `Int64` (and smaller integers /
/// `Boolean`, widened) → INTEGER, `Float64` → DOUBLE, `Float32` → REAL,
/// `String` → STRING; nulls become ODB missing values. Other dtypes are
/// rejected.
///
/// # Example
///
/// ```no_run
/// use odc::polars::prelude::*;
///
/// let df = df!(
///     "expver" => ["0001", "0001"],
///     "obsvalue@body" => [Some(274.5_f64), None],
/// )?;
/// odc::write_odb(&df, "out.odb", &odc::WriteOptions::default())?;
/// # Ok::<(), odc::Error>(())
/// ```
///
/// # Errors
///
/// Fails on an empty `DataFrame`, unsupported dtypes, invalid type
/// overrides or bitfield specifications, or if the file cannot be written.
pub fn write_odb(df: &DataFrame, path: impl AsRef<Path>, options: &WriteOptions) -> Result<()> {
    init();
    let handle = eckit::DataHandle::from_path(path)?;
    let mut handle = handle.open_for_write(0)?;
    let result = write_odb_to(df, &mut handle, options);
    let closed = handle.close();
    result?;
    closed?;
    Ok(())
}

/// Encode a `DataFrame` into an open eckit
/// [`DataHandle`](eckit::DataHandle) (file, buffer, tee, …).
///
/// # Errors
///
/// See [`write_odb`].
pub fn write_odb_to(
    df: &DataFrame,
    handle: &mut eckit::DataHandle<eckit::Writing>,
    options: &WriteOptions,
) -> Result<()> {
    init();
    let nrows = df.height();
    if nrows == 0 || df.width() == 0 {
        return Err(Error::EmptyDataFrame);
    }

    let mut prepared: Vec<(String, ColumnType, Series)> = Vec::with_capacity(df.width());
    for column in df.columns() {
        let name = column.name().to_string();
        let series = column.as_materialized_series().rechunk();

        let natural = match series.dtype() {
            DataType::Int64
            | DataType::Int8
            | DataType::Int16
            | DataType::Int32
            | DataType::UInt8
            | DataType::UInt16
            | DataType::UInt32
            | DataType::Boolean => ColumnType::Integer,
            DataType::Float64 => ColumnType::Double,
            DataType::Float32 => ColumnType::Real,
            DataType::String => ColumnType::String,
            other => {
                return Err(Error::UnsupportedDtype {
                    column: name,
                    dtype: other.to_string(),
                });
            }
        };

        let target = options.types.get(&name).copied().unwrap_or(natural);
        let compatible = target == natural
            || (natural == ColumnType::Integer && target == ColumnType::Bitfield)
            || (natural == ColumnType::Double && target == ColumnType::Real)
            || (natural == ColumnType::Real && target == ColumnType::Double);
        if !compatible {
            return Err(Error::InvalidTypeOverride {
                column: name,
                from: natural,
                to: target,
            });
        }
        if target == ColumnType::Bitfield {
            validate_bitfield(&name, options.bitfields.get(&name))?;
        }

        prepared.push((name, target, series));
    }

    let missing_int = SettingsWrapper::integer_missing_value();
    let missing_dbl = SettingsWrapper::double_missing_value();
    let staged: Vec<Staged> = prepared
        .iter()
        .map(|(_, _, series)| stage(series, missing_int, missing_dbl))
        .collect::<Result<_>>()?;

    let specs: Vec<ColumnSpec> = prepared
        .iter()
        .zip(&staged)
        .map(|((name, column_type, _), data)| ColumnSpec {
            name,
            column_type: *column_type,
            ptr: data.as_ptr(),
            elem_size: data.elem_size(),
            stride: data.elem_size(),
        })
        .collect();
    // SAFETY: borrowed slices point into `prepared` and owned buffers live
    // in `staged`, both until after the encode call.
    unsafe { encode_columns(&specs, nrows, handle, options) }
}

/// Caller-owned source data for one column of [`write_odb_raw`].
///
/// All slots are 8 bytes, matching the encoded ODB layout. Missing values
/// are in-band: [`crate::integer_missing_value`] in `I64` data and
/// [`crate::double_missing_value`] in `F64` data. `Str` holds fixed-width
/// NUL-padded cells of `width` bytes.
pub enum EncodeSource<'a> {
    I64(&'a [i64]),
    F64(&'a [f64]),
    Str { data: &'a [u8], width: usize },
}

/// One column of [`write_odb_raw`].
pub struct RawColumn<'a> {
    pub name: &'a str,
    /// `Integer` or `Bitfield` for `I64` data, `Double` or `Real` for
    /// `F64`, `String` for `Str`.
    pub column_type: ColumnType,
    pub data: EncodeSource<'a>,
}

/// Encode raw column slices into an ODB-2 file, without a `DataFrame`.
///
/// [`WriteOptions::types`] is ignored — each column's type is explicit.
///
/// # Example
///
/// ```no_run
/// use odc::{ColumnType, EncodeSource, RawColumn, WriteOptions};
///
/// let seqno = [1_i64, 2, 3];
/// let value = [274.5_f64, odc::double_missing_value(), 271.9];
/// let columns = [
///     RawColumn {
///         name: "seqno@hdr",
///         column_type: ColumnType::Integer,
///         data: EncodeSource::I64(&seqno),
///     },
///     RawColumn {
///         name: "obsvalue@body",
///         column_type: ColumnType::Double,
///         data: EncodeSource::F64(&value),
///     },
/// ];
/// odc::write_odb_raw(&columns, "out.odb", &WriteOptions::default())?;
/// # Ok::<(), odc::Error>(())
/// ```
///
/// # Errors
///
/// Fails on empty input, a buffer that does not fit its column type,
/// mismatched row counts, invalid bitfield specifications, or if the file
/// cannot be written.
pub fn write_odb_raw(
    columns: &[RawColumn<'_>],
    path: impl AsRef<Path>,
    options: &WriteOptions,
) -> Result<()> {
    init();
    let handle = eckit::DataHandle::from_path(path)?;
    let mut handle = handle.open_for_write(0)?;
    let result = write_odb_raw_to(columns, &mut handle, options);
    let closed = handle.close();
    result?;
    closed?;
    Ok(())
}

/// Encode raw column slices into an open eckit
/// [`DataHandle`](eckit::DataHandle).
///
/// # Errors
///
/// See [`write_odb_raw`].
pub fn write_odb_raw_to(
    columns: &[RawColumn<'_>],
    handle: &mut eckit::DataHandle<eckit::Writing>,
    options: &WriteOptions,
) -> Result<()> {
    init();
    let mut specs = Vec::with_capacity(columns.len());
    let mut nrows = None;
    for column in columns {
        let (ptr, elem_size, rows) = raw_spec(column)?;
        match nrows {
            None => nrows = Some(rows),
            Some(expected) if expected != rows => {
                return Err(Error::InvalidBuffer {
                    column: column.name.to_string(),
                    reason: format!("has {rows} rows, expected {expected}"),
                });
            }
            Some(_) => {}
        }
        if column.column_type == ColumnType::Bitfield {
            validate_bitfield(column.name, options.bitfields.get(column.name))?;
        }
        specs.push(ColumnSpec {
            name: column.name,
            column_type: column.column_type,
            ptr,
            elem_size,
            stride: elem_size,
        });
    }
    let Some(nrows) = nrows.filter(|&rows| rows > 0) else {
        return Err(Error::EmptyDataFrame);
    };
    // SAFETY: the spec pointers borrow from `columns`, alive until after
    // the encode call.
    unsafe { encode_columns(&specs, nrows, handle, options) }
}

/// One column of [`write_odb_strided`] and
/// [`decode_strided`](crate::Frame::decode_strided): a periodic layout
/// within a shared buffer of 8-byte cells.
pub struct StridedColumn<'a> {
    pub name: &'a str,
    /// Any type except `Ignore`; `Bitfield` requires a
    /// [`WriteOptions::bitfields`] entry when encoding.
    pub column_type: ColumnType,
    /// Element size in bytes — 8, except for string columns, which may
    /// span several 8-byte cells.
    pub size: usize,
    /// Byte offset of the first element within the cell buffer.
    pub offset: usize,
    /// Byte distance between consecutive elements.
    pub stride: usize,
}

/// Validates the layout of one strided column against a buffer of
/// `buffer_bytes` bytes holding `nrows` elements per column.
pub fn checked_strided(
    column: &StridedColumn<'_>,
    nrows: usize,
    buffer_bytes: usize,
) -> Result<()> {
    let mismatch = |reason: String| Error::InvalidBuffer {
        column: column.name.to_string(),
        reason,
    };
    if column.column_type == ColumnType::Ignore {
        return Err(Error::UnsupportedColumnType {
            column: column.name.to_string(),
            column_type: column.column_type,
        });
    }
    if column.size == 0 || !column.size.is_multiple_of(8) {
        return Err(mismatch(format!(
            "element size {} is not a positive multiple of 8",
            column.size
        )));
    }
    if column.size != 8 && column.column_type != ColumnType::String {
        return Err(mismatch(format!(
            "element size {} on a {:?} column",
            column.size, column.column_type
        )));
    }
    if !column.offset.is_multiple_of(8) || !column.stride.is_multiple_of(8) {
        return Err(mismatch(format!(
            "offset {} and stride {} must be multiples of 8",
            column.offset, column.stride
        )));
    }
    if column.stride < column.size {
        return Err(mismatch(format!(
            "stride {} is less than the element size {}",
            column.stride, column.size
        )));
    }
    let end = (nrows - 1)
        .checked_mul(column.stride)
        .and_then(|span| span.checked_add(column.offset))
        .and_then(|start| start.checked_add(column.size));
    match end {
        Some(end) if end <= buffer_bytes => Ok(()),
        _ => Err(mismatch(format!(
            "layout of {nrows} rows does not fit a buffer of {buffer_bytes} bytes"
        ))),
    }
}

/// Encode columns laid out with periodic strides within a shared buffer of
/// 8-byte cells into an ODB-2 file.
///
/// Within the buffer, an integer or bitfield element holds an `i64` bit
/// pattern, a real or double element holds an `f64` bit pattern, and a
/// string element holds NUL-padded bytes. [`WriteOptions::types`] is
/// ignored: each column's type is explicit.
///
/// # Example
///
/// A column-major layout: each column occupies a contiguous run of cells.
///
/// ```no_run
/// use odc::{ColumnType, StridedColumn, WriteOptions};
///
/// let nrows = 3;
/// let cells = [
///     u64::from_ne_bytes(1_i64.to_ne_bytes()),
///     u64::from_ne_bytes(2_i64.to_ne_bytes()),
///     u64::from_ne_bytes(3_i64.to_ne_bytes()),
///     274.5_f64.to_bits(),
///     272.1_f64.to_bits(),
///     271.9_f64.to_bits(),
/// ];
/// let columns = [
///     StridedColumn {
///         name: "seqno@hdr",
///         column_type: ColumnType::Integer,
///         size: 8,
///         offset: 0,
///         stride: 8,
///     },
///     StridedColumn {
///         name: "obsvalue@body",
///         column_type: ColumnType::Double,
///         size: 8,
///         offset: 3 * 8,
///         stride: 8,
///     },
/// ];
/// odc::write_odb_strided(&cells, &columns, nrows, "out.odb", &WriteOptions::default())?;
/// # Ok::<(), odc::Error>(())
/// ```
///
/// # Errors
///
/// Fails on an invalid column layout, a layout that does not fit the
/// buffer, an empty input, an invalid bitfield specification, or if the
/// file cannot be written.
pub fn write_odb_strided(
    cells: &[u64],
    columns: &[StridedColumn<'_>],
    nrows: usize,
    path: impl AsRef<Path>,
    options: &WriteOptions,
) -> Result<()> {
    init();
    let handle = eckit::DataHandle::from_path(path)?;
    let mut handle = handle.open_for_write(0)?;
    let result = write_odb_strided_to(cells, columns, nrows, &mut handle, options);
    let closed = handle.close();
    result?;
    closed?;
    Ok(())
}

/// Encode columns laid out with periodic strides within a shared buffer of
/// 8-byte cells into an open eckit [`DataHandle`](eckit::DataHandle).
///
/// # Errors
///
/// See [`write_odb_strided`].
pub fn write_odb_strided_to(
    cells: &[u64],
    columns: &[StridedColumn<'_>],
    nrows: usize,
    handle: &mut eckit::DataHandle<eckit::Writing>,
    options: &WriteOptions,
) -> Result<()> {
    init();
    if nrows == 0 || columns.is_empty() {
        return Err(Error::EmptyDataFrame);
    }
    let buffer_bytes = cells.len() * 8;
    let mut specs = Vec::with_capacity(columns.len());
    for column in columns {
        checked_strided(column, nrows, buffer_bytes)?;
        if column.column_type == ColumnType::Bitfield {
            validate_bitfield(column.name, options.bitfields.get(column.name))?;
        }
        // SAFETY: checked_strided keeps the offset within the buffer.
        let ptr = unsafe { cells.as_ptr().cast::<u8>().add(column.offset) };
        specs.push(ColumnSpec {
            name: column.name,
            column_type: column.column_type,
            ptr,
            elem_size: column.size,
            stride: column.stride,
        });
    }
    // SAFETY: the spec pointers borrow from `cells`, alive until after the
    // encode call, and checked_strided keeps every element within it.
    unsafe { encode_columns(&specs, nrows, handle, options) }
}

/// One column of the row-major and column-major cell layouts.
///
/// Used by [`write_odb_row_major`], [`write_odb_column_major`],
/// [`decode_row_major`](crate::Frame::decode_row_major) and
/// [`decode_column_major`](crate::Frame::decode_column_major).
pub struct CellColumn<'a> {
    pub name: &'a str,
    /// Any type except `Ignore`; `Bitfield` requires a
    /// [`WriteOptions::bitfields`] entry.
    pub column_type: ColumnType,
    /// Cell size in bytes — 8, except for string columns, which may span
    /// several 8-byte cells.
    pub size: usize,
}

/// Encode rows of 8-byte cells into an ODB-2 file.
///
/// `cells` holds consecutive rows, each as wide as the summed column sizes.
/// Within a row, an integer or bitfield cell holds an `i64` bit pattern, a
/// real or double cell holds an `f64` bit pattern, and a string column's
/// cells hold NUL-padded bytes.
///
/// [`WriteOptions::types`] is ignored — each column's type is explicit.
///
/// # Example
///
/// ```no_run
/// use odc::{ColumnType, CellColumn, WriteOptions};
///
/// let columns = [
///     CellColumn {
///         name: "seqno@hdr",
///         column_type: ColumnType::Integer,
///         size: 8,
///     },
///     CellColumn {
///         name: "obsvalue@body",
///         column_type: ColumnType::Double,
///         size: 8,
///     },
/// ];
/// let cells = [
///     u64::from_ne_bytes(1_i64.to_ne_bytes()),
///     274.5_f64.to_bits(),
///     u64::from_ne_bytes(2_i64.to_ne_bytes()),
///     271.9_f64.to_bits(),
/// ];
/// odc::write_odb_row_major(&cells, &columns, "out.odb", &WriteOptions::default())?;
/// # Ok::<(), odc::Error>(())
/// ```
///
/// # Errors
///
/// Fails on an invalid column size, a `cells` length that is not a whole
/// number of rows, an empty buffer, an invalid bitfield specification, or
/// if the file cannot be written.
pub fn write_odb_row_major(
    cells: &[u64],
    columns: &[CellColumn<'_>],
    path: impl AsRef<Path>,
    options: &WriteOptions,
) -> Result<()> {
    init();
    let handle = eckit::DataHandle::from_path(path)?;
    let mut handle = handle.open_for_write(0)?;
    let result = write_odb_row_major_to(cells, columns, &mut handle, options);
    let closed = handle.close();
    result?;
    closed?;
    Ok(())
}

/// Encode rows of 8-byte cells into an open eckit
/// [`DataHandle`](eckit::DataHandle).
///
/// # Errors
///
/// See [`write_odb_row_major`].
pub fn write_odb_row_major_to(
    cells: &[u64],
    columns: &[CellColumn<'_>],
    handle: &mut eckit::DataHandle<eckit::Writing>,
    options: &WriteOptions,
) -> Result<()> {
    init();
    let (row_bytes, nrows) = cell_rows(columns, cells.len())?;
    write_odb_strided_to(
        cells,
        &row_major_layout(columns, row_bytes),
        nrows,
        handle,
        options,
    )
}

/// Encode columns stored as a column-major block of 8-byte cells — each
/// column a contiguous run of elements, columns arranged sequentially —
/// into an ODB-2 file.
///
/// The cell contents follow [`write_odb_row_major`]; the number of rows is
/// the buffer size divided by the combined column sizes.
///
/// # Errors
///
/// See [`write_odb_row_major`].
pub fn write_odb_column_major(
    cells: &[u64],
    columns: &[CellColumn<'_>],
    path: impl AsRef<Path>,
    options: &WriteOptions,
) -> Result<()> {
    init();
    let handle = eckit::DataHandle::from_path(path)?;
    let mut handle = handle.open_for_write(0)?;
    let result = write_odb_column_major_to(cells, columns, &mut handle, options);
    let closed = handle.close();
    result?;
    closed?;
    Ok(())
}

/// Encode columns stored as a column-major block of 8-byte cells into an
/// open eckit [`DataHandle`](eckit::DataHandle).
///
/// # Errors
///
/// See [`write_odb_row_major`].
pub fn write_odb_column_major_to(
    cells: &[u64],
    columns: &[CellColumn<'_>],
    handle: &mut eckit::DataHandle<eckit::Writing>,
    options: &WriteOptions,
) -> Result<()> {
    init();
    let (_, nrows) = cell_rows(columns, cells.len())?;
    write_odb_strided_to(
        cells,
        &column_major_layout(columns, nrows),
        nrows,
        handle,
        options,
    )
}

/// Validates the cell sizes of a row of columns and splits a buffer of
/// `len` cells into whole rows, returning the row width in bytes and the
/// row count.
pub fn cell_rows(columns: &[CellColumn<'_>], len: usize) -> Result<(usize, usize)> {
    let mut row_bytes = 0_usize;
    for column in columns {
        if column.size == 0 || !column.size.is_multiple_of(8) {
            return Err(Error::InvalidBuffer {
                column: column.name.to_string(),
                reason: format!("cell size {} is not a positive multiple of 8", column.size),
            });
        }
        if column.size != 8 && column.column_type != ColumnType::String {
            return Err(Error::InvalidBuffer {
                column: column.name.to_string(),
                reason: format!(
                    "cell size {} on a {:?} column",
                    column.size, column.column_type
                ),
            });
        }
        row_bytes += column.size;
    }
    let row_cells = row_bytes / 8;
    if row_cells == 0 {
        return Err(Error::EmptyDataFrame);
    }
    if !len.is_multiple_of(row_cells) {
        return Err(Error::InvalidCellLayout(format!(
            "{len} cells is not a whole number of {row_cells}-cell rows"
        )));
    }
    Ok((row_bytes, len / row_cells))
}

/// The strided form of a row-major cell layout: offsets accumulate within
/// the row, every column strides by the row width.
pub fn row_major_layout<'a>(
    columns: &'a [CellColumn<'a>],
    row_bytes: usize,
) -> Vec<StridedColumn<'a>> {
    let mut offset = 0_usize;
    columns
        .iter()
        .map(|column| {
            let strided = StridedColumn {
                name: column.name,
                column_type: column.column_type,
                size: column.size,
                offset,
                stride: row_bytes,
            };
            offset += column.size;
            strided
        })
        .collect()
}

/// The strided form of a column-major cell layout: each column a
/// contiguous run of `nrows` elements, columns arranged sequentially.
pub fn column_major_layout<'a>(
    columns: &'a [CellColumn<'a>],
    nrows: usize,
) -> Vec<StridedColumn<'a>> {
    let mut offset = 0_usize;
    columns
        .iter()
        .map(|column| {
            let strided = StridedColumn {
                name: column.name,
                column_type: column.column_type,
                size: column.size,
                offset,
                stride: column.size,
            };
            offset += column.size * nrows;
            strided
        })
        .collect()
}

fn raw_spec(column: &RawColumn<'_>) -> Result<(*const u8, usize, usize)> {
    let mismatch = |reason: String| Error::InvalidBuffer {
        column: column.name.to_string(),
        reason,
    };
    match (&column.data, column.column_type) {
        (EncodeSource::I64(data), ColumnType::Integer | ColumnType::Bitfield) => {
            Ok((data.as_ptr().cast(), 8, data.len()))
        }
        (EncodeSource::F64(data), ColumnType::Double | ColumnType::Real) => {
            Ok((data.as_ptr().cast(), 8, data.len()))
        }
        (EncodeSource::Str { data, width }, ColumnType::String) => {
            if *width == 0 || *width % 8 != 0 {
                return Err(mismatch(format!(
                    "string width {width} is not a positive multiple of 8"
                )));
            }
            if data.len() % *width != 0 {
                return Err(mismatch(format!(
                    "{} bytes is not a whole number of {width}-byte cells",
                    data.len()
                )));
            }
            Ok((data.as_ptr(), *width, data.len() / *width))
        }
        (_, column_type) => Err(mismatch(format!(
            "data does not match column type {column_type:?}"
        ))),
    }
}

struct ColumnSpec<'a> {
    name: &'a str,
    column_type: ColumnType,
    ptr: *const u8,
    elem_size: usize,
    stride: usize,
}

/// # Safety
///
/// Every `spec.ptr` must point to at least
/// `(nrows - 1) * spec.stride + spec.elem_size` bytes that stay alive until
/// this returns.
unsafe fn encode_columns(
    specs: &[ColumnSpec<'_>],
    nrows: usize,
    handle: &mut eckit::DataHandle<eckit::Writing>,
    options: &WriteOptions,
) -> Result<()> {
    let mut encoder = odc_sys::EncoderWrapper::create();
    for spec in specs {
        // SAFETY: guaranteed by the caller.
        unsafe {
            encoder.pin_mut().add_column(
                spec.name,
                spec.column_type,
                spec.elem_size,
                spec.ptr,
                nrows,
                spec.stride,
            );
        }
        if spec.column_type == ColumnType::Bitfield
            && let Some(bits) = options.bitfields.get(spec.name)
        {
            for bit in bits {
                encoder
                    .pin_mut()
                    .add_bitfield(&bit.name, bit.size, bit.offset)?;
            }
        }
    }
    for (key, value) in &options.properties {
        encoder.pin_mut().set_property(key, value);
    }
    if FIRST_ENCODE_DONE.load(Ordering::Acquire) {
        encoder
            .pin_mut()
            .encode(handle.as_sys_mut()?, options.rows_per_frame)?;
    } else {
        let _guard = FIRST_ENCODE_LOCK.lock();
        encoder
            .pin_mut()
            .encode(handle.as_sys_mut()?, options.rows_per_frame)?;
        FIRST_ENCODE_DONE.store(true, Ordering::Release);
    }
    Ok(())
}

// odc's CodecOptimizer lazily fills a static codec map inside the first
// encode without synchronization; serialize encodes until one has
// completed, after which the map is only read.
static FIRST_ENCODE_DONE: AtomicBool = AtomicBool::new(false);
static FIRST_ENCODE_LOCK: parking_lot::Mutex<()> = parking_lot::Mutex::new(());

/// Column data as the encoder consumes it: borrowed straight from the
/// `DataFrame`'s Arrow buffer when the column has no nulls, otherwise an
/// owned copy with nulls materialized as ODB missing-value sentinels.
/// Strings are always re-encoded to fixed-width NUL-padded cells.
enum Staged<'a> {
    I64(&'a [i64]),
    F64(&'a [f64]),
    OwnedI64(Vec<i64>),
    OwnedF64(Vec<f64>),
    Bytes { data: Vec<u8>, width: usize },
}

impl Staged<'_> {
    const fn as_ptr(&self) -> *const u8 {
        match self {
            Self::I64(v) => v.as_ptr().cast(),
            Self::F64(v) => v.as_ptr().cast(),
            Self::OwnedI64(v) => v.as_ptr().cast(),
            Self::OwnedF64(v) => v.as_ptr().cast(),
            Self::Bytes { data, .. } => data.as_ptr(),
        }
    }

    const fn elem_size(&self) -> usize {
        match self {
            Self::I64(_) | Self::F64(_) | Self::OwnedI64(_) | Self::OwnedF64(_) => 8,
            Self::Bytes { width, .. } => *width,
        }
    }
}

fn stage(series: &Series, missing_int: i64, missing_dbl: f64) -> Result<Staged<'_>> {
    match series.dtype() {
        DataType::Int64 => {
            let values = series.i64()?;
            Ok(if values.null_count() == 0 {
                Staged::I64(values.cont_slice()?)
            } else {
                Staged::OwnedI64(values.iter().map(|v| v.unwrap_or(missing_int)).collect())
            })
        }
        DataType::Int32 => Ok(Staged::OwnedI64(widened(series.i32()?, missing_int))),
        DataType::UInt32 => Ok(Staged::OwnedI64(widened(series.u32()?, missing_int))),
        DataType::Boolean => Ok(Staged::OwnedI64(
            series
                .bool()?
                .iter()
                .map(|v| v.map_or(missing_int, i64::from))
                .collect(),
        )),
        DataType::Float64 => {
            let values = series.f64()?;
            Ok(if values.null_count() == 0 {
                Staged::F64(values.cont_slice()?)
            } else {
                Staged::OwnedF64(values.iter().map(|v| v.unwrap_or(missing_dbl)).collect())
            })
        }
        DataType::Float32 => Ok(Staged::OwnedF64(
            series
                .f32()?
                .iter()
                .map(|v| v.map_or(missing_dbl, f64::from))
                .collect(),
        )),
        DataType::String => stage_str(series),
        // Integer dtypes behind polars features this crate does not enable
        // (Int8/Int16/UInt8/UInt16).
        _ => {
            let series = series.cast(&DataType::Int64)?;
            Ok(Staged::OwnedI64(widened(series.i64()?, missing_int)))
        }
    }
}

fn widened<T>(values: &ChunkedArray<T>, missing: i64) -> Vec<i64>
where
    T: PolarsIntegerType,
    i64: From<T::Native>,
{
    values
        .iter()
        .map(|v| v.map_or(missing, i64::from))
        .collect()
}

fn stage_str(series: &Series) -> Result<Staged<'_>> {
    // Fixed width: longest value rounded up to a multiple of 8 (min 8),
    // NUL-padded. Nulls encode as the empty string.
    let values = series.str()?;
    let longest = values.iter().flatten().map(str::len).max().unwrap_or(0);
    let width = longest.max(1).div_ceil(8) * 8;
    let mut data = vec![0_u8; values.len() * width];
    for (row, value) in values.iter().enumerate() {
        if let Some(value) = value {
            data[row * width..row * width + value.len()].copy_from_slice(value.as_bytes());
        }
    }
    Ok(Staged::Bytes { data, width })
}

fn validate_bitfield(column: &str, bits: Option<&Vec<Bit>>) -> Result<()> {
    let bits = bits.ok_or_else(|| Error::InvalidBitfield(column.to_string()))?;
    if bits.is_empty() {
        return Err(Error::InvalidBitfield(column.to_string()));
    }
    let mut next_free = 0_i32;
    for bit in bits {
        // Groups must be ordered, non-overlapping and fit in 32 bits.
        if bit.size <= 0 || bit.offset < next_free || bit.offset + bit.size > 32 {
            return Err(Error::InvalidBitfield(column.to_string()));
        }
        next_free = bit.offset + bit.size;
    }
    Ok(())
}
