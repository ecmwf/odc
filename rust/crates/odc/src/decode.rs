//! Decoding a [`Frame`] into a Polars `DataFrame`.

use odc_sys::{ColumnInfo, ColumnType, SettingsWrapper};
use polars::prelude::*;

use crate::error::{Error, Result};
use crate::frame::{DecodeOptions, Frame};

/// Decode target for one column. All slots are 8 bytes: with
/// integers-as-longs behaviour, INTEGER/BITFIELD decode as `i64`, REAL and
/// DOUBLE as `f64`, and STRING as fixed-width byte blocks (`Vec<u64>` keeps
/// them 8-byte aligned).
enum Buffer {
    I64(Vec<i64>),
    F64(Vec<f64>),
    Str { data: Vec<u64>, width: usize },
}

impl Buffer {
    fn for_column(col: &ColumnInfo, nrows: usize) -> Self {
        match col.column_type {
            ColumnType::Integer | ColumnType::Bitfield => Self::I64(vec![0; nrows]),
            ColumnType::String => {
                let width = col.decoded_size.max(8);
                Self::Str {
                    data: vec![0; nrows * width / 8],
                    width,
                }
            }
            _ => Self::F64(vec![0.0; nrows]),
        }
    }

    const fn as_mut_ptr(&mut self) -> *mut u8 {
        match self {
            Self::I64(v) => v.as_mut_ptr().cast(),
            Self::F64(v) => v.as_mut_ptr().cast(),
            Self::Str { data, .. } => data.as_mut_ptr().cast(),
        }
    }

    const fn elem_size(&self) -> usize {
        match self {
            Self::I64(_) | Self::F64(_) => 8,
            Self::Str { width, .. } => *width,
        }
    }
}

pub fn dataframe(frame: &Frame, options: &DecodeOptions) -> Result<DataFrame> {
    let nrows = frame.row_count();

    let selected: Vec<&ColumnInfo> = match &options.columns {
        Some(names) => names
            .iter()
            .map(|name| {
                frame
                    .column(name)
                    .ok_or_else(|| Error::ColumnNotFound(name.clone()))
            })
            .collect::<Result<_>>()?,
        None => frame
            .columns()
            .iter()
            .filter(|c| c.column_type != ColumnType::Ignore)
            .collect(),
    };
    if let Some(col) = selected
        .iter()
        .find(|c| c.column_type == ColumnType::Ignore)
    {
        return Err(Error::UnsupportedColumnType {
            column: col.name.clone(),
            column_type: col.column_type,
        });
    }

    // All buffers are allocated up front so no Vec reallocation can move
    // them while the decoder holds raw pointers into them.
    let mut buffers: Vec<Buffer> = selected
        .iter()
        .map(|col| Buffer::for_column(col, nrows))
        .collect();

    let mut decoder = odc_sys::DecoderWrapper::create();
    for (col, buf) in selected.iter().zip(&mut buffers) {
        let elem_size = buf.elem_size();
        // SAFETY: each buffer is 8-byte aligned (Vec<i64>/Vec<f64>/Vec<u64>),
        // holds nrows * elem_size bytes, and outlives the decode call below.
        unsafe {
            decoder
                .pin_mut()
                .add_column(&col.name, buf.as_mut_ptr(), nrows, elem_size, elem_size);
        }
    }
    decoder.pin_mut().decode(frame.wrapper(), options.threads)?;

    let columns = selected
        .iter()
        .zip(buffers)
        .map(|(col, buf)| to_polars(col, buf, nrows))
        .collect::<Result<Vec<Column>>>()?;
    Ok(DataFrame::new(nrows, columns)?)
}

fn to_polars(col: &ColumnInfo, buffer: Buffer, nrows: usize) -> Result<Column> {
    let name: PlSmallStr = col.name.as_str().into();
    let series = match buffer {
        // Bitfields are raw bit patterns — no missing-value mapping.
        Buffer::I64(values) if col.column_type == ColumnType::Bitfield => Series::new(name, values),
        Buffer::I64(values) => {
            let missing = SettingsWrapper::integer_missing_value();
            if values.contains(&missing) {
                let values: Vec<Option<i64>> = values
                    .iter()
                    .map(|&v| (v != missing).then_some(v))
                    .collect();
                Series::new(name, values)
            } else {
                Series::new(name, values)
            }
        }
        Buffer::F64(values) => {
            let missing = SettingsWrapper::double_missing_value().to_bits();
            let series = if values.iter().any(|v| v.to_bits() == missing) {
                let values: Vec<Option<f64>> = values
                    .iter()
                    .map(|&v| (v.to_bits() != missing).then_some(v))
                    .collect();
                Series::new(name, values)
            } else {
                Series::new(name, values)
            };
            if col.column_type == ColumnType::Real {
                series.cast(&DataType::Float32)?
            } else {
                series
            }
        }
        Buffer::Str { data, width } => {
            let slots = width / 8;
            let strings: Vec<String> = (0..nrows)
                .map(|row| {
                    let cell: Vec<u8> = data[row * slots..(row + 1) * slots]
                        .iter()
                        .flat_map(|slot| slot.to_ne_bytes())
                        .collect();
                    let end = cell.iter().rposition(|&b| b != 0).map_or(0, |i| i + 1);
                    String::from_utf8_lossy(&cell[..end]).into_owned()
                })
                .collect();
            Series::new(name, strings)
        }
    };
    Ok(series.into_column())
}
