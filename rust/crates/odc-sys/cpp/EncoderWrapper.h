// odc encode bridge — wraps the `odc::api::encode` free function.
#pragma once

#include "DataHandleWrapper.h"

#include "odc/api/ColumnType.h"
#include "odc/api/Odb.h"

#include "rust/cxx.h"

#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace odc_bridge {

// ColumnType is odc::api::ColumnType — cxx static_asserts values match.
using odc::api::ColumnType;

/// Accumulates column specs + ConstStridedData facades over Rust-owned
/// buffers, then calls the `odc::api::encode` free function.
class EncoderWrapper {
    std::vector<odc::api::ColumnInfo> columns_;
    std::vector<odc::api::ConstStridedData> data_;
    std::map<std::string, std::string> properties_;

public:

    static std::unique_ptr<EncoderWrapper> create();

    /// `data` must point to caller-owned memory of at least nrows * stride
    /// bytes, valid until encode() returns.
    void add_column(rust::Str name, ColumnType column_type, size_t elem_size, const uint8_t* data, size_t nrows,
                    size_t stride);

    /// Append a bit group to the most recently added column.
    void add_bitfield(rust::Str name, int32_t size, int32_t offset);

    void set_property(rust::Str key, rust::Str value);

    /// Encode all columns to an (already open) data handle.
    void encode(eckit_bridge::DataHandleWrapper& out, size_t max_rows_per_frame);
};

}  // namespace odc_bridge
