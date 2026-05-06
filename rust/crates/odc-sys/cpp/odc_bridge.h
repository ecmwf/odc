// odc C++ bridge for Rust FFI
#pragma once

#include "eckit_bridge.h"
#include "eckit_exceptions.h"

#include "odc/Select.h"
#include "odc/Writer.h"
#include "odc/api/ColumnType.h"

#include "rust/cxx.h"

#include <cstdint>
#include <memory>
#include <string>

namespace odc_bridge {

// ColumnType is odc::api::ColumnType — cxx static_asserts values match.
using odc::api::ColumnType;

// ==================== SelectIteratorWrapper ====================

/// Wraps `odc::Select::iterator` pair for row-by-row iteration.
class SelectIteratorWrapper {
    odc::Select::iterator current_;
    odc::Select::iterator end_;

public:

    SelectIteratorWrapper(odc::Select& select);
    SelectIteratorWrapper(odc::Select::iterator current, odc::Select::iterator end);

    bool valid();
    void advance();

    size_t column_count() const;
    rust::String column_name(size_t idx) const;
    ColumnType column_type(size_t idx) const;
    double data(size_t idx) const;
    rust::String data_string(size_t idx);
    int64_t data_integer(size_t idx);
};

// ==================== SelectWrapper ====================

/// Wraps `odc::Select` — owns the query, produces iterators.
class SelectWrapper {
    std::unique_ptr<odc::Select> select_;

    friend class WriterWrapper;

public:

    SelectWrapper(rust::Str sql, eckit_bridge::DataHandleWrapper& handle);

    std::unique_ptr<SelectIteratorWrapper> begin();
    std::unique_ptr<SelectIteratorWrapper> createSelectIterator(rust::Str sql);
    rust::String database_name();
};

std::unique_ptr<SelectWrapper> select_create(rust::Str sql, eckit_bridge::DataHandleWrapper& handle);

// ==================== WriteIteratorWrapper ====================

/// Wraps `odc::Writer<>::iterator` for row-by-row writing.
class WriteIteratorWrapper {
    odc::Writer<>::iterator iter_;

public:

    explicit WriteIteratorWrapper(odc::Writer<>::iterator iter);

    /// Define a column. Must be called before writing any rows.
    void set_column(size_t index, rust::Str name, ColumnType col_type);

    /// Set number of columns.
    void set_number_of_columns(size_t n);

    /// Set a double value at column index for the current row.
    void set_data(size_t index, double value);

    /// Set a string value at column index for the current row.
    void set_data_string(size_t index, rust::Str value);

    /// Set an integer value at column index for the current row.
    void set_data_integer(size_t index, int64_t value);

    /// Set the missing value for a column.
    void set_missing_value(size_t index, double value);

    /// Write the current row (advances the iterator).
    void write_row();

    /// Close the writer.
    void close();
};

// ==================== WriterWrapper ====================

/// Wraps `odc::Writer<>` — writes filtered ODB data via pass1.
class WriterWrapper {
    std::unique_ptr<odc::Writer<>> writer_;
    odc::Writer<>::iterator outit_;

public:

    explicit WriterWrapper(eckit_bridge::DataHandleWrapper& handle);

    void pass1(SelectWrapper& select);

    /// Get a write iterator for row-by-row writing.
    std::unique_ptr<WriteIteratorWrapper> begin();

    size_t rows_buffer_size() const;
    void set_rows_buffer_size(size_t n);
    rust::String path() const;
};

std::unique_ptr<WriterWrapper> writer_create(eckit_bridge::DataHandleWrapper& handle);

}  // namespace odc_bridge
