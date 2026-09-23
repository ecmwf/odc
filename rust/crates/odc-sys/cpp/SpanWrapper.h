// odc Span bridge — wraps `odc::api::Span`.
#pragma once

#include "odc/api/Odb.h"

#include "rust/cxx.h"

#include <cstdint>

namespace odc_bridge {

/// Owns an `odc::api::Span` — the sets of values of chosen columns within
/// one frame, and that frame's byte range in the stream. Self-contained:
/// the underlying value sets live in the Span, independent of the frame.
class SpanWrapper {
    odc::api::Span span_;

public:

    explicit SpanWrapper(odc::api::Span&& span);

    uint64_t offset() const;
    uint64_t length() const;
    bool equals(const SpanWrapper& other) const;
    rust::Vec<int64_t> integer_values(rust::Str column) const;
    rust::Vec<double> real_values(rust::Str column) const;
    rust::Vec<rust::String> string_values(rust::Str column) const;
};

}  // namespace odc_bridge
