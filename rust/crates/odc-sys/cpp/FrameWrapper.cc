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

rust::Vec<BridgeColumnInfo> FrameWrapper::column_info() const {
    rust::Vec<BridgeColumnInfo> result;
    result.reserve(frame_.columnCount());
    for (const auto& info : frame_.columnInfo()) {
        rust::Vec<BridgeBit> bitfield;
        bitfield.reserve(info.bitfield.size());
        for (const auto& bit : info.bitfield) {
            bitfield.push_back(BridgeBit{rust::String(bit.name), bit.size, bit.offset});
        }
        result.push_back(BridgeColumnInfo{rust::String(info.name), info.type, info.decodedSize, std::move(bitfield)});
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

}  // namespace odc_bridge
