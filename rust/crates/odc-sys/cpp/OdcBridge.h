// odc C++ bridge for Rust FFI
#pragma once

#include "EckitBridge.h"
#include "odc_exceptions.h"

#include "odc/api/ColumnType.h"
#include "odc/api/Odb.h"

#include "rust/cxx.h"

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace odc_bridge {

// ColumnType is odc::api::ColumnType — cxx static_asserts values match.
using odc::api::ColumnType;

// cxx-generated shared structs — defined in "odc-sys/src/lib.rs.h", which
// OdcBridge.cc includes. Forward-declared here to avoid a circular include
// (the generated header includes OdcBridge.h first).
struct BridgeBit;
struct BridgeColumnInfo;
struct BridgeProperty;

// ==================== FrameWrapper ====================

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

// ==================== ReaderWrapper ====================

/// Wraps `odc::api::Reader` — owns the ODB-2 stream, yields frames.
class ReaderWrapper {
    std::unique_ptr<odc::api::Reader> reader_;

public:

    explicit ReaderWrapper(std::unique_ptr<odc::api::Reader> reader);

    static std::unique_ptr<ReaderWrapper> from_path(rust::Str path, bool aggregated, int64_t rowlimit);

    /// Does not take ownership: the DataHandleWrapper must outlive this
    /// reader (and every frame it yields) — enforced on the Rust side.
    /// The handle must not be open; the reader opens it for reading.
    static std::unique_ptr<ReaderWrapper> from_handle(eckit_bridge::DataHandleWrapper& handle, bool aggregated,
                                                      int64_t rowlimit);

    /// Returns nullptr when the stream is exhausted.
    std::unique_ptr<FrameWrapper> next_frame();
};

// ==================== DecoderWrapper ====================

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

// ==================== EncoderWrapper ====================

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

// ==================== SettingsWrapper ====================

/// Wraps `odc::api::Settings` — process-global settings and version info.
/// Static-only; never instantiated.
class SettingsWrapper {
public:

    SettingsWrapper() = delete;

    static void treat_integers_as_doubles(bool flag);
    static int64_t integer_missing_value();
    static void set_integer_missing_value(int64_t value);
    static double double_missing_value();
    static void set_double_missing_value(double value);
    static rust::String version();
    static rust::String gitsha1();
};

}  // namespace odc_bridge
