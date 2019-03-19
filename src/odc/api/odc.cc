/*
 * (C) Copyright 2019- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <functional>
#include <cstdint>
#include <unistd.h>

#include "eckit/exception/Exceptions.h"
#include "eckit/maths/Functions.h"
#include "eckit/runtime/Main.h"
#include "eckit/io/MemoryHandle.h"
#include "eckit/io/FileDescHandle.h"

#include "odc/api/odc.h"
#include "odc/api/Odc.h"

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
//      ODC_ERRORS_REPORT  - All exceptions will be caught and odc_errno set
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

//----------------------------------------------------------------------------------------------------------------------

// Types for lookup

static_assert(ODC_NUM_TYPES == NUM_TYPES, "Number of types must match");

const char* ODC_TYPE_NAMES[] = {
    OdbTypes<ColumnType(0)>::name,
    OdbTypes<ColumnType(1)>::name,
    OdbTypes<ColumnType(2)>::name,
    OdbTypes<ColumnType(3)>::name,
    OdbTypes<ColumnType(4)>::name,
    OdbTypes<ColumnType(5)>::name
};

//----------------------------------------------------------------------------------------------------------------------

/* Error handling */

static std::string g_current_error_str;
static int g_odc_error_behaviour = ODC_THROW;
int odc_errno = 0;

void odc_reset_error() {
    g_current_error_str.clear();
    odc_errno = 0;
}

void odc_error_handling(int handling_type) {
    ASSERT(handling_type >= ODC_THROW && handling_type <= ODC_ERRORS_REPORT);
    g_odc_error_behaviour = handling_type;
    odc_reset_error();
}

const char* odc_error_string() {
    if (g_current_error_str.empty()) {
        return 0;
    } else {
        return g_current_error_str.c_str();
    }
}

static void set_error(const std::string& s, int err) {
    g_current_error_str = s;
    odc_errno = err;
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
    if (g_odc_error_behaviour == ODC_ERRORS_CHECKED && odc_errno != 0) {
        std::stringstream ss;
        ss << "APi call being made after unchecked error: ("
           << odc_errno << "): " << odc_error_string()
           << ". SeriousBug in calling code";
        throw SeriousBug(ss.str(), Here());
    }

    try {
        return f();
    } catch (Exception& e) {
        Log::error() << "Caught exception on C-C++ API boundary: " << e.what() << std::endl;
        if (g_odc_error_behaviour == ODC_THROW) throw;
        set_error(e.what(), 1);
    } catch (std::exception& e) {
        Log::error() << "Caught exception on C-C++ API boundary: " << e.what() << std::endl;
        if (g_odc_error_behaviour == ODC_THROW) throw;
        set_error(e.what(), 2);
    } catch (...) {
        Log::error() << "Caught unknown exception on C-C++ API boundary: " << std::endl;
        if (g_odc_error_behaviour == ODC_THROW) throw;
        set_error("Unexpected exception caught", 3);
    }

    // Return default constructed value.
    return DefaultConstructed<decltype(f())>();
}

}

extern "C" {

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

const char* odc_type_name(int type) {
    return wrapApiFunction([type] {
        ASSERT(type >= 0);
        ASSERT(type < NUM_TYPES);
        return ODC_TYPE_NAMES[type];
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

/* Basic READ objects */

odb_t* odc_open_for_read(const char* filename) {
    return wrapApiFunction([filename] {
        return new odb_t {filename};
    });
}

odb_t* odc_open_from_fd(int fd) {
    return wrapApiFunction([fd] {
        // Take a copy of the file descriptor. This allows us to decouple the life of this
        // from the life of the caller
        int fd2 = dup(fd);
        if (fd == -1) throw CantOpenFile("dup() failed on supplied file descriptor", Here());
        return new odb_t {new FileDescHandle(fd2, true)};
    });
}

void odc_close(odb_t* o) {
    return wrapApiFunction([o]{
        ASSERT(o);
        delete o;
    });
}

/*
 * Table handling
 */

odb_table_t* odc_next_table(odb_t* o, bool aggregated) {
    return wrapApiFunction([o, aggregated] {
        eckit::Log::info() << "Aggregated: " << (aggregated ? "T": "F") << std::endl;
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

int odc_table_num_rows(const odb_table_t* t) {
    return wrapApiFunction([t] {
        ASSERT(t);
        return t->internal.numRows();
    });
}

int odc_table_num_columns(const odb_table_t* t) {
    return wrapApiFunction([t] {
        ASSERT(t);
        return t->internal.numColumns();
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

/*
const odb_decoded_t* odc_table_decode_all(const odb_table_t* t) {
    return wrapApiFunction([t] {

        const Table& tbl(t->internal);

        size_t nrows = tbl.numRows();
        size_t ncols = tbl.numColumns();

        // Allocate the object to return

        odb_decoded_t* dt = new odb_decoded_t;

        dt->ncolumns = ncols;
        dt->nrows = nrows;
        dt->columns = 0;
        dt->columnData = new odb_strided_data_t[ncols];

        // Fill in column details

        uintptr_t totalRowSize = 0;
        for (size_t col = 0; col < ncols; ++col) {
            dt->columnData[col].elemSize = tbl.columnInfo()[col].decodedSize;
//            Log::info() << "Setting facade: " << col << " - " << nrows << std::endl;
            dt->columnData[col].nelem = nrows;
            dt->columnData[col].data = reinterpret_cast<char*>(totalRowSize); // Store offset. Update with ptr later.
            totalRowSize += round(dt->columnData[col].elemSize, sizeof(double));
        }

        // Allocate the storage (and assign the strided data to the columns)

        dt->ownedData = new char[totalRowSize * nrows];
        for (size_t col = 0; col < ncols; ++col) {
            dt->columnData[col].data = dt->ownedData + reinterpret_cast<uintptr_t>(dt->columnData[col].data);
            dt->columnData[col].stride = totalRowSize;
        }

        // And do the actual decoding!

        odc_table_decode(t, dt, 1);

        return dt;
    });
}*/

long odc_table_decode(const odb_table_t* t, int ncolumns, long nrows, odb_strided_column_t* columnData, int nthreads) {
    return wrapApiFunction([t, ncolumns, nrows, columnData, nthreads] {

        // Sanity checking

        long frame_rows = t->internal.numRows();
        long frame_cols = t->internal.numColumns();

        ASSERT(ncolumns <= long(frame_cols));
        ASSERT(nrows >= long(frame_rows));
        ASSERT(columnData);

        // Construct C++ API adapter

        std::vector<StridedData> dataFacade;
        dataFacade.reserve(ncolumns);
        std::vector<std::string> columnNames;
        columnNames.reserve(ncolumns);

        for (int i = 0; i < ncolumns; i++) {
            auto& col(columnData[i]);
//            Log::info() << "Facade (" << i << "): " << col.nelem << " : " << col.elemSize << " -- " << nrows << std::endl;
            ASSERT(col.nelem >= long(frame_rows));
            dataFacade.emplace_back(col.data, col.nelem, col.elemSize, col.stride);
            columnNames.emplace_back(col.name);
        }

        DecodeTarget target(columnNames, dataFacade);

        // Do the decoder

        ASSERT(nthreads >= 1);
        t->internal.decode(target, static_cast<size_t>(nthreads));

        // And return the values

        for (int i = 0; i < ncolumns; i++) columnData[i].nelem = frame_rows;
        return frame_rows;
    });
}

/*void odc_free_odb_decoded(const odb_decoded_t* dt) {
    return wrapApiFunction([dt] {
        ASSERT(dt);
        ASSERT(dt->columnData);
        if (dt->ownedData) delete [] dt->ownedData;
        delete [] dt->columnData;
        delete dt;
    });
}*/


void odc_table_encode_dh(eckit::DataHandle& dh,
                         int ncolumns,
                         long nrows,
                         odb_strided_column_t* columnData,
                         long maxRowsPerFrame) {

    ASSERT(maxRowsPerFrame > 0);
    ASSERT(ncolumns > 0);

    std::vector<ColumnInfo> colInfo;
    std::vector<ConstStridedData> data;
    colInfo.reserve(ncolumns);
    data.reserve(ncolumns);

    for (int i = 0; i < ncolumns; i++) {

        const odb_strided_column_t& c(columnData[i]);
        ASSERT(c.nelem == nrows);

        colInfo.emplace_back(ColumnInfo {c.name, ColumnType(c.type), size_t(c.elemSize), {}});
        data.emplace_back(ConstStridedData {c.data, size_t(nrows), size_t(c.elemSize), size_t(c.stride)});
    }

    ::odc::api::encode(dh, colInfo, data, static_cast<size_t>(maxRowsPerFrame));
}

void odc_table_encode(void* buffer, long bufferSize,
                      int ncolumns, long nrows,
                      struct odb_strided_column_t* columnData,
                      long maxRowsPerFrame=10000) {

    return wrapApiFunction([buffer, bufferSize, ncolumns, nrows, columnData, maxRowsPerFrame] {

        // TODO: Allocate and return a buffer, as per the routines below?
        MemoryHandle dh(buffer, bufferSize);
        dh.openForWrite(0);
        AutoClose closer(dh);
        odc_table_encode_dh(dh, ncolumns, nrows, columnData, maxRowsPerFrame);
    });
}

void odc_table_encode_to_file(int fd, int ncolumns, long nrows,
                              struct odb_strided_column_t* columnData,
                              long maxRowsPerFrame=10000) {
    return wrapApiFunction([fd, ncolumns, nrows, columnData, maxRowsPerFrame] {
        FileDescHandle dh(fd);
        dh.openForWrite(0);
        AutoClose closer(dh);
        odc_table_encode_dh(dh, ncolumns, nrows, columnData, maxRowsPerFrame);
    });
}

void* odc_import_encode_text(const char* data, const char* delimiter, void* buffer, long* size) {
    return wrapApiFunction([data, buffer, size, delimiter] {

        ASSERT(*size >= 0);
        MemoryHandle input_dh(data, ::strlen(data));
        std::unique_ptr<MemoryHandle> output_dh(buffer ? new MemoryHandle(buffer, size_t(*size)) : new MemoryHandle);

        input_dh.openForRead();
        output_dh->openForWrite(0);
        AutoClose close_in(input_dh);
        AutoClose close_out(*output_dh);

        importText(input_dh, *output_dh, delimiter);
        *size = output_dh->position();

        if (buffer) return buffer;

        // Allocate memory that can be freed on the client side

        void* output = ::malloc(output_dh->position());
        ::memcpy(output, output_dh->data(), output_dh->position());
        *size = output_dh->position();
        return output;
    });
}

void* odc_import_encode_file(int fd, const char* delimiter, void* buffer, long* size) {
    return wrapApiFunction([fd, buffer, size, delimiter] {

        ASSERT(*size >= 0);
        FileDescHandle input_dh(fd);
        std::unique_ptr<MemoryHandle> output_dh(buffer ? new MemoryHandle(buffer, size_t(*size)) : new MemoryHandle);

        input_dh.openForRead();
        output_dh->openForWrite(0);
        AutoClose close_in(input_dh);
        AutoClose close_out(*output_dh);

        importText(input_dh, *output_dh, delimiter);
        *size = output_dh->position();

        if (buffer) return buffer;

        // Allocate memory that can be freed on the client side

        void* output = ::malloc(output_dh->position());
        ::memcpy(output, output_dh->data(), output_dh->position());
        *size = output_dh->position();
        return output;
    });
}

void odc_import_encode_file_to_file(int input_fd, int output_fd, const char* delimiter) {
    return wrapApiFunction([input_fd, output_fd, delimiter] {

        FileDescHandle input_dh(input_fd);
        FileDescHandle output_dh(output_fd);

        input_dh.openForRead();
        output_dh.openForWrite(0);
        AutoClose close_in(input_dh);
        AutoClose close_out(output_dh);

        importText(input_dh, output_dh, delimiter);
    });
}

//----------------------------------------------------------------------------------------------------------------------

} // extern "C"
