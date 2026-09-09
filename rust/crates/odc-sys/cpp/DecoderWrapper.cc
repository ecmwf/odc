// odc Decoder bridge — implementation.

#include "DecoderWrapper.h"
#include "odc-sys/src/lib.rs.h"

#include <string>

namespace odc_bridge {

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

}  // namespace odc_bridge
