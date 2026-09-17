// odc Reader bridge — wraps `odc::api::Reader`.
#pragma once

#include "DataHandleWrapper.h"
#include "FrameWrapper.h"

#include "odc/api/Odb.h"

#include "rust/cxx.h"

#include <cstdint>
#include <memory>

namespace odc_bridge {

/// Wraps `odc::api::Reader` — owns the ODB-2 stream, yields frames.
class ReaderWrapper {
    odc::api::Reader reader_;

public:

    ReaderWrapper(rust::Str path, bool aggregated, int64_t rowlimit);

    /// Does not take ownership: the DataHandleWrapper must outlive this
    /// reader (and every frame it yields) — enforced on the Rust side.
    /// The handle must already be open for reading; the reader does not
    /// open or close it.
    ReaderWrapper(eckit_bridge::DataHandleWrapper& handle, bool aggregated, int64_t rowlimit);

    static std::unique_ptr<ReaderWrapper> from_path(rust::Str path, bool aggregated, int64_t rowlimit);

    static std::unique_ptr<ReaderWrapper> from_handle(eckit_bridge::DataHandleWrapper& handle, bool aggregated,
                                                      int64_t rowlimit);

    /// Returns nullptr when the stream is exhausted.
    std::unique_ptr<FrameWrapper> next_frame();
};

}  // namespace odc_bridge
