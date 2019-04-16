/*
 * (C) Copyright 2019- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cstdint>
#include <functional>
#include <numeric>
#include <unistd.h>

#include "eckit/exception/Exceptions.h"
#include "eckit/maths/Functions.h"
#include "eckit/runtime/Main.h"
#include "eckit/io/MemoryHandle.h"
#include "eckit/io/FileDescHandle.h"

#include "odc/api/odc.h"
#include "odc/api/Odb.h"

using namespace odc::api;
using namespace eckit;

extern "C" {

//----------------------------------------------------------------------------------------------------------------------

// DOCUMENTATION comments

// i) By default this API throws exceptions. This will result in hard termination
//    of applications on failure. The routine:
//
//      void odc_error_handling(handling_type);
//
//    changes the behaviour with three options:
//
//      ODC_THROW          - Throws exceptions
//      ODC_ERRORS_CHECKED - Enforces that error values are checked. If an API call
//                           is made and the previous error has not been reset, then
//                           std::terminate will be called.
//      ODC_ERRORS_REPORT  - All exceptions will be caught and odc_success set
//                           accordingly.

// ii) odc_encode accepts a description of the data structured in memory.

//----------------------------------------------------------------------------------------------------------------------

struct odb_t {
    odb_t(const char* f) : internal(f) {}
    odb_t(eckit::DataHandle* dh) : internal(dh) {} // takes ownership
    Odb internal;
};

struct odb_table_t {
    odb_table_t(const Table& t) : internal(t) {}
    const Table internal;
};

struct odb_decode_target_t {

    struct DecodeColumn {
        void* data;
        size_t elemSize;
        size_t stride;
    };

    odb_decode_target_t() : nrows(0), ownedData() {}

    size_t nrows;
    std::vector<std::string> columnNames;
    std::vector<DecodeColumn> columnData;
    std::unique_ptr<char> ownedData;
};

struct odb_encoder_t {

    odb_encoder_t() : arrayData(0), columnMajor(false), maxRowsPerFrame(10000) {}

    struct EncodeColumn {
        const void* data;
        size_t stride;
    };

    void* arrayData;
    bool columnMajor;

    size_t nrows;
    size_t maxRowsPerFrame;
    std::vector<ColumnInfo> columnInfo;
    std::vector<EncodeColumn> columnData;
};


//----------------------------------------------------------------------------------------------------------------------

/* Error handling */

static std::string g_current_error_str;
static int g_odc_error_behaviour = ODC_THROW;
bool g_last_success = true;

void odc_reset_error() {
    g_current_error_str.clear();
    g_last_success = true;
}

void odc_error_handling(int handling_type) {
    ASSERT(handling_type >= ODC_THROW && handling_type <= ODC_ERRORS_REPORT);
    g_odc_error_behaviour = handling_type;
    odc_reset_error();
}

bool odc_success() {
    return g_last_success;
}

const char* odc_error_string() {
    if (g_current_error_str.empty()) {
        return 0;
    } else {
        return g_current_error_str.c_str();
    }
}

static void set_error(const std::string& s) {
    g_current_error_str = s;
    g_last_success = false;
}

} // extern "C"

// Template can't have C linkage

namespace {

template <typename T>
T DefaultConstructed() { return T(); }

template <>
void DefaultConstructed<void>() {}

template <typename FN>
auto wrapApiFunction(FN f) -> decltype(f()) {

    if (g_odc_error_behaviour == ODC_ERRORS_REPORT) {
        odc_reset_error();
    }
    if (g_odc_error_behaviour == ODC_ERRORS_CHECKED && !odc_success()) {
        std::stringstream ss;
        ss << "APi call being made after unchecked error: "
           << odc_error_string()
           << ". SeriousBug in calling code";
        throw SeriousBug(ss.str(), Here());
    }

    try {
        return f();
    } catch (Exception& e) {
        Log::error() << "Caught exception on C-C++ API boundary: " << e.what() << std::endl;
        if (g_odc_error_behaviour == ODC_THROW) throw;
        set_error(e.what());
    } catch (std::exception& e) {
        Log::error() << "Caught exception on C-C++ API boundary: " << e.what() << std::endl;
        if (g_odc_error_behaviour == ODC_THROW) throw;
        set_error(e.what());
    } catch (...) {
        Log::error() << "Caught unknown exception on C-C++ API boundary: " << std::endl;
        if (g_odc_error_behaviour == ODC_THROW) throw;
        set_error("Unexpected exception caught");
    }

    // Return default constructed value.
    return DefaultConstructed<decltype(f())>();
}

}

extern "C" {

//----------------------------------------------------------------------------------------------------------------------

// Types for lookup

int odc_type_count() {
    return NUM_TYPES;
}

const char* odc_type_name(int type) {

    return wrapApiFunction([type] {

        static const char* names[] = {
            OdbTypes<ColumnType(0)>::name,
            OdbTypes<ColumnType(1)>::name,
            OdbTypes<ColumnType(2)>::name,
            OdbTypes<ColumnType(3)>::name,
            OdbTypes<ColumnType(4)>::name,
            OdbTypes<ColumnType(5)>::name
        };

        if (type < 0 || type >= NUM_TYPES) {
            std::stringstream ss;
            ss << "Unrecognised type: " << type;
            throw UserError(ss.str(), Here());
        }

        return names[type];
    });
}

//----------------------------------------------------------------------------------------------------------------------

// TODO: In a sensible, templated, way catch all exceptions.
//       --> We ought to have a standardised error return process.


/*
 * Initialise API
 * @note This is only required if being used from a context where Main()
 *       is not otherwise initialised
*/

void odc_initialise_api() {
    return wrapApiFunction([] {
        static bool initialised = false;

        if (initialised) {
            Log::warning() << "Initialising ODC library twice" << std::endl;
        }

        if (!initialised) {
            const char* argv[2] = {"odc-api", 0};
            Main::initialise(1, const_cast<char**>(argv));
            initialised = true;
        }

        ASSERT(ODC_IGNORE   == static_cast<int>(IGNORE));
        ASSERT(ODC_INTEGER  == static_cast<int>(INTEGER));
        ASSERT(ODC_REAL     == static_cast<int>(REAL));
        ASSERT(ODC_STRING   == static_cast<int>(STRING));
        ASSERT(ODC_BITFIELD == static_cast<int>(BITFIELD));
        ASSERT(ODC_DOUBLE   == static_cast<int>(DOUBLE));
    });
}

void odc_integer_behaviour(int integerBehaviour) {
    return wrapApiFunction([integerBehaviour] {
        if (integerBehaviour != ODC_INTEGERS_AS_DOUBLES && integerBehaviour != ODC_INTEGERS_AS_LONGS) {
            throw SeriousBug("ODC integer behaviour must be either ODC_INTEGERS_AS_DOUBLES or ODC_INTEGERS_AS_LONGS", Here());
        }
        Settings::treatIntegersAsDoubles(integerBehaviour == ODC_INTEGERS_AS_DOUBLES);
    });
}

long odc_missing_integer() {
    return Settings::integerMissingValue();
}

const char* odc_version() {
    return wrapApiFunction([]{
        return Settings::version().c_str();
    });
}

const char* odc_git_sha1() {
    return wrapApiFunction([]{
        return Settings::gitsha1().c_str();
    });
}

//----------------------------------------------------------------------------------------------------------------------

/* Basic READ objects */

odb_t* odc_open_path(const char* filename) {
    return wrapApiFunction([filename] {
        return new odb_t {filename};
    });
}

odb_t* odc_open_file_descriptor(int fd) {
    return wrapApiFunction([fd] {
        // Take a copy of the file descriptor. This allows us to decouple the life of this
        // from the life of the caller
        int fd2 = dup(fd);
        if (fd == -1) throw CantOpenFile("dup() failed on supplied file descriptor", Here());
        return new odb_t {new FileDescHandle(fd2, true)};
    });
}

odb_t* odc_open_buffer(const void* data, long length) {
    return wrapApiFunction([data, length] {
        return new odb_t {new MemoryHandle(data, length)};
    });

}

typedef long (*stream_read_t)(void* handle, void* buffer, long length);

odb_t* odb_open_stream(void* handle, stream_read_t stream_proc) {

    // Wrap the stream in a DataHandle
    struct ReadStreamDataHandle : public eckit::DataHandle {
        ReadStreamDataHandle(void* handle, stream_read_t fn) : handle_(handle), fn_(fn) {}
        virtual ~ReadStreamDataHandle() {}
        void print(std::ostream& s) const override { s << "StreamReadHandle(" << fn_ << "(" << handle_ << "))"; }
        Length openForRead() override { return 0; }
        void openForWrite(const Length&) override { NOTIMP; }
        void openForAppend(const Length&) override { NOTIMP; }
        long read(void* buffer, long length) override { return fn_(handle_, buffer, length); }
        long write(const void*, long) override { NOTIMP; }
        void close() {}

        void* handle_;
        stream_read_t fn_;
    };

    return wrapApiFunction([handle, stream_proc] {
        return new odb_t(new ReadStreamDataHandle(handle, stream_proc));
    });
}

void odc_close(odb_t* o) {
    return wrapApiFunction([o]{
        ASSERT(o);
        delete o;
    });
}

//----------------------------------------------------------------------------------------------------------------------

/*
 * Table handling
 */

odb_table_t* odc_alloc_next_table(odb_t* o, bool aggregated) {
    return wrapApiFunction([o, aggregated] {
        if (Optional<Table> t = o->internal.next(aggregated)) {
            return new odb_table_t(t.get());
        }
        return static_cast<odb_table_t*>(nullptr);
    });
}

void odc_free_table(odb_table_t* t) {
    return wrapApiFunction([t] {
        ASSERT(t);
        delete(t);
    });
}

long odc_table_row_count(const odb_table_t* t) {
    return wrapApiFunction([t] {
        ASSERT(t);
        return t->internal.rowCount();
    });
}

int odc_table_column_count(const odb_table_t* t) {
    return wrapApiFunction([t] {
        ASSERT(t);
        return t->internal.columnCount();
    });
}

int odc_table_column_type(const odb_table_t* t, int col) {
    return wrapApiFunction([t, col] {
        ASSERT(t);
        return t->internal.columnInfo()[col].type;
    });
}

int odc_table_column_data_size(const odb_table_t* t, int col) {
    return wrapApiFunction([t, col] {
        ASSERT(t);
        return t->internal.columnInfo()[col].decodedSize;
    });
}

const char* odc_table_column_name(const odb_table_t* t, int col) {
    return wrapApiFunction([t, col] {
        ASSERT(t);
        return t->internal.columnInfo()[col].name.c_str();
    });
}

int odc_table_column_bitfield_count(const struct odb_table_t* t, int col) {
    return wrapApiFunction([t, col] {
        ASSERT(t);
        return t->internal.columnInfo()[col].bitfield.size();
    });
}

const char* odc_table_column_bitfield_field_name(const struct odb_table_t* t, int col, int n) {
    return wrapApiFunction([t, col, n] {
        ASSERT(t);
        return t->internal.columnInfo()[col].bitfield[n].name.c_str();
    });
}

int odc_table_column_bitfield_field_size(const struct odb_table_t* t, int col, int n) {
    return wrapApiFunction([t, col, n] {
        ASSERT(t);
        return t->internal.columnInfo()[col].bitfield[n].size;
    });
}

int odc_table_column_bitfield_field_offset(const struct odb_table_t* t, int col, int n) {
    return wrapApiFunction([t, col, n] {
        ASSERT(t);
        return t->internal.columnInfo()[col].bitfield[n].offset;
    });
}

//----------------------------------------------------------------------------------------------------------------------

/* Decode functionality */

odb_decode_target_t* odc_alloc_decode_target() {
    return wrapApiFunction([] {
        return new odb_decode_target_t;
    });
}

void odc_free_decode_target(odb_decode_target_t* dc) {
    return wrapApiFunction([dc] {
        delete dc;
    });
}

void odc_decode_target_set_row_count(odb_decode_target_t* dc, long nrows) {
    return wrapApiFunction([dc, nrows] {
        ASSERT(dc);
        dc->nrows = nrows;
    });
}

/* Add a column to decode (self-allocating by default) */
int odc_decode_target_add_column(odb_decode_target_t* dt, const char* name) {
    return wrapApiFunction([dt, name] {
        ASSERT(dt);
        dt->columnNames.emplace_back(name);
        dt->columnData.emplace_back(odb_decode_target_t::DecodeColumn {0, 0, 0});
        return dt->columnNames.size()-1;
    });
}

void odc_decode_target_column_set_size(odb_decode_target_t* dt, int col, int elemSize) {
    return wrapApiFunction([dt, col, elemSize] {
        ASSERT(dt);
        ASSERT(col >= 0 && col < dt->columnData.size());
        ASSERT(elemSize > 0 && elemSize % sizeof(double) == 0);
        dt->columnData[col].elemSize = elemSize;
    });
}

void odc_decode_target_column_set_stride(odb_decode_target_t* dt, int col, int stride) {
    return wrapApiFunction([dt, col, stride] {
        ASSERT(dt);
        ASSERT(col >= 0 && col < dt->columnData.size());
        ASSERT(stride > 0 && stride % sizeof(double) == 0);
        dt->columnData[col].stride = stride;
    });
}

void odc_decode_target_column_set_data(odb_decode_target_t* dt, int col, void* data) {
    return wrapApiFunction([dt, col, data] {
        ASSERT(dt);
        ASSERT(col >= 0 && col < dt->columnData.size());
        ASSERT(data);
        dt->columnData[col].data = data;
    });
}

const void* odc_decode_target_array_data(struct odb_decode_target_t* dt) {
    return wrapApiFunction([dt] {
        ASSERT(dt);
        ASSERT(dt->ownedData);
        return dt->ownedData.get();
    });
}

int odc_decode_target_column_stride(struct odb_decode_target_t* dt, int col) {
    return wrapApiFunction([dt, col] {
        ASSERT(dt);
        ASSERT(col >= 0 && col < dt->columnData.size());
        ASSERT(dt->ownedData);
        return dt->columnData[col].stride;
    });
}

int odc_decode_target_column_size(struct odb_decode_target_t* dt, int col) {
    return wrapApiFunction([dt, col] {
        ASSERT(dt);
        ASSERT(col >= 0 && col < dt->columnData.size());
        ASSERT(dt->ownedData);
        return dt->columnData[col].elemSize;
    });
}

const void* odc_decode_target_column_data(struct odb_decode_target_t* dt, int col) {
    return wrapApiFunction([dt, col] {
        ASSERT(dt);
        ASSERT(col >= 0 && col < dt->columnData.size());
        ASSERT(dt->ownedData);
        return dt->columnData[col].data;
    });
}

void odc_table_build_all_decode_target(const odb_table_t* t, odb_decode_target_t* dt) {
    return wrapApiFunction([t, dt] {

        const Table& tbl(t->internal);

        size_t nrows = tbl.rowCount();
        size_t ncols = tbl.columnCount();

        dt->nrows = nrows;

        // Fill in column details

        for (size_t col = 0; col < ncols; ++col) {
            odc_decode_target_add_column(dt, tbl.columnInfo()[col].name.c_str());
        }
    });
}

static void fill_in_decode_target(const odb_table_t* t, odb_decode_target_t* dt) {

    if (dt->nrows == 0) {
        dt->nrows = t->internal.rowCount();
    }

    // If we are allocating, we assume that we are filling in dense columns (for now)

    size_t allocateSize = 0;
    std::vector<std::pair<size_t, long>> offsets;

    for (size_t i = 0; i < dt->columnData.size(); ++i) {
        odb_decode_target_t::DecodeColumn& col(dt->columnData[i]);

        if (col.elemSize == 0) {
            if (col.data) {
                col.elemSize = sizeof(double); // backwards compatible default
            } else {
                const std::string& colName(dt->columnNames[i]);
                auto it = std::find_if(t->internal.columnInfo().begin(), t->internal.columnInfo().end(),
                                       [&colName](const ColumnInfo& ci) { return ci.name == colName; });
                ASSERT(it != t->internal.columnInfo().end());
                col.elemSize = it->decodedSize;
            }
        }

        if (col.stride == 0) {
            col.stride = col.elemSize;
        } else if (col.stride != col.elemSize) {
            ASSERT(col.data);
        }

        if (!col.data) {
            offsets.push_back(std::make_pair(i, allocateSize));
            allocateSize += (col.elemSize * dt->nrows);
        }
    }
    if (allocateSize != 0) {

        dt->ownedData.reset(new char[allocateSize]);

        // Update the allocated columns data pointer information
        for (const auto& off : offsets) {
            dt->columnData[off.first].data = dt->ownedData.get() + off.second;
        }
    }
}

long odc_table_decode(const odb_table_t* t, odb_decode_target_t* dt, int nthreads) {
    return wrapApiFunction([t, dt, nthreads] {

        // Sanity checking

        size_t frame_rows = t->internal.rowCount();
        size_t frame_cols = t->internal.columnCount();

        ASSERT(dt);
        ASSERT(dt->columnData.size() == dt->columnNames.size());
        ASSERT(dt->columnNames.size() <= frame_cols);
        ASSERT(dt->nrows >= frame_rows);

        // Fill in and allocate decode target as required

        fill_in_decode_target(t, dt);

        // Construct C++ API adapter

        std::vector<StridedData> dataFacade;
        dataFacade.reserve(dt->columnNames.size());

        for (auto& col : dt->columnData) {
            dataFacade.emplace_back(StridedData{col.data, size_t(dt->nrows), size_t(col.elemSize), size_t(col.stride)});
        }

        DecodeTarget target(dt->columnNames, dataFacade);

        // Do the decoder

        ASSERT(nthreads >= 1);
        t->internal.decode(target, static_cast<size_t>(nthreads));

        // And return the values

        dt->nrows = frame_rows;
        return frame_rows;
    });
}

//----------------------------------------------------------------------------------------------------------------------

/* Encode functionality */

odb_encoder_t* odc_alloc_encoder() {
    return wrapApiFunction([] {
        return new odb_encoder_t;
    });
}

void odc_free_encoder(odb_encoder_t* en) {
    return wrapApiFunction([en] {
        delete en;
    });
}

void odc_encoder_set_row_count(odb_encoder_t* en, long nrows) {
    return wrapApiFunction([en, nrows] {
        ASSERT(en);
        en->nrows = nrows;
    });
}

void odc_encoder_set_rows_per_frame(odb_encoder_t* en, long rows_per_frame) {
    return wrapApiFunction([en, rows_per_frame] {
        ASSERT(en);
        en->maxRowsPerFrame = rows_per_frame;
    });
}

void odc_encoder_set_data_array(odb_encoder_t* en, void* data, bool columnMajor) {
    return wrapApiFunction([en, data, columnMajor] {
        ASSERT(en);
        en->arrayData = data;
        en->columnMajor = columnMajor;
    });
}

int odc_encoder_add_column(odb_encoder_t* en, const char* name, int type) {
    return wrapApiFunction([en, name, type] {
        ASSERT(en);
        en->columnInfo.emplace_back(ColumnInfo{std::string(name), ColumnType(type)});
        en->columnData.emplace_back(odb_encoder_t::EncodeColumn {0, 0});
        return en->columnInfo.size()-1;
    });
}

void odc_encoder_column_set_size(odb_encoder_t* en, int col, int elemSize) {
    return wrapApiFunction([en, col, elemSize] {
        ASSERT(en);
        ASSERT(col >= 0 && col < en->columnInfo.size());
        ASSERT(elemSize > 0 && elemSize % sizeof(double) == 0);
        en->columnInfo[col].decodedSize = elemSize;
    });
}

void odc_encoder_column_set_stride(odb_encoder_t* en, int col, int stride) {
    return wrapApiFunction([en, col, stride] {
        ASSERT(en);
        ASSERT(col >= 0 && col < en->columnInfo.size());
        ASSERT(stride > 0 && stride % sizeof(double) == 0);
        en->columnData[col].stride = stride;
    });
}

void odc_encoder_column_set_data(odb_encoder_t* en, int col, const void* data) {
    return wrapApiFunction([en, col, data] {
        ASSERT(en);
        ASSERT(col >= 0 && col < en->columnInfo.size());
        ASSERT(data);
        en->columnData[col].data = data;
    });
}

void odc_encoder_column_add_bitfield_field(struct odb_encoder_t* en, int col, const char* name, int nbits) {
    return wrapApiFunction([en, col, name, nbits] {
        ASSERT(en);
        ASSERT(col >= 0 && col < en->columnInfo.size());

        size_t offset;
        ColumnInfo& ci(en->columnInfo[col]);
        if (ci.bitfield.size() == 0) {
            offset = 0;
        } else {
            offset = ci.bitfield.back().offset + ci.bitfield.back().size;
        }

        ASSERT(offset + nbits <= 32);
        ci.bitfield.emplace_back(ColumnInfo::Bit {name, nbits, offset});
    });
}

void fill_in_encoder(odb_encoder_t* en) {

    ASSERT(en->nrows > 0);
    ASSERT(en->columnData.size() == en->columnInfo.size());

    for (auto& info : en->columnInfo) {
        if (info.decodedSize == 0) info.decodedSize = 8;
    }

    if (en->arrayData) {

        // We are constructing the encoder from a 2D array of data.

        if (en->columnMajor) {
            throw NotImplemented("Column major auto-sizing not yet implemented", Here());
        } else {
            size_t rowSize = std::accumulate(en->columnInfo.begin(), en->columnInfo.end(), size_t{0},
                                             [](size_t l, const ColumnInfo& r) {return l + r.decodedSize;});

            uint8_t* p = reinterpret_cast<uint8_t*>(en->arrayData); // uint8_t == 1 byte, for pointer arithmetic
            for (size_t i = 0; i < en->columnData.size(); ++i) {
                odb_encoder_t::EncodeColumn& c(en->columnData[i]);
                const ColumnInfo& info(en->columnInfo[i]);
                ASSERT(info.decodedSize != 0);
                ASSERT(c.data == 0);
                ASSERT(c.stride == 0);

                c.stride = rowSize;
                c.data = p;
                p += info.decodedSize;

                // sanity checks
                ASSERT(c.stride % sizeof(double) == 0);
                ASSERT(info.decodedSize % sizeof(double) == 0);
            }
        }
    } else {

        // We are constructing the data with a supplied structure per column

        for (size_t i = 0; i < en->columnData.size(); ++i) {
            odb_encoder_t::EncodeColumn& c(en->columnData[i]);
            const ColumnInfo& info(en->columnInfo[i]);

            // If no stride is supplied for a column, assume that it is densely packed
            ASSERT(info.decodedSize != 0);
            ASSERT(c.data);
            if (c.stride == 0) {
                c.stride = info.decodedSize;
            }

            // sanity checks
            ASSERT(c.stride % sizeof(double) == 0);
            ASSERT(info.decodedSize % sizeof(double) == 0);
        }
    }
}

void odc_encode_to_data_handle(odb_encoder_t* en, eckit::DataHandle& dh) {

    ASSERT(en);
    ASSERT(en->nrows > 0);
    ASSERT(en->columnData.size() == en->columnInfo.size());
    ASSERT(en->maxRowsPerFrame > 0);

    fill_in_encoder(en);

    size_t ncolumns = en->columnData.size();
    ASSERT(ncolumns > 0);

    std::vector<ConstStridedData> stridedData;
    stridedData.reserve(ncolumns);

    for (size_t i = 0; i < ncolumns; i++) {
        const ColumnInfo& info{en->columnInfo[i]};
        const odb_encoder_t::EncodeColumn& c{en->columnData[i]};
        stridedData.emplace_back(ConstStridedData {c.data, en->nrows, info.decodedSize, c.stride});
    }

    ::odc::api::encode(dh, en->columnInfo, stridedData, en->maxRowsPerFrame);
}

typedef long (*stream_write_t)(void* handle, const void* buffer, long length);

long odc_encode_to_stream(odb_encoder_t* en, void* handle, stream_write_t write_fn) {

    // Wrap the stream in a DataHandle
    struct WriteStreamDataHandle : public eckit::DataHandle {
        WriteStreamDataHandle(void* handle, stream_write_t fn) : handle_(handle), fn_(fn), pos_(0) {}
        virtual ~WriteStreamDataHandle() {}
        void print(std::ostream& s) const override { s << "StreamReadHandle(" << fn_ << "(" << handle_ << "))"; }
        Length openForRead() override { NOTIMP; }
        void openForWrite(const Length&) override {}
        void openForAppend(const Length&) override {}
        long read(void*, long) override { NOTIMP; }
        long write(const void* buffer, long length) override {
            long written = fn_(handle_, buffer, length);
            pos_ += written;
            return written;
        }
        Offset position() override { return pos_; }
        void close() {}

        void* handle_;
        stream_write_t fn_;
        Offset pos_;
    };

    return wrapApiFunction([en, handle, write_fn] {
        WriteStreamDataHandle dh(handle, write_fn);
        dh.openForWrite(0);
        AutoClose closer(dh);
        odc_encode_to_data_handle(en, dh);
        return dh.position();
    });
}

long odc_encode_to_file_descriptor(odb_encoder_t* en, int fd) {
    return wrapApiFunction([en, fd] {
        FileDescHandle dh(fd);
        dh.openForWrite(0);
        AutoClose closer(dh);
        odc_encode_to_data_handle(en, dh);
        return dh.position();
    });
}

long odc_encode_to_buffer(odb_encoder_t* en, void* buffer, long length) {
    return wrapApiFunction([en, buffer, length] {
        MemoryHandle dh(buffer, length);
        dh.openForWrite(0);
        AutoClose closer(dh);
        odc_encode_to_data_handle(en, dh);
        return dh.position();
    });
}

//----------------------------------------------------------------------------------------------------------------------

} // extern "C"
