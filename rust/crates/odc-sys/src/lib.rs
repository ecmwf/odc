//! FFI bindings to ECMWF odc (ODB-2 encoder/decoder) library.
//!
//! Wraps the public C++ API (`odc::api`): `Reader` → `Frame` →
//! `Decoder`/`encode()`, plus the global `Settings`.

use bindman::track_cpp_api;

// Auto-generated odc Error enum + From<cxx::Exception> impl
include!(concat!(env!("OUT_DIR"), "/odc_exceptions.rs"));

#[track_cpp_api(
    ("odc/api/Odb.h", class = "Reader"),
    ("odc/api/Odb.h", class = "Frame"),
    ("odc/api/Odb.h", class = "Decoder"),
    ("odc/api/Odb.h", class = "Settings"),
    ignore = ["offset", "length", "filter", "encodedData", "span", "slice"]
)]
#[cxx::bridge(namespace = "odc_bridge")]
pub mod ffi {
    /// ODB column data types — compile-time verified against C++ `odc::api::ColumnType`.
    #[derive(Debug, Clone, Copy, PartialEq, Eq)]
    #[repr(i32)]
    enum ColumnType {
        #[cxx_name = "IGNORE"]
        Ignore = 0,
        #[cxx_name = "INTEGER"]
        Integer = 1,
        #[cxx_name = "REAL"]
        Real = 2,
        #[cxx_name = "STRING"]
        String = 3,
        #[cxx_name = "BITFIELD"]
        Bitfield = 4,
        #[cxx_name = "DOUBLE"]
        Double = 5,
    }

    /// A bit group within a bitfield column.
    #[derive(Debug, Clone, PartialEq, Eq)]
    struct BridgeBit {
        name: String,
        /// Bit group size in bits.
        size: i32,
        /// Bit group offset in bits.
        offset: i32,
    }

    /// Metadata for one column of a frame.
    #[derive(Debug, Clone)]
    struct BridgeColumnInfo {
        name: String,
        column_type: ColumnType,
        /// Size of a single decoded value in bytes (always a multiple of 8).
        decoded_size: usize,
        /// Bit groups — non-empty only for bitfield columns.
        bitfield: Vec<BridgeBit>,
    }

    /// A key/value property encoded in a frame.
    #[derive(Debug, Clone, PartialEq, Eq)]
    struct BridgeProperty {
        key: String,
        value: String,
    }

    unsafe extern "C++" {
        // odc_exceptions.h first: it defines the `rust::behavior::trycatch`
        // that maps C++ exceptions to typed errors, and must be visible in
        // the cxx-generated translation unit before the wrapper headers.
        include!("odc_exceptions.h");
        include!("OdcBridge.h");

        // Verify ColumnType matches C++ odc::api::ColumnType at compile time
        #[namespace = "odc::api"]
        type ColumnType;

        // Cross-crate ExternType from eckit-sys
        #[namespace = "eckit_bridge"]
        type DataHandleWrapper = eckit_sys::DataHandleWrapper;

        // ==================== ReaderWrapper ====================

        type ReaderWrapper;

        /// Open an ODB-2 file for reading.
        #[Self = "ReaderWrapper"]
        fn from_path(
            path: &str,
            aggregated: bool,
            rowlimit: i64,
        ) -> Result<UniquePtr<ReaderWrapper>>;

        /// Read from an eckit data handle. Does NOT take ownership: the
        /// handle must be unopened and must outlive the reader and every
        /// frame it yields.
        #[Self = "ReaderWrapper"]
        fn from_handle(
            handle: Pin<&mut DataHandleWrapper>,
            aggregated: bool,
            rowlimit: i64,
        ) -> Result<UniquePtr<ReaderWrapper>>;

        /// Next frame in the stream; null when exhausted.
        fn next_frame(self: Pin<&mut ReaderWrapper>) -> Result<UniquePtr<FrameWrapper>>;

        // ==================== FrameWrapper ====================

        type FrameWrapper;

        fn row_count(self: &FrameWrapper) -> usize;
        fn column_count(self: &FrameWrapper) -> usize;
        fn has_column(self: &FrameWrapper, name: &str) -> bool;
        fn column_info(self: &FrameWrapper) -> Result<Vec<BridgeColumnInfo>>;
        fn properties(self: &FrameWrapper) -> Result<Vec<BridgeProperty>>;

        // ==================== DecoderWrapper ====================

        type DecoderWrapper;

        #[Self = "DecoderWrapper"]
        fn create() -> UniquePtr<DecoderWrapper>;

        /// Register a decode target for the named column.
        ///
        /// # Safety
        ///
        /// `data` must be 8-byte-aligned, valid for `nrows * stride` bytes,
        /// and must not be dropped or aliased until `decode` returns.
        unsafe fn add_column(
            self: Pin<&mut DecoderWrapper>,
            name: &str,
            data: *mut u8,
            nrows: usize,
            elem_size: usize,
            stride: usize,
        );

        /// Decode the frame into the registered buffers; returns rows decoded.
        fn decode(
            self: Pin<&mut DecoderWrapper>,
            frame: &FrameWrapper,
            nthreads: usize,
        ) -> Result<usize>;

        // ==================== EncoderWrapper ====================

        type EncoderWrapper;

        #[Self = "EncoderWrapper"]
        fn create() -> UniquePtr<EncoderWrapper>;

        /// Register a source column for encoding.
        ///
        /// # Safety
        ///
        /// `data` must be valid for `nrows * stride` bytes and must not be
        /// dropped until `encode` returns.
        unsafe fn add_column(
            self: Pin<&mut EncoderWrapper>,
            name: &str,
            column_type: ColumnType,
            elem_size: usize,
            data: *const u8,
            nrows: usize,
            stride: usize,
        );

        /// Append a bit group to the most recently added column.
        fn add_bitfield(
            self: Pin<&mut EncoderWrapper>,
            name: &str,
            size: i32,
            offset: i32,
        ) -> Result<()>;

        fn set_property(self: Pin<&mut EncoderWrapper>, key: &str, value: &str);

        /// Encode all registered columns to an (already open) data handle.
        fn encode(
            self: Pin<&mut EncoderWrapper>,
            out: Pin<&mut DataHandleWrapper>,
            max_rows_per_frame: usize,
        ) -> Result<()>;

        // ==================== SettingsWrapper (process-global) ====================

        type SettingsWrapper;

        /// Whether INTEGER/BITFIELD columns decode as doubles (true, odc
        /// default) or as int64 (false).
        #[Self = "SettingsWrapper"]
        fn treat_integers_as_doubles(flag: bool);
        #[Self = "SettingsWrapper"]
        fn integer_missing_value() -> i64;
        #[Self = "SettingsWrapper"]
        fn set_integer_missing_value(value: i64);
        #[Self = "SettingsWrapper"]
        fn double_missing_value() -> f64;
        #[Self = "SettingsWrapper"]
        fn set_double_missing_value(value: f64);
        #[Self = "SettingsWrapper"]
        fn version() -> String;
        #[Self = "SettingsWrapper"]
        fn gitsha1() -> String;
    }
}

pub use cxx::{Exception, UniquePtr};
pub use ffi::*;

// SAFETY: All odc wrapper types have no thread affinity or thread-local
// state. Frames share the reader's underlying stream, but that access is
// serialized C++-side by odc::core::ThreadSharedDataHandle.
#[allow(clippy::non_send_fields_in_send_ty)]
mod send_impls {
    use super::ffi::{DecoderWrapper, EncoderWrapper, FrameWrapper, ReaderWrapper};
    unsafe impl Send for ReaderWrapper {}
    unsafe impl Send for FrameWrapper {}
    unsafe impl Send for DecoderWrapper {}
    unsafe impl Send for EncoderWrapper {}
}
