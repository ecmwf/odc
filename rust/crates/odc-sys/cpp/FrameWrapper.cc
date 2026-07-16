// odc Frame bridge — implementation.

#include "FrameWrapper.h"
#include "odc-sys/src/lib.rs.h"

#include <string>
#include <utility>

namespace odc_bridge {

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

rust::Vec<ColumnInfo> FrameWrapper::column_info() const {
    rust::Vec<ColumnInfo> result;
    result.reserve(frame_.columnCount());
    for (const auto& info : frame_.columnInfo()) {
        rust::Vec<Bit> bitfield;
        bitfield.reserve(info.bitfield.size());
        for (const auto& bit : info.bitfield) {
            bitfield.push_back(Bit{rust::String(bit.name), bit.size, bit.offset});
        }
        result.push_back(ColumnInfo{rust::String(info.name), info.type, info.decodedSize, std::move(bitfield)});
    }
    return result;
}

rust::Vec<Property> FrameWrapper::properties() const {
    rust::Vec<Property> result;
    for (const auto& [key, value] : frame_.properties()) {
        result.push_back(Property{rust::String(key), rust::String(value)});
    }
    return result;
}

}  // namespace odc_bridge
