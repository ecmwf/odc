//! Decoding a [`Frame`] into a Polars `DataFrame`.

use odc_sys::{ColumnInfo, ColumnType, SettingsWrapper};
use polars::prelude::*;
use polars_arrow::bitmap::Bitmap;

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

/// Caller-owned destination buffers for [`Frame::decode_into`].
///
/// All slots are 8 bytes, matching the decoded ODB layout: INTEGER and
/// BITFIELD decode as `i64`, REAL and DOUBLE as `f64`, STRING as
/// fixed-width NUL-padded cells of `width` bytes held in native-endian
/// `u64` slots.
pub enum DecodeTarget<'a> {
    I64(&'a mut [i64]),
    F64(&'a mut [f64]),
    Str { data: &'a mut [u64], width: usize },
}

pub fn into_buffers(
    frame: &Frame,
    columns: &mut [(&str, DecodeTarget<'_>)],
    threads: usize,
) -> Result<usize> {
    crate::init();
    let nrows = frame.row_count();
    let mut decoder = odc_sys::DecoderWrapper::create();
    for (name, target) in columns.iter_mut() {
        let col = frame
            .column(name)
            .ok_or_else(|| Error::ColumnNotFound((*name).to_string()))?;
        let (ptr, elem_size) = checked_target(col, target, nrows)?;
        // SAFETY: checked_target guarantees 8-byte-aligned caller memory of
        // nrows * elem_size bytes, borrowed until decode returns.
        unsafe {
            decoder
                .pin_mut()
                .add_column(&col.name, ptr, nrows, elem_size, elem_size);
        }
    }
    Ok(decoder.pin_mut().decode(frame.wrapper(), threads)?)
}

fn checked_target(
    col: &ColumnInfo,
    target: &mut DecodeTarget<'_>,
    nrows: usize,
) -> Result<(*mut u8, usize)> {
    let mismatch = |reason: String| Error::InvalidBuffer {
        column: col.name.clone(),
        reason,
    };
    match (col.column_type, target) {
        (ColumnType::Integer | ColumnType::Bitfield, DecodeTarget::I64(data)) => {
            if data.len() != nrows {
                return Err(mismatch(format!("{} slots for {nrows} rows", data.len())));
            }
            Ok((data.as_mut_ptr().cast(), 8))
        }
        (ColumnType::Real | ColumnType::Double, DecodeTarget::F64(data)) => {
            if data.len() != nrows {
                return Err(mismatch(format!("{} slots for {nrows} rows", data.len())));
            }
            Ok((data.as_mut_ptr().cast(), 8))
        }
        (ColumnType::String, DecodeTarget::Str { data, width }) => {
            if *width == 0 || *width % 8 != 0 {
                return Err(mismatch(format!(
                    "string width {width} is not a positive multiple of 8"
                )));
            }
            if *width < col.decoded_size {
                return Err(mismatch(format!(
                    "width {width} is less than the decoded size {}",
                    col.decoded_size
                )));
            }
            if data.len() * 8 != nrows * *width {
                return Err(mismatch(format!(
                    "{} slots for {nrows} rows of {width}-byte cells",
                    data.len()
                )));
            }
            Ok((data.as_mut_ptr().cast(), *width))
        }
        (ColumnType::Ignore, _) => Err(Error::UnsupportedColumnType {
            column: col.name.clone(),
            column_type: col.column_type,
        }),
        (column_type, _) => Err(mismatch(format!(
            "target does not match column type {column_type:?}"
        ))),
    }
}

pub fn dataframe(frame: &Frame, options: &DecodeOptions) -> Result<DataFrame> {
    crate::init();
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

/// Wraps the decoded buffer as a Series in place: the buffer becomes the
/// backing Arrow data, with missing-value sentinels marked null through a
/// validity bitmap (`None` when the column has no missing values).
fn to_polars(col: &ColumnInfo, buffer: Buffer, nrows: usize) -> Result<Column> {
    let name: PlSmallStr = col.name.as_str().into();
    let series = match buffer {
        // Bitfields are raw bit patterns — no missing-value mapping.
        Buffer::I64(values) if col.column_type == ColumnType::Bitfield => {
            Int64Chunked::from_vec(name, values).into_series()
        }
        Buffer::I64(values) => {
            let missing = SettingsWrapper::integer_missing_value();
            let validity = Bitmap::opt_from_iter(values.iter().map(|&v| v != missing));
            Int64Chunked::from_vec_validity(name, values, validity).into_series()
        }
        Buffer::F64(values) => {
            let missing = SettingsWrapper::double_missing_value().to_bits();
            let validity = Bitmap::opt_from_iter(values.iter().map(|v| v.to_bits() != missing));
            let series = Float64Chunked::from_vec_validity(name, values, validity).into_series();
            if col.column_type == ColumnType::Real {
                series.cast(&DataType::Float32)?
            } else {
                series
            }
        }
        Buffer::Str { data, width } => {
            let slots = width / 8;
            let mut builder = StringChunkedBuilder::new(name, nrows);
            let mut cell = Vec::with_capacity(width);
            for row in 0..nrows {
                cell.clear();
                for slot in &data[row * slots..(row + 1) * slots] {
                    cell.extend_from_slice(&slot.to_ne_bytes());
                }
                let end = cell.iter().rposition(|&b| b != 0).map_or(0, |i| i + 1);
                builder.append_value(String::from_utf8_lossy(&cell[..end]));
            }
            builder.finish().into_series()
        }
    };
    Ok(series.into_column())
}
