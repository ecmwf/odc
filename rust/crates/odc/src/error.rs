//! Error types for odc operations.

use odc_sys::ColumnType;

/// Errors returned by odc operations.
#[derive(Debug, thiserror::Error)]
pub enum Error {
    /// Typed odc C++ exception (auto-generated in `odc-sys`).
    #[error(transparent)]
    Odc(#[from] odc_sys::Error),

    /// Typed eckit C++ exception (e.g. from `DataHandle` operations).
    #[error(transparent)]
    Eckit(#[from] eckit::Error),

    /// Polars error while building or consuming a `DataFrame`.
    #[error(transparent)]
    Polars(#[from] polars::prelude::PolarsError),

    /// A `DataFrame` column has a dtype that cannot be encoded to ODB-2.
    #[error("column '{column}' has unsupported dtype {dtype} for ODB encoding")]
    UnsupportedDtype { column: String, dtype: String },

    /// A requested ODB column cannot be decoded (e.g. type `Ignore`).
    #[error("column '{column}' has unsupported ODB type {column_type:?}")]
    UnsupportedColumnType {
        column: String,
        column_type: ColumnType,
    },

    /// A type override in [`WriteOptions::types`](crate::WriteOptions::types)
    /// is not compatible with the column's dtype.
    #[error("column '{column}': cannot encode {from:?} data as {to:?}")]
    InvalidTypeOverride {
        column: String,
        from: ColumnType,
        to: ColumnType,
    },

    /// Column not found in the frame.
    #[error("column not found: {0}")]
    ColumnNotFound(String),

    /// Encoding requires at least one row and one column.
    #[error("cannot encode an empty DataFrame")]
    EmptyDataFrame,

    /// Invalid bitfield specification (missing, oversized or overlapping).
    #[error("invalid bitfield specification for column '{0}'")]
    InvalidBitfield(String),

    /// I/O error.
    #[error("I/O error: {0}")]
    Io(#[from] std::io::Error),
}

/// Result type alias for odc operations.
pub type Result<T> = std::result::Result<T, Error>;

impl From<odc_sys::Exception> for Error {
    fn from(e: odc_sys::Exception) -> Self {
        // The trycatch bridge prefixes messages with the exception's
        // namespace — try odc's typed errors first, then eckit's.
        if let Some(err) = odc_sys::Error::try_from_cxx(&e) {
            return Self::Odc(err);
        }
        if let Some(err) = eckit::Error::try_from_cxx(&e) {
            return Self::Eckit(err);
        }
        Self::Odc(odc_sys::Error::Other(e.what().to_string()))
    }
}
