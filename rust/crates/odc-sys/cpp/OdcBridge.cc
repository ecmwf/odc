// odc C++ bridge implementation
#include "OdcBridge.h"

#include "odc-sys/src/lib.rs.h"

#include "eckit/exception/Exceptions.h"

#include <utility>

namespace odc_bridge {

// ==================== FrameWrapper ====================

FrameWrapper::FrameWrapper(odc::api::Frame&& frame) : frame_(std::move(frame)) {}

size_t FrameWrapper::row_count() const {
    return frame_.rowCount();
}

size_t FrameWrapper::column_count() const {
    return frame_.columnCount();
}

bool FrameWrapper::has_column(rust::Str name) const {
    return frame_.hasColumn(std::string(name));
}

rust::Vec<BridgeColumnInfo> FrameWrapper::column_info() const {
    rust::Vec<BridgeColumnInfo> result;
    result.reserve(frame_.columnCount());
    for (const auto& info : frame_.columnInfo()) {
        rust::Vec<BridgeBit> bitfield;
        bitfield.reserve(info.bitfield.size());
        for (const auto& bit : info.bitfield) {
            bitfield.push_back(BridgeBit{rust::String(bit.name), bit.size, bit.offset});
        }
        result.push_back(
            BridgeColumnInfo{rust::String(info.name), info.type, info.decodedSize, std::move(bitfield)});
    }
    return result;
}

rust::Vec<BridgeProperty> FrameWrapper::properties() const {
    rust::Vec<BridgeProperty> result;
    for (const auto& [key, value] : frame_.properties()) {
        result.push_back(BridgeProperty{rust::String(key), rust::String(value)});
    }
    return result;
}

// ==================== ReaderWrapper ====================

ReaderWrapper::ReaderWrapper(std::unique_ptr<odc::api::Reader> reader) : reader_(std::move(reader)) {}

std::unique_ptr<ReaderWrapper> ReaderWrapper::from_path(rust::Str path, bool aggregated, int64_t rowlimit) {
    return std::make_unique<ReaderWrapper>(
        std::make_unique<odc::api::Reader>(std::string(path), aggregated, static_cast<long>(rowlimit)));
}

std::unique_ptr<ReaderWrapper> ReaderWrapper::from_handle(eckit_bridge::DataHandleWrapper& handle, bool aggregated,
                                                          int64_t rowlimit) {
    return std::make_unique<ReaderWrapper>(
        std::make_unique<odc::api::Reader>(handle.inner(), aggregated, static_cast<long>(rowlimit)));
}

std::unique_ptr<FrameWrapper> ReaderWrapper::next_frame() {
    odc::api::Frame frame = reader_->next();
    if (!frame) {
        return nullptr;
    }
    return std::make_unique<FrameWrapper>(std::move(frame));
}

// ==================== DecoderWrapper ====================

std::unique_ptr<DecoderWrapper> DecoderWrapper::create() {
    return std::make_unique<DecoderWrapper>();
}

void DecoderWrapper::add_column(rust::Str name, uint8_t* data, size_t nrows, size_t elem_size, size_t stride) {
    names_.emplace_back(std::string(name));
    facades_.emplace_back(reinterpret_cast<char*>(data), nrows, elem_size, stride);
}

size_t DecoderWrapper::decode(const FrameWrapper& frame, size_t nthreads) {
    odc::api::Decoder decoder(names_, facades_);
    decoder.decode(frame.frame_, nthreads == 0 ? 1 : nthreads);
    return frame.frame_.rowCount();
}

// ==================== EncoderWrapper ====================

std::unique_ptr<EncoderWrapper> EncoderWrapper::create() {
    return std::make_unique<EncoderWrapper>();
}

void EncoderWrapper::add_column(rust::Str name, ColumnType column_type, size_t elem_size, const uint8_t* data,
                                size_t nrows, size_t stride) {
    columns_.push_back(odc::api::ColumnInfo{std::string(name), column_type, elem_size, {}});
    data_.emplace_back(reinterpret_cast<const char*>(data), nrows, elem_size, stride);
}

void EncoderWrapper::add_bitfield(rust::Str name, int32_t size, int32_t offset) {
    if (columns_.empty()) {
        throw eckit::UserError("add_bitfield called before add_column");
    }
    columns_.back().bitfield.push_back(odc::api::ColumnInfo::Bit{std::string(name), size, offset});
}

void EncoderWrapper::set_property(rust::Str key, rust::Str value) {
    properties_[std::string(key)] = std::string(value);
}

void EncoderWrapper::encode(eckit_bridge::DataHandleWrapper& out, size_t max_rows_per_frame) {
    odc::api::encode(out.inner(), columns_, data_, properties_, max_rows_per_frame);
}

// ==================== SettingsWrapper ====================

void SettingsWrapper::treat_integers_as_doubles(bool flag) {
    odc::api::Settings::treatIntegersAsDoubles(flag);
}

int64_t SettingsWrapper::integer_missing_value() {
    return static_cast<int64_t>(odc::api::Settings::integerMissingValue());
}

void SettingsWrapper::set_integer_missing_value(int64_t value) {
    odc::api::Settings::setIntegerMissingValue(static_cast<long>(value));
}

double SettingsWrapper::double_missing_value() {
    return odc::api::Settings::doubleMissingValue();
}

void SettingsWrapper::set_double_missing_value(double value) {
    odc::api::Settings::setDoubleMissingValue(value);
}

rust::String SettingsWrapper::version() {
    return rust::String(odc::api::Settings::version());
}

rust::String SettingsWrapper::gitsha1() {
    return rust::String(odc::api::Settings::gitsha1());
}

}  // namespace odc_bridge
