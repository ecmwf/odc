//! [`Frame`] — a decodable chunk of an ODB-2 stream.

use std::collections::BTreeMap;
use std::sync::Arc;

use polars::prelude::DataFrame;

use crate::decode::{self, DecodeTarget};
use crate::error::Result;
use crate::reader::ReaderShared;
use crate::span::Span;
use odc_sys::ColumnInfo;

/// Options for decoding a [`Frame`] into a `DataFrame`.
#[derive(Debug, Clone)]
pub struct DecodeOptions {
    /// Columns to decode, in the requested order. `None` decodes all.
    pub columns: Option<Vec<String>>,
    /// Number of decode threads. Parallelism applies across the physical
    /// frames of an aggregated logical frame.
    pub threads: usize,
}

impl Default for DecodeOptions {
    fn default() -> Self {
        Self {
            columns: None,
            threads: 1,
        }
    }
}

/// A viewport onto a chunk of contiguous, compatible data within an ODB-2
/// stream — possibly a logical frame aggregating several physical frames.
///
/// Column metadata and properties are available without decoding; the data
/// itself decodes into a Polars `DataFrame` via [`Frame::dataframe`].
pub struct Frame {
    inner: odc_sys::UniquePtr<odc_sys::FrameWrapper>,
    columns: Vec<ColumnInfo>,
    properties: BTreeMap<String, String>,
    // Frames read lazily from the reader's stream — keep it alive.
    _reader: Arc<ReaderShared>,
}

impl Frame {
    pub(crate) fn new(
        inner: odc_sys::UniquePtr<odc_sys::FrameWrapper>,
        reader: Arc<ReaderShared>,
    ) -> Result<Self> {
        let columns = inner.column_info()?;
        let properties = inner
            .properties()?
            .into_iter()
            .map(|p| (p.key, p.value))
            .collect();
        Ok(Self {
            inner,
            columns,
            properties,
            _reader: reader,
        })
    }

    pub(crate) fn wrapper(&self) -> &odc_sys::FrameWrapper {
        &self.inner
    }

    /// Number of rows.
    #[must_use]
    pub fn row_count(&self) -> usize {
        self.inner.row_count()
    }

    /// Number of columns.
    #[must_use]
    pub fn column_count(&self) -> usize {
        self.inner.column_count()
    }

    /// Column metadata, in frame order.
    #[must_use]
    pub fn columns(&self) -> &[ColumnInfo] {
        &self.columns
    }

    /// Metadata of the named column.
    #[must_use]
    pub fn column(&self, name: &str) -> Option<&ColumnInfo> {
        self.columns.iter().find(|c| c.name == name)
    }

    /// Whether the frame has a column with this name.
    #[must_use]
    pub fn has_column(&self, name: &str) -> bool {
        self.columns.iter().any(|c| c.name == name)
    }

    /// Key/value properties encoded in the frame.
    #[must_use]
    pub const fn properties(&self) -> &BTreeMap<String, String> {
        &self.properties
    }

    /// The sets of values of the named columns, and the frame's byte range
    /// in the stream, determined without decoding the frame.
    ///
    /// With `only_constant`, every named column must hold a single constant
    /// value across the frame.
    ///
    /// # Errors
    ///
    /// Fails if a named column does not exist, or the `only_constant`
    /// constraint is violated.
    pub fn span(&self, columns: &[&str], only_constant: bool) -> Result<Span> {
        let names = columns.iter().map(ToString::to_string).collect();
        Ok(Span {
            inner: self.inner.span(&names, only_constant)?,
        })
    }

    /// Decode all columns into a `DataFrame`.
    ///
    /// Missing values become nulls; see [`crate::read_odb`] for the full
    /// type mapping.
    ///
    /// # Errors
    ///
    /// Fails if the underlying stream cannot be read or decoded.
    pub fn dataframe(&self) -> Result<DataFrame> {
        decode::dataframe(self, &DecodeOptions::default())
    }

    /// Decode selected columns into a `DataFrame`.
    ///
    /// # Example
    ///
    /// ```no_run
    /// # let reader = odc::Reader::from_path("data.odb")?;
    /// # let frame = reader.frames().next().unwrap()?;
    /// let options = odc::DecodeOptions {
    ///     columns: Some(vec!["expver".into(), "date@hdr".into()]),
    ///     ..Default::default()
    /// };
    /// let df = frame.dataframe_with(&options)?;
    /// # Ok::<(), odc::Error>(())
    /// ```
    ///
    /// # Errors
    ///
    /// Fails if a requested column does not exist or the underlying stream
    /// cannot be read or decoded.
    pub fn dataframe_with(&self, options: &DecodeOptions) -> Result<DataFrame> {
        decode::dataframe(self, options)
    }

    /// Decode the named columns into caller-allocated buffers.
    ///
    /// Raw output, unlike [`Frame::dataframe`]: missing values keep their
    /// ODB sentinels ([`crate::integer_missing_value`] /
    /// [`crate::double_missing_value`]) and strings stay fixed-width
    /// NUL-padded cells. [`Frame::columns`] gives each string column's
    /// cell width as `decoded_size`.
    ///
    /// Returns the number of rows decoded.
    ///
    /// # Example
    ///
    /// ```no_run
    /// use odc::DecodeTarget;
    ///
    /// # let reader = odc::Reader::from_path("data.odb")?;
    /// # let frame = reader.frames().next().unwrap()?;
    /// let mut seqno = vec![0_i64; frame.row_count()];
    /// let mut obsvalue = vec![0.0_f64; frame.row_count()];
    /// frame.decode_into(
    ///     &mut [
    ///         ("seqno@hdr", DecodeTarget::I64(&mut seqno)),
    ///         ("obsvalue@body", DecodeTarget::F64(&mut obsvalue)),
    ///     ],
    ///     1,
    /// )?;
    /// # Ok::<(), odc::Error>(())
    /// ```
    ///
    /// # Errors
    ///
    /// Fails if a column does not exist, a buffer does not fit its column,
    /// or the underlying stream cannot be read or decoded.
    pub fn decode_into(
        &self,
        columns: &mut [(&str, DecodeTarget<'_>)],
        threads: usize,
    ) -> Result<usize> {
        decode::into_buffers(self, columns, threads)
    }
}
