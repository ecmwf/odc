//! Safe Rust wrapper for ECMWF's odc (ODB-2 encoder/decoder) library.
//!
//! ODB-2 data decodes into [Polars](https://pola.rs) `DataFrame`s and
//! encodes from them, following the same model as
//! [pyodc](https://github.com/ecmwf/pyodc):
//!
//! ```no_run
//! // One DataFrame per logical frame, or concatenated:
//! let df = odc::read_odb_single("data.odb", &odc::ReadOptions::default())?;
//! println!("{df}");
//!
//! odc::write_odb(&df, "copy.odb", &odc::WriteOptions::default())?;
//! # Ok::<(), odc::Error>(())
//! ```
//!
//! For streaming access use [`Reader`] and iterate [`Frame`]s, inspecting
//! column metadata and properties before deciding what to decode.
//!
//! # Type mapping
//!
//! | ODB type   | decodes to | encoded from                             |
//! |------------|------------|------------------------------------------|
//! | `Integer`  | `Int64`    | `Int64` (smaller ints/`Boolean` widened) |
//! | `Double`   | `Float64`  | `Float64`                                |
//! | `Real`     | `Float32`  | `Float32`                                |
//! | `String`   | `String`   | `String`                                 |
//! | `Bitfield` | `Int64`    | `Int64` + [`WriteOptions::bitfields`]    |
//!
//! ODB missing values map to nulls in both directions (bitfields excepted).
//!
//! # Process-global state
//!
//! odc stores its integer behaviour and missing-value sentinels globally.
//! This crate pins integers-as-longs on first use, so INTEGER and BITFIELD
//! columns decode as `i64`. Other in-process users of the odc C++ library
//! observe the same setting.

mod decode;
mod encode;
mod error;
mod frame;
mod reader;

pub use encode::{WriteOptions, write_odb, write_odb_to};
pub use error::{Error, Result};
pub use frame::{DecodeOptions, Frame};
pub use odc_sys::{Bit, ColumnInfo, ColumnType, Property};
pub use polars;
pub use reader::{Frames, Reader, ReaderOptions};

use std::path::Path;

use polars::prelude::DataFrame;

/// One-time process-global initialization, called by every public entry
/// point: eckit runtime (with the Rust log bridge) and integers-as-longs
/// decode behaviour.
pub(crate) fn init() {
    static ONCE: std::sync::Once = std::sync::Once::new();
    ONCE.call_once(|| {
        eckit::init();
        odc_sys::SettingsWrapper::treat_integers_as_doubles(false);
    });
}

/// Release version of the odc C++ library, e.g. `1.6.3`.
#[must_use]
pub fn version() -> String {
    init();
    odc_sys::SettingsWrapper::version()
}

/// Version control checksum of the odc C++ library.
#[must_use]
pub fn vcs_version() -> String {
    init();
    odc_sys::SettingsWrapper::gitsha1()
}

/// The sentinel value marking a missing integer in ODB-2 data.
#[must_use]
pub fn integer_missing_value() -> i64 {
    init();
    odc_sys::SettingsWrapper::integer_missing_value()
}

/// The sentinel value marking a missing double in ODB-2 data.
#[must_use]
pub fn double_missing_value() -> f64 {
    init();
    odc_sys::SettingsWrapper::double_missing_value()
}

/// Options for [`read_odb`] and [`read_odb_single`].
#[derive(Debug, Clone)]
pub struct ReadOptions {
    /// Columns to decode, in the requested order. `None` decodes all.
    pub columns: Option<Vec<String>>,
    /// Aggregate consecutive compatible physical frames into logical frames.
    pub aggregated: bool,
    /// Number of decode threads per frame.
    pub threads: usize,
}

impl Default for ReadOptions {
    fn default() -> Self {
        Self {
            columns: None,
            aggregated: true,
            threads: 1,
        }
    }
}

/// Decode an ODB-2 file into one `DataFrame` per logical frame.
///
/// # Errors
///
/// Fails if the file cannot be opened, is not valid ODB-2, or a requested
/// column does not exist.
pub fn read_odb(path: impl AsRef<Path>, options: &ReadOptions) -> Result<Vec<DataFrame>> {
    let reader_options = ReaderOptions {
        aggregated: options.aggregated,
        row_limit: None,
    };
    let decode_options = DecodeOptions {
        columns: options.columns.clone(),
        threads: options.threads,
    };
    Reader::from_path_with(path, &reader_options)?
        .frames()
        .map(|frame| frame?.dataframe_with(&decode_options))
        .collect()
}

/// Decode an ODB-2 file into a single `DataFrame`, concatenating all frames.
///
/// Returns an empty `DataFrame` for an empty source.
///
/// # Errors
///
/// Fails like [`read_odb`], or if frames have incompatible schemas.
pub fn read_odb_single(path: impl AsRef<Path>, options: &ReadOptions) -> Result<DataFrame> {
    let mut frames = read_odb(path, options)?.into_iter();
    let Some(mut df) = frames.next() else {
        return Ok(DataFrame::default());
    };
    for frame in frames {
        df.vstack_mut(&frame)?;
    }
    Ok(df)
}
