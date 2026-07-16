// odc Frame bridge — wraps `odc::api::Frame`.
#pragma once

#include "odc/api/Odb.h"

#include "rust/cxx.h"

#include <cstddef>

namespace odc_bridge {

// cxx-generated shared structs — defined in "odc-sys/src/lib.rs.h", which
// the .cc includes. Forward-declared here to avoid a circular include (the
// generated header includes this one first).
struct BridgeBit;
struct BridgeColumnInfo;
struct BridgeProperty;

/// Owns an `odc::api::Frame` — a viewport onto a chunk of contiguous,
/// compatible data (possibly a logical frame aggregating several physical
/// frames). The frame reads lazily from the Reader's stream, so it must not
/// outlive its ReaderWrapper — enforced on the Rust side.
class FrameWrapper {
    odc::api::Frame frame_;

    friend class DecoderWrapper;

public:

    explicit FrameWrapper(odc::api::Frame&& frame);

    size_t row_count() const;
    size_t column_count() const;
    bool has_column(rust::Str name) const;
    rust::Vec<BridgeColumnInfo> column_info() const;
    rust::Vec<BridgeProperty> properties() const;
};

}  // namespace odc_bridge
