// odc Reader bridge — implementation.

#include "ReaderWrapper.h"
#include "odc-sys/src/lib.rs.h"

#include <string>
#include <utility>

namespace odc_bridge {

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

}  // namespace odc_bridge
