//! FFI bindings to ECMWF odc (ODB-2 encoder/decoder) library.

use bindman::track_cpp_api;

#[track_cpp_api(
    ("odc/Select.h", class = "Select"),
    ("odc/Writer.h", class = "Writer"),
    ignore = ["end", "dataHandle"]
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

    unsafe extern "C++" {
        include!("odc_bridge.h");

        // Verify ColumnType matches C++ odc::api::ColumnType at compile time
        #[namespace = "odc::api"]
        type ColumnType;

        // Cross-crate ExternType from eckit-sys
        #[namespace = "eckit_bridge"]
        type DataHandleWrapper = eckit_sys::DataHandleWrapper;

        // ==================== SelectIteratorWrapper ====================

        type SelectIteratorWrapper;

        #[must_use]
        fn valid(self: Pin<&mut SelectIteratorWrapper>) -> bool;
        fn advance(self: Pin<&mut SelectIteratorWrapper>) -> Result<()>;
        fn column_count(self: &SelectIteratorWrapper) -> usize;
        fn column_name(self: &SelectIteratorWrapper, idx: usize) -> Result<String>;
        fn column_type(self: &SelectIteratorWrapper, idx: usize) -> Result<ColumnType>;
        fn data(self: &SelectIteratorWrapper, idx: usize) -> Result<f64>;
        fn data_string(self: Pin<&mut SelectIteratorWrapper>, idx: usize) -> Result<String>;
        fn data_integer(self: Pin<&mut SelectIteratorWrapper>, idx: usize) -> Result<i64>;

        // ==================== SelectWrapper ====================

        type SelectWrapper;

        fn begin(self: Pin<&mut SelectWrapper>) -> Result<UniquePtr<SelectIteratorWrapper>>;
        #[cxx_name = "createSelectIterator"]
        fn create_select_iterator(
            self: Pin<&mut SelectWrapper>,
            sql: &str,
        ) -> Result<UniquePtr<SelectIteratorWrapper>>;
        fn database_name(self: Pin<&mut SelectWrapper>) -> Result<String>;

        fn select_create(
            sql: &str,
            handle: Pin<&mut DataHandleWrapper>,
        ) -> Result<UniquePtr<SelectWrapper>>;

        // ==================== WriteIteratorWrapper ====================

        type WriteIteratorWrapper;

        fn set_column(
            self: Pin<&mut WriteIteratorWrapper>,
            index: usize,
            name: &str,
            col_type: ColumnType,
        ) -> Result<()>;
        fn set_number_of_columns(self: Pin<&mut WriteIteratorWrapper>, n: usize) -> Result<()>;
        fn set_data(self: Pin<&mut WriteIteratorWrapper>, index: usize, value: f64) -> Result<()>;
        fn set_data_string(
            self: Pin<&mut WriteIteratorWrapper>,
            index: usize,
            value: &str,
        ) -> Result<()>;
        fn set_data_integer(
            self: Pin<&mut WriteIteratorWrapper>,
            index: usize,
            value: i64,
        ) -> Result<()>;
        fn set_missing_value(
            self: Pin<&mut WriteIteratorWrapper>,
            index: usize,
            value: f64,
        ) -> Result<()>;
        fn write_row(self: Pin<&mut WriteIteratorWrapper>) -> Result<()>;
        fn close(self: Pin<&mut WriteIteratorWrapper>) -> Result<()>;

        // ==================== WriterWrapper ====================

        type WriterWrapper;

        fn pass1(self: Pin<&mut WriterWrapper>, select: Pin<&mut SelectWrapper>) -> Result<()>;
        #[cxx_name = "begin"]
        fn create_write_iterator(
            self: Pin<&mut WriterWrapper>,
        ) -> Result<UniquePtr<WriteIteratorWrapper>>;
        fn rows_buffer_size(self: &WriterWrapper) -> usize;
        fn set_rows_buffer_size(self: Pin<&mut WriterWrapper>, n: usize);
        fn path(self: &WriterWrapper) -> Result<String>;

        fn writer_create(handle: Pin<&mut DataHandleWrapper>) -> Result<UniquePtr<WriterWrapper>>;
    }
}

pub use cxx::{Exception, UniquePtr};
pub use ffi::*;

// SAFETY: All odc wrapper types own their data with no thread-local or global mutable state.
#[allow(clippy::non_send_fields_in_send_ty)]
mod send_impls {
    use super::ffi::{SelectIteratorWrapper, SelectWrapper, WriteIteratorWrapper, WriterWrapper};
    unsafe impl Send for SelectIteratorWrapper {}
    unsafe impl Send for SelectWrapper {}
    unsafe impl Send for WriteIteratorWrapper {}
    unsafe impl Send for WriterWrapper {}
}
