// odc C++ bridge implementation
#include "odc_bridge.h"

#include "eckit/io/MemoryHandle.h"

#include <string>

namespace odc_bridge {

// ==================== SelectIteratorWrapper ====================

SelectIteratorWrapper::SelectIteratorWrapper(odc::Select& select) : current_(select.begin()), end_(select.end()) {}

SelectIteratorWrapper::SelectIteratorWrapper(odc::Select::iterator current, odc::Select::iterator end) :
    current_(std::move(current)), end_(std::move(end)) {}

bool SelectIteratorWrapper::valid() {
    return current_ != end_;
}

void SelectIteratorWrapper::advance() {
    ++current_;
}

size_t SelectIteratorWrapper::column_count() const {
    return current_->columns().size();
}

rust::String SelectIteratorWrapper::column_name(size_t idx) const {
    return rust::String(current_->columns()[idx]->name());
}

ColumnType SelectIteratorWrapper::column_type(size_t idx) const {
    return current_->columns()[idx]->type();
}

double SelectIteratorWrapper::data(size_t idx) const {
    return current_->data(idx);
}

rust::String SelectIteratorWrapper::data_string(size_t idx) {
    return rust::String(current_->string(idx));
}

int64_t SelectIteratorWrapper::data_integer(size_t idx) {
    return current_->integer(idx);
}

// ==================== SelectWrapper ====================

SelectWrapper::SelectWrapper(rust::Str sql, eckit_bridge::DataHandleWrapper& handle) :
    select_(std::make_unique<odc::Select>(std::string(sql), handle.inner())) {}

std::unique_ptr<SelectIteratorWrapper> SelectWrapper::begin() {
    return std::make_unique<SelectIteratorWrapper>(*select_);
}

std::unique_ptr<SelectIteratorWrapper> SelectWrapper::createSelectIterator(rust::Str sql) {
    auto* it = select_->createSelectIterator(std::string(sql));
    it->next();
    return std::make_unique<SelectIteratorWrapper>(odc::Select::iterator(it), select_->end());
}

rust::String SelectWrapper::database_name() {
    return rust::String(select_->database().name());
}

std::unique_ptr<SelectWrapper> select_create(rust::Str sql, eckit_bridge::DataHandleWrapper& handle) {
    return std::make_unique<SelectWrapper>(sql, handle);
}

// ==================== WriteIteratorWrapper ====================

WriteIteratorWrapper::WriteIteratorWrapper(odc::Writer<>::iterator iter) : iter_(std::move(iter)) {}

void WriteIteratorWrapper::set_column(size_t index, rust::Str name, ColumnType col_type) {
    iter_->setColumn(index, std::string(name), col_type);
}

void WriteIteratorWrapper::set_number_of_columns(size_t n) {
    iter_->setNumberOfColumns(n);
}

void WriteIteratorWrapper::set_data(size_t index, double value) {
    iter_->data(index) = value;
}

void WriteIteratorWrapper::set_data_string(size_t index, rust::Str value) {
    size_t maxlen = sizeof(double) * iter_->columns()[index]->dataSizeDoubles();
    ::strncpy(reinterpret_cast<char*>(&iter_->data(index)), std::string(value).c_str(), maxlen);
}

void WriteIteratorWrapper::set_data_integer(size_t index, int64_t value) {
    iter_->data(index) = static_cast<double>(value);
}

void WriteIteratorWrapper::set_missing_value(size_t index, double value) {
    iter_->missingValue(index, value);
}

void WriteIteratorWrapper::write_row() {
    ++iter_;
}

void WriteIteratorWrapper::close() {
    iter_->close();
}

// ==================== WriterWrapper ====================

WriterWrapper::WriterWrapper(eckit_bridge::DataHandleWrapper& handle) :
    writer_(std::make_unique<odc::Writer<>>(handle.inner())), outit_(writer_->begin()) {}

std::unique_ptr<WriteIteratorWrapper> WriterWrapper::begin() {
    return std::make_unique<WriteIteratorWrapper>(writer_->begin());
}

void WriterWrapper::pass1(SelectWrapper& select) {
    auto it  = select.select_->begin();
    auto end = select.select_->end();
    outit_->pass1(it, end);
}

size_t WriterWrapper::rows_buffer_size() const {
    return writer_->rowsBufferSize();
}

void WriterWrapper::set_rows_buffer_size(size_t n) {
    writer_->rowsBufferSize(n);
}

// Note: data_handle() not exposed — Writer owns the DataHandle internally
// and it's the same one passed to the constructor. Access it via the
// original DataHandleWrapper on the Rust side.

rust::String WriterWrapper::path() const {
    return rust::String(writer_->path());
}

std::unique_ptr<WriterWrapper> writer_create(eckit_bridge::DataHandleWrapper& handle) {
    return std::make_unique<WriterWrapper>(handle);
}

}  // namespace odc_bridge
