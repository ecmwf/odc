//! Span access: the sets of values of chosen columns, without decoding.

use odc_sys::ColumnType;

use crate::error::{Error, Result};

/// The set of values of one column of a [`Span`], in ascending order.
#[derive(Debug, Clone, PartialEq)]
pub enum SpanValues {
    /// Values of an integer or bitfield column.
    Integer(Vec<i64>),
    /// Values of a real or double column.
    Real(Vec<f64>),
    /// Values of a string column.
    String(Vec<String>),
}

/// The sets of values of chosen columns within one
/// [`Frame`](crate::Frame), and that frame's byte range in the stream —
/// determined without decoding the frame.
///
/// Two spans compare equal when they cover the same columns with the same
/// value sets, regardless of their position in the stream.
pub struct Span {
    pub(crate) inner: odc_sys::UniquePtr<odc_sys::SpanWrapper>,
}

impl Span {
    /// Byte offset of the frame within the data stream.
    #[must_use]
    pub fn offset(&self) -> u64 {
        self.inner.offset()
    }

    /// Names and values of all spanned columns, in the order requested
    /// when the span was created.
    ///
    /// # Errors
    ///
    /// Fails if the underlying values cannot be read.
    pub fn columns(&self) -> Result<Vec<(String, SpanValues)>> {
        self.inner
            .columns()?
            .into_iter()
            .map(|column| {
                let values = match column.kind {
                    ColumnType::Integer => {
                        SpanValues::Integer(self.inner.integer_values(&column.name)?)
                    }
                    ColumnType::Double => SpanValues::Real(self.inner.real_values(&column.name)?),
                    ColumnType::String => {
                        SpanValues::String(self.inner.string_values(&column.name)?)
                    }
                    kind => {
                        return Err(Error::UnsupportedColumnType {
                            column: column.name,
                            column_type: kind,
                        });
                    }
                };
                Ok((column.name, values))
            })
            .collect()
    }

    /// Length in bytes of the frame's encoded data.
    #[must_use]
    pub fn length(&self) -> u64 {
        self.inner.length()
    }

    /// Integer values present in the named column, in ascending order.
    ///
    /// # Errors
    ///
    /// Fails if the column is not part of the span or holds another type.
    pub fn integer_values(&self, column: &str) -> Result<Vec<i64>> {
        Ok(self.inner.integer_values(column)?)
    }

    /// Floating-point values present in the named column, in ascending
    /// order.
    ///
    /// # Errors
    ///
    /// Fails if the column is not part of the span or holds another type.
    pub fn real_values(&self, column: &str) -> Result<Vec<f64>> {
        Ok(self.inner.real_values(column)?)
    }

    /// String values present in the named column, in ascending order.
    ///
    /// # Errors
    ///
    /// Fails if the column is not part of the span or holds another type.
    pub fn string_values(&self, column: &str) -> Result<Vec<String>> {
        Ok(self.inner.string_values(column)?)
    }
}

impl PartialEq for Span {
    fn eq(&self, other: &Self) -> bool {
        self.inner.equals(&other.inner)
    }
}
