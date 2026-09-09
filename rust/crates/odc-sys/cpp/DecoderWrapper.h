// odc Decoder bridge — wraps `odc::api::Decoder`.
#pragma once

#include "FrameWrapper.h"

#include "odc/api/Odb.h"

#include "rust/cxx.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace odc_bridge {

/// Accumulates per-column decode targets (StridedData facades over
/// Rust-owned buffers), then runs `odc::api::Decoder::decode`.
class DecoderWrapper {
    std::vector<std::string> names_;
    std::vector<odc::api::StridedData> facades_;

public:

    static std::unique_ptr<DecoderWrapper> create();

    /// `data` must point to caller-owned, 8-byte-aligned memory of at least
    /// nrows * stride bytes, valid until decode() returns.
    void add_column(rust::Str name, uint8_t* data, size_t nrows, size_t elem_size, size_t stride);

    /// Decode the frame into the registered buffers; returns rows decoded.
    size_t decode(const FrameWrapper& frame, size_t nthreads);
};

}  // namespace odc_bridge
