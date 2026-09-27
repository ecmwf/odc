// odc encode bridge — implementation.

#include "EncoderWrapper.h"
#include "odc-sys/src/lib.rs.h"

#include "eckit/exception/Exceptions.h"

#include <string>

namespace odc_bridge {

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

}  // namespace odc_bridge
