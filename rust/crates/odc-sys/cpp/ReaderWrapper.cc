// odc Reader bridge — implementation.

#include "ReaderWrapper.h"
#include "odc-sys/src/lib.rs.h"

#include <string>
#include <utility>

namespace odc_bridge {

ReaderWrapper::ReaderWrapper(rust::Str path, bool aggregated, int64_t rowlimit) :
    reader_(std::string(path), aggregated, static_cast<long>(rowlimit)) {}

ReaderWrapper::ReaderWrapper(eckit_bridge::DataHandleWrapper& handle, bool aggregated, int64_t rowlimit) :
    reader_(handle.inner(), aggregated, static_cast<long>(rowlimit)) {}

std::unique_ptr<ReaderWrapper> ReaderWrapper::from_path(rust::Str path, bool aggregated, int64_t rowlimit) {
    return std::make_unique<ReaderWrapper>(path, aggregated, rowlimit);
}

std::unique_ptr<ReaderWrapper> ReaderWrapper::from_handle(eckit_bridge::DataHandleWrapper& handle, bool aggregated,
                                                          int64_t rowlimit) {
    return std::make_unique<ReaderWrapper>(handle, aggregated, rowlimit);
}

std::unique_ptr<FrameWrapper> ReaderWrapper::next_frame() {
    odc::api::Frame frame = reader_.next();
    if (!frame) {
        return nullptr;
    }
    return std::make_unique<FrameWrapper>(std::move(frame));
}

}  // namespace odc_bridge
