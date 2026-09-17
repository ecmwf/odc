//! Encoding a Polars `DataFrame` into ODB-2.

use std::collections::{BTreeMap, HashMap};
use std::path::Path;

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

        let (natural, series) = match series.dtype() {
            DataType::Int64 => (ColumnType::Integer, series),
            DataType::Int8
            | DataType::Int16
            | DataType::Int32
            | DataType::UInt8
            | DataType::UInt16
            | DataType::UInt32
            | DataType::Boolean => (ColumnType::Integer, series.cast(&DataType::Int64)?),
            DataType::Float64 => (ColumnType::Double, series),
            DataType::Float32 => (ColumnType::Real, series.cast(&DataType::Float64)?),
            DataType::String => (ColumnType::String, series),
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
        });
    }
    let Some(nrows) = nrows.filter(|&rows| rows > 0) else {
        return Err(Error::EmptyDataFrame);
    };
    // SAFETY: the spec pointers borrow from `columns`, alive until after
    // the encode call.
    unsafe { encode_columns(&specs, nrows, handle, options) }
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
}

/// # Safety
///
/// Every `spec.ptr` must point to at least `nrows * spec.elem_size` bytes
/// that stay alive until this returns.
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
                spec.elem_size,
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
    encoder
        .pin_mut()
        .encode(handle.as_sys_mut()?, options.rows_per_frame)?;
    Ok(())
}

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
        DataType::Float64 => {
            let values = series.f64()?;
            Ok(if values.null_count() == 0 {
                Staged::F64(values.cont_slice()?)
            } else {
                Staged::OwnedF64(values.iter().map(|v| v.unwrap_or(missing_dbl)).collect())
            })
        }
        DataType::String => Ok(stage_str(series)),
        other => unreachable!("prepared columns are Int64, Float64 or String, got {other}"),
    }
}

fn stage_str(series: &Series) -> Staged<'_> {
    // Fixed width: longest value rounded up to a multiple of 8 (min 8),
    // NUL-padded. Nulls encode as the empty string.
    let values: Vec<Option<&str>> = series
        .str()
        .map_or_else(|_| Vec::new(), |ca| ca.iter().collect());
    let longest = values
        .iter()
        .map(|v| v.map_or(0, str::len))
        .max()
        .unwrap_or(0);
    let width = longest.max(1).div_ceil(8) * 8;
    let mut data = vec![0_u8; series.len() * width];
    for (row, value) in values.iter().enumerate() {
        if let Some(value) = value {
            data[row * width..row * width + value.len()].copy_from_slice(value.as_bytes());
        }
    }
    Staged::Bytes { data, width }
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
