//! Encoding a Polars `DataFrame` into ODB-2.

use std::collections::{BTreeMap, HashMap};
use std::path::Path;

use odc_sys::{Bit, ColumnType, SettingsWrapper};
use polars::prelude::*;

use crate::error::{Error, Result};
use crate::init;

/// Options for [`write_odb`].
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

    let missing_int = SettingsWrapper::integer_missing_value();
    let missing_dbl = SettingsWrapper::double_missing_value();

    let mut staged: Vec<(String, ColumnType, Staged)> = Vec::with_capacity(df.width());
    for column in df.columns() {
        let name = column.name().to_string();
        let series = column.as_materialized_series().rechunk();

        let (natural, data) = match series.dtype() {
            DataType::Int64 => (ColumnType::Integer, stage_i64(&series, missing_int)?),
            DataType::Int8
            | DataType::Int16
            | DataType::Int32
            | DataType::UInt8
            | DataType::UInt16
            | DataType::UInt32
            | DataType::Boolean => (
                ColumnType::Integer,
                stage_i64(&series.cast(&DataType::Int64)?, missing_int)?,
            ),
            DataType::Float64 => (ColumnType::Double, stage_f64(&series, missing_dbl)?),
            DataType::Float32 => (
                ColumnType::Real,
                stage_f64(&series.cast(&DataType::Float64)?, missing_dbl)?,
            ),
            DataType::String => (ColumnType::String, stage_str(&series)),
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

        staged.push((name, target, data));
    }

    let mut encoder = odc_sys::EncoderWrapper::create();
    for (name, column_type, data) in &staged {
        let elem_size = data.elem_size();
        // SAFETY: the staged buffers live in `staged` until after the
        // encode call below.
        unsafe {
            encoder.pin_mut().add_column(
                name,
                *column_type,
                elem_size,
                data.as_ptr(),
                nrows,
                elem_size,
            );
        }
        if *column_type == ColumnType::Bitfield
            && let Some(bits) = options.bitfields.get(name)
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
        .encode(handle.inner_mut()?, options.rows_per_frame)?;
    Ok(())
}

/// Staged (contiguous, null-resolved) source data for one column.
enum Staged {
    I64(Vec<i64>),
    F64(Vec<f64>),
    Bytes { data: Vec<u8>, width: usize },
}

impl Staged {
    const fn as_ptr(&self) -> *const u8 {
        match self {
            Self::I64(v) => v.as_ptr().cast(),
            Self::F64(v) => v.as_ptr().cast(),
            Self::Bytes { data, .. } => data.as_ptr(),
        }
    }

    const fn elem_size(&self) -> usize {
        match self {
            Self::I64(_) | Self::F64(_) => 8,
            Self::Bytes { width, .. } => *width,
        }
    }
}

fn stage_i64(series: &Series, missing: i64) -> Result<Staged> {
    let values = series.i64()?;
    Ok(Staged::I64(
        values.iter().map(|v| v.unwrap_or(missing)).collect(),
    ))
}

fn stage_f64(series: &Series, missing: f64) -> Result<Staged> {
    let values = series.f64()?;
    Ok(Staged::F64(
        values.iter().map(|v| v.unwrap_or(missing)).collect(),
    ))
}

fn stage_str(series: &Series) -> Staged {
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
