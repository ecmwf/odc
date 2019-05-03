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

struct odc_reader_t : public Reader {
    using Reader::Reader;
};

struct odc_frame_t : public Frame {
    using Frame::Frame;
};

struct odc_decoder_t {

    struct DecodeColumn {
        void* data;
        size_t elemSize;
        size_t stride;
        bool transpose;
    };

    odc_decoder_t() : nrows(0), dataWidth(0), dataHeight(0), externalData(0), columnMajor(true), ownedData() {}

    size_t nrows;
    std::vector<std::string> columnNames;
    std::vector<DecodeColumn> columnData;

    size_t dataWidth;
    size_t dataHeight;
    void* externalData;
    bool columnMajor;

    std::unique_ptr<char> ownedData;
};

struct odc_encoder_t {

    odc_encoder_t() : arrayData(0), columnMajor(false), nrows(0), arrayWidth(0), arrayHeight(0), maxRowsPerFrame(10000) {}

    struct EncodeColumn {
        const void* data;
        size_t stride;
    };

    const void* arrayData;
    bool columnMajor;

    size_t nrows;
    size_t arrayWidth;
    size_t arrayHeight;
    size_t maxRowsPerFrame;
    std::vector<ColumnInfo> columnInfo;
    std::vector<EncodeColumn> columnData;
};


//----------------------------------------------------------------------------------------------------------------------

/* Error handling */

static std::string g_current_error_str;

const char* odc_error_string(int err) {
    switch (err) {
    case ODC_SUCCESS:
        return "Success";
    case ODC_ERROR_ECKIT_EXCEPTION:
    case ODC_ERROR_GENERAL_EXCEPTION:
    case ODC_ERROR_UNKNOWN_EXCEPTION:
        return g_current_error_str.c_str();
    case ODC_ITERATION_COMPLETE:
        return "Iteration complete";
    default:
        return "<unknown>";
    };
}

} // extern "C"

// Template can't have C linkage

namespace {

// Template magic to provide a consistent error-handling approach

int innerWrapFn(std::function<int()> f) {
    return f();
}

int innerWrapFn(std::function<void()> f) {
    f();
    return ODC_SUCCESS;
}

template <typename FN>
int wrapApiFunction(FN f) {

    try {
        return innerWrapFn(f);
    } catch (Exception& e) {
        Log::error() << "Caught exception on C-C++ API boundary: " << e.what() << std::endl;
        g_current_error_str = e.what();
        return ODC_ERROR_ECKIT_EXCEPTION;
    } catch (std::exception& e) {
        Log::error() << "Caught exception on C-C++ API boundary: " << e.what() << std::endl;
        g_current_error_str = e.what();
        return ODC_ERROR_GENERAL_EXCEPTION;
    } catch (...) {
        Log::error() << "Caught unknown on C-C++ API boundary" << std::endl;
        g_current_error_str = "Unrecognised and unknown exception";
        return ODC_ERROR_UNKNOWN_EXCEPTION;
    }

    ASSERT(false);
}

}

extern "C" {

//----------------------------------------------------------------------------------------------------------------------

// Types for lookup

int odc_type_count(int* count) {
    return wrapApiFunction([count] {
        (*count) = NUM_TYPES;
    });
}

int odc_type_name(int type, const char** type_name) {
    return wrapApiFunction([type, type_name] {

        static const char* names[] = {
            OdbTypes<ColumnType(0)>::name,
            OdbTypes<ColumnType(1)>::name,
            OdbTypes<ColumnType(2)>::name,
            OdbTypes<ColumnType(3)>::name,
            OdbTypes<ColumnType(4)>::name,
            OdbTypes<ColumnType(5)>::name
        };

        if (type < 0 || size_t(type) >= sizeof(names)/sizeof(names[0])) {
            std::stringstream ss;
            ss << "Unknown type id: " << type;
            throw UserError(ss.str(), Here());
        }

        *type_name = names[type];
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

int odc_initialise_api() {
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

int odc_integer_behaviour(int integerBehaviour) {
    return wrapApiFunction([integerBehaviour] {
        if (integerBehaviour != ODC_INTEGERS_AS_DOUBLES && integerBehaviour != ODC_INTEGERS_AS_LONGS) {
            throw SeriousBug("ODC integer behaviour must be either ODC_INTEGERS_AS_DOUBLES or ODC_INTEGERS_AS_LONGS", Here());
        }
        Settings::treatIntegersAsDoubles(integerBehaviour == ODC_INTEGERS_AS_DOUBLES);
    });
}

int odc_missing_integer(long* missing_value) {
    return wrapApiFunction([missing_value] {
        ASSERT(missing_value);
        (*missing_value) = Settings::integerMissingValue();
    });
}

int odc_missing_double(double* missing_value) {
    return wrapApiFunction([missing_value] {
        ASSERT(missing_value);
        (*missing_value) = Settings::doubleMissingValue();
    });
}

int odc_set_missing_integer(long missing_integer) {
    return wrapApiFunction([missing_integer] {
        Settings::setIntegerMissingValue(missing_integer);
    });
}

int odc_set_missing_double(double missing_double) {
    return wrapApiFunction([missing_double] {
        Settings::setDoubleMissingValue(missing_double);
    });
}

int odc_version(const char** version) {
    return wrapApiFunction([version]{
        (*version) = Settings::version().c_str();
    });
}

int odc_git_sha1(const char** sha1) {
    return wrapApiFunction([sha1]{
        (*sha1) = Settings::gitsha1().c_str();
    });
}

//----------------------------------------------------------------------------------------------------------------------

/* Basic READ objects */

int odc_open_path(odc_reader_t** reader, const char* filename) {
    return wrapApiFunction([reader, filename] {
//        ASSERT(!(*reader));
        (*reader) = new odc_reader_t {filename};
    });
}

int odc_open_file_descriptor(odc_reader_t** reader, int fd) {
    return wrapApiFunction([reader, fd] {
        // Take a copy of the file descriptor. This allows us to decouple the life of this
        // from the life of the caller
        int fd2 = dup(fd);
        if (fd == -1) throw CantOpenFile("dup() failed on supplied file descriptor", Here());
        (*reader) = new odc_reader_t {new FileDescHandle(fd2, true)};
    });
}

int odc_open_buffer(odc_reader_t** reader, const void* data, long length) {
    return wrapApiFunction([reader, data, length] {
        (*reader) = new odc_reader_t {new MemoryHandle(data, length)};
    });

}

int odb_open_stream(odc_reader_t** reader, void* handle, stream_read_t stream_proc) {

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

    return wrapApiFunction([reader, handle, stream_proc] {
        (*reader) = new odc_reader_t(new ReadStreamDataHandle(handle, stream_proc));
    });
}

int odc_close(const odc_reader_t* reader) {
    return wrapApiFunction([reader]{
        ASSERT(reader);
        delete reader;
    });
}

//----------------------------------------------------------------------------------------------------------------------

/*
 * Table handling
 */

int odc_new_frame(odc_frame_t** frame, odc_reader_t* reader) {
    return wrapApiFunction([frame, reader] {
        ASSERT(reader);
        (*frame) = new odc_frame_t {*reader};
    });
}

int odc_free_frame(const odc_frame_t* frame) {
    return wrapApiFunction([frame] {
        ASSERT(frame);
        delete frame;
    });
}

int odc_next_frame(odc_frame_t* frame) {
    return wrapApiFunction(std::function<int()> {[frame] {
        ASSERT(frame);
        if (frame->next(false)) {
            return ODC_SUCCESS;
        } else {
            return ODC_ITERATION_COMPLETE;
        }
    }});
}

int odc_next_frame_aggregated(odc_frame_t* frame, long maximum_rows) {
    return wrapApiFunction(std::function<int()> {[frame, maximum_rows] {
        ASSERT(frame);
        if (frame->next(true, maximum_rows)) {
            return ODC_SUCCESS;
        } else {
            return ODC_ITERATION_COMPLETE;
        }
    }});
}

int odc_copy_frame(odc_frame_t* source_frame, odc_frame_t** copy) {
    return wrapApiFunction([source_frame, copy] {
        ASSERT(source_frame);
        (*copy) = new odc_frame_t {*source_frame};
    });
}

int odc_frame_row_count(const odc_frame_t* frame, long* count) {
    return wrapApiFunction([frame, count] {
        ASSERT(frame);
        ASSERT(count);
        (*count) = frame->rowCount();
    });
}

int odc_frame_column_count(const odc_frame_t* frame, int* count) {
    return wrapApiFunction([frame, count] {
        ASSERT(frame);
        ASSERT(count);
        (*count) = frame->columnCount();
    });
}

int odc_frame_column_attrs(const odc_frame_t* frame,
                           int col,
                           const char** name,
                           int* type,
                           int* element_size,
                           int* bitfield_count) {
    return wrapApiFunction([frame, col, name, type, element_size, bitfield_count] {
        ASSERT(frame);
        const auto& ci(frame->columnInfo());
        ASSERT(col >= 0 && size_t(col) < ci.size());
        const auto& colInfo(ci[col]);

        if (name) (*name) = colInfo.name.c_str();
        if (type) (*type) = colInfo.type;
        if (element_size) (*element_size) = colInfo.decodedSize;
        if (bitfield_count) (*bitfield_count) = colInfo.bitfield.size();
    });
}

int odc_frame_bitfield_attrs(const odc_frame_t* frame, int col, int field, const char** name, int* offset, int* size) {
    return wrapApiFunction([frame, col, field, name, offset, size] {
        ASSERT(frame);
        const auto& ci(frame->columnInfo());
        ASSERT(col >= 0 && size_t(col) < ci.size());
        const auto& colInfo(ci[col]);
        ASSERT(field >= 0 && size_t(field) < colInfo.bitfield.size());

        if (name) (*name) = colInfo.bitfield[field].name.c_str();
        if (offset) (*offset) = colInfo.bitfield[field].offset;
        if (size) (*size) = colInfo.bitfield[field].size;
    });
}

//----------------------------------------------------------------------------------------------------------------------

/* Decode functionality */

int odc_new_decoder(odc_decoder_t** decoder) {
    return wrapApiFunction([decoder] {
        (*decoder) = new odc_decoder_t;
    });
}

int odc_free_decoder(const odc_decoder_t* decoder) {
    return wrapApiFunction([decoder] {
        delete decoder;
    });
}

int odc_decoder_defaults_from_frame(odc_decoder_t* decoder, const odc_frame_t* frame) {
    return wrapApiFunction([decoder, frame] {

        ASSERT(decoder);
        ASSERT(frame);

        size_t nrows = frame->rowCount();
        size_t ncols = frame->columnCount();

        decoder->nrows = nrows;

        // Fill in column details

        for (size_t col = 0; col < ncols; ++col) {
            odc_decoder_add_column(decoder, frame->columnInfo()[col].name.c_str());
        }
    });
}

int odc_decoder_set_row_count(odc_decoder_t* decoder, long nrows) {
    return wrapApiFunction([decoder, nrows] {
        ASSERT(decoder);
        decoder->nrows = nrows;
    });
}

int odc_decoder_row_count(const odc_decoder_t* decoder, long* nrows) {
    return wrapApiFunction([decoder, nrows] {
        ASSERT(decoder);
        ASSERT(nrows);
        (*nrows) = decoder->nrows;
    });
}

int odc_decoder_set_data_array(odc_decoder_t* decoder, void* buffer, long width, long height, bool columnMajor) {
    return wrapApiFunction([decoder, buffer, width, height, columnMajor] {
        ASSERT(decoder);
        ASSERT(buffer);
        ASSERT(width > 0);
        ASSERT(height > 0);
        decoder->externalData = buffer;
        decoder->dataWidth = width;
        decoder->dataHeight = height;
        decoder->columnMajor = columnMajor;
        decoder->nrows = height;
    });
}

int odc_decoder_data_array(const odc_decoder_t* decoder, const void** data, long* width, long* height, bool* columnMajor) {
    return wrapApiFunction([decoder, data, width, height, columnMajor] {
        ASSERT(decoder);

        if (data) {
            if (decoder->ownedData) {
                (*data) = decoder->ownedData.get();
            } else {
                ASSERT(decoder->externalData);
                (*data) = decoder->externalData;
            }
        }

        if (width) (*width) = decoder->dataWidth;
        if (height) (*height) = decoder->dataHeight;
        if (columnMajor) (*columnMajor) = decoder->columnMajor;
    });
}

int odc_decoder_add_column(odc_decoder_t* decoder, const char* name) {
    return wrapApiFunction([decoder, name] {
        ASSERT(decoder);
        ASSERT(name);
        decoder->columnNames.emplace_back(name);
        decoder->columnData.emplace_back(odc_decoder_t::DecodeColumn {0, 0, 0, false});
    });
}

int odc_decoder_column_count(const odc_decoder_t* decoder, int* count) {
    return wrapApiFunction([decoder, count] {
        ASSERT(decoder);
        ASSERT(count);
        (*count) = decoder->columnNames.size();
    });
}

int odc_decoder_column_set_attrs(odc_decoder_t* decoder, int col, int element_size, int stride, void* data) {
    return wrapApiFunction([decoder, col, element_size, stride, data] {
        ASSERT(decoder);
        ASSERT(col >= 0 && size_t(col) < decoder->columnData.size());

        auto& cd(decoder->columnData[col]);
        cd.elemSize = element_size;
        cd.stride = stride;
        cd.data = data;
    });
}

int odc_decoder_column_attrs(const odc_decoder_t* decoder, int col, int* element_size, int* stride, const void** data) {
    return wrapApiFunction([decoder, col, element_size, stride, data] {
        ASSERT(decoder);
        ASSERT(col >= 0 && size_t(col) < decoder->columnData.size());

        auto& cd(decoder->columnData[col]);
        if (element_size) (*element_size) = cd.elemSize;
        if (stride) (*stride) = cd.stride;
        if (data) (*data) = cd.data;
    });
}

static void fill_in_decoder(odc_decoder_t* decoder, const odc_frame_t* frame) {

    if (decoder->nrows == 0) {
        decoder->nrows = frame->rowCount();
    }

    size_t height = decoder->nrows;  // in rows
    size_t width = 0;                // in bytes
    std::vector<std::pair<size_t, long>> offsets;

    for (size_t i = 0; i < decoder->columnData.size(); ++i) {
        odc_decoder_t::DecodeColumn& col(decoder->columnData[i]);

        if (col.elemSize == 0) {
            if (col.data) {
                col.elemSize = sizeof(double); // backwards compatible default
            } else {
                const std::string& colName(decoder->columnNames[i]);
                auto it = std::find_if(frame->columnInfo().begin(), frame->columnInfo().end(),
                                       [&colName](const ColumnInfo& ci) { return ci.name == colName; });
                ASSERT(it != frame->columnInfo().end());
                ASSERT(it->decodedSize > 0);
                ASSERT(it->decodedSize % sizeof(double) == 0);
                col.elemSize = it->decodedSize;
            }
            width += col.elemSize;
        }
    }

    if (width != 0 && height != 0) {
        void* dataPtr;
        if (decoder->externalData) {
            decoder->ownedData.reset();
            dataPtr = decoder->externalData;
            ASSERT(width <= decoder->dataWidth);
            ASSERT(height <= decoder->dataHeight);
            width = decoder->dataWidth;
            height = decoder->dataHeight;
        } else {
            decoder->ownedData.reset(new char[width * height]);
            decoder->dataWidth = width;
            decoder->dataHeight = height;
            dataPtr = decoder->ownedData.get();
        }

        size_t offset = 0;
        for (auto& col : decoder->columnData) {
            if (!col.data) {
                col.data = static_cast<char*>(dataPtr) + offset;
                if (decoder->columnMajor) {
                    col.stride = col.elemSize;
                    offset += height * col.elemSize;
                    if (col.elemSize > 8) col.transpose = true;
                } else {
                    col.stride = width;
                    offset += col.elemSize;
                }
            } else {
                if (col.stride == 0) col.stride = col.elemSize;
            }
        }
    }
}


int odc_decode_threaded(odc_decoder_t* decoder, const odc_frame_t* frame, long* rows_decoded, int nthreads) {
    return wrapApiFunction([decoder, frame, rows_decoded, nthreads] {

        ASSERT(decoder);
        ASSERT(frame);

        // Sanity checking

        size_t frame_rows = frame->rowCount();
        size_t frame_cols = frame->columnCount();

        ASSERT(decoder->columnData.size() == decoder->columnNames.size());
        ASSERT(decoder->columnNames.size() <= frame_cols);
        ASSERT(decoder->nrows >= frame_rows);

        // Fill in and allocate decode target as required

        fill_in_decoder(decoder, frame);

        // Construct C++ API adapter

        std::vector<StridedData> dataFacade;
        dataFacade.reserve(decoder->columnNames.size());

        for (auto& col : decoder->columnData) {
            dataFacade.emplace_back(StridedData{col.data, size_t(decoder->nrows), size_t(col.elemSize), size_t(col.stride)});
        }

        Decoder target(decoder->columnNames, dataFacade);

        // Do the decoder

        ASSERT(nthreads >= 1);
        frame->decode(target, static_cast<size_t>(nthreads));

        // For the cases where needed, reorder the data

        for (auto& col : decoder->columnData) {
            if (col.transpose) {
                ASSERT(col.elemSize == col.stride);
                size_t rows = decoder->nrows;
                size_t cols = col.elemSize / sizeof(8);
                double* output = static_cast<double*>(col.data);
                std::vector<double> tmpArray(output, output+(frame_rows * cols));
                for (size_t r = 0; r < frame_rows; r++) {
                    for (size_t c = 0; c < cols; c++) {
                        output[r + (c * rows)] = tmpArray[c + (r * cols)];
                    }
                }
            }
        }

        // And return the values

//        decoder->nrows = frame_rows;
        if (rows_decoded) *(rows_decoded) = frame_rows;
    });
}

int odc_decode(odc_decoder_t* decoder, const odc_frame_t* frame, long* rows_decoded) {
    return odc_decode_threaded(decoder, frame, rows_decoded, 1);
}

//----------------------------------------------------------------------------------------------------------------------

/* Encode functionality */

int odc_new_encoder(odc_encoder_t** encoder) {
    return wrapApiFunction([encoder] {
        (*encoder) = new odc_encoder_t;
    });
}

int odc_free_encoder(const odc_encoder_t* encoder) {
    return wrapApiFunction([encoder] {
        delete encoder;
    });
}

int odc_encoder_set_row_count(odc_encoder_t* encoder, long nrows) {
    return wrapApiFunction([encoder, nrows] {
        ASSERT(encoder);
        encoder->nrows = nrows;
    });
}

int odc_encoder_set_rows_per_frame(odc_encoder_t* encoder, long rows_per_frame) {
    return wrapApiFunction([encoder, rows_per_frame] {
        ASSERT(encoder);
        encoder->maxRowsPerFrame = rows_per_frame;
    });
}

int odc_encoder_set_data_array(odc_encoder_t* encoder, const void* data, long width, long height, bool columnMajor) {
    return wrapApiFunction([encoder, data, width, height, columnMajor] {
        ASSERT(encoder);
        encoder->arrayData = data;
        encoder->arrayWidth = width;
        encoder->arrayHeight = height;
        encoder->columnMajor = columnMajor;
        if (encoder->nrows == 0) encoder->nrows = height;
    });
}

int odc_encoder_add_column(odc_encoder_t* encoder, const char* name, int type) {
    return wrapApiFunction([encoder, name, type] {
        ASSERT(encoder);
        encoder->columnInfo.emplace_back(ColumnInfo{std::string(name), ColumnType(type)});
        encoder->columnData.emplace_back(odc_encoder_t::EncodeColumn {0, 0});
    });
}

int odc_encoder_column_set_attrs(odc_encoder_t* encoder, int col, int element_size, int stride, const void* data) {
    return wrapApiFunction([encoder, col, element_size, stride, data] {
        ASSERT(encoder);
        ASSERT(col >= 0 && size_t(col) < encoder->columnInfo.size());
        ASSERT(element_size >= 0 && element_size % 8 == 0);
        ASSERT(stride >= 0 && stride % 8 == 0);
        encoder->columnInfo[col].decodedSize = element_size;
        encoder->columnData[col].stride = stride;
        encoder->columnData[col].data = data;
    });
}

int odc_encoder_column_add_bitfield(odc_encoder_t* encoder, int col, const char* name, int nbits) {
    return wrapApiFunction([encoder, col, name, nbits] {
        ASSERT(encoder);
        ASSERT(col >= 0 && size_t(col) < encoder->columnInfo.size());

        size_t offset;
        ColumnInfo& ci(encoder->columnInfo[col]);
        if (ci.bitfield.size() == 0) {
            offset = 0;
        } else {
            offset = ci.bitfield.back().offset + ci.bitfield.back().size;
        }

        ASSERT(offset + nbits <= 32);
        ci.bitfield.emplace_back(ColumnInfo::Bit {name, nbits, int(offset)});
    });
}

void fill_in_encoder(odc_encoder_t* encoder, std::vector<std::unique_ptr<double[]>>& transposedData) {

    ASSERT(encoder->nrows > 0);
    ASSERT(encoder->columnData.size() == encoder->columnInfo.size());

    size_t decodedWidth = 0;
    for (auto& info : encoder->columnInfo) {
        if (info.decodedSize == 0) info.decodedSize = 8;
        decodedWidth += info.decodedSize;
    }

    if (encoder->arrayData) {

        /// We are encoding from an externally supplied array

        // Some sanity checks

        if (encoder->nrows > encoder->arrayHeight) {
            std::stringstream ss;
            ss << "Expected " << encoder->nrows << " rows, but array only contains " << encoder->arrayHeight;
            throw SeriousBug(ss.str(), Here());
        }

        if (decodedWidth > encoder->arrayWidth) {
            std::stringstream ss;
            ss << "Expected array width " << decodedWidth << " bytes, but array only contains " << encoder->arrayWidth;
            throw SeriousBug(ss.str(), Here());
        }

        // We are constructing the encoder from a 2D array of data.

        // n.b. There is a complication with arrays in column major order. There must be a column size
        //      (i.e. double). If the data size is greater than this, the data becomes non-contiguous
        //      and split across multiple columns. Need to aggregate it for encoding.

        size_t offset;
        for (size_t i = 0; i < encoder->columnData.size(); ++i) {
            odc_encoder_t::EncodeColumn& c(encoder->columnData[i]);
            const ColumnInfo& info(encoder->columnInfo[i]);

            ASSERT(info.decodedSize != 0);
            ASSERT(info.decodedSize % sizeof(double) == 0);
            ASSERT(c.data == 0);
            ASSERT(c.stride == 0);

            if (encoder->columnMajor) {
                c.stride = info.decodedSize;
                c.data = static_cast<const char*>(encoder->arrayData) + offset;
                if (info.decodedSize != sizeof(double)) {
                    // Transpose data for contiguous elementsn
                    size_t widthDoubles = info.decodedSize / sizeof(double);
                    transposedData.push_back(std::unique_ptr<double[]>(new double[widthDoubles * encoder->arrayHeight]));
                    const double* psrc = reinterpret_cast<const double*>(c.data);
                    double* ptgt = transposedData.back().get();
                    for (size_t y = 0; y < encoder->arrayHeight; ++y) {
                        for (size_t x = 0; x < widthDoubles; ++x) {
                            ptgt[x + (y * widthDoubles)] = psrc[y + (x * encoder->arrayHeight)];
                        }
                    }
                    c.data = ptgt;
                }
                offset += info.decodedSize * encoder->arrayHeight;
            } else {
                c.stride = encoder->arrayWidth;
                offset += info.decodedSize;
            }

            // sanity checks
            ASSERT(c.stride % sizeof(double) == 0);
        }
    } else {

        // We are constructing the data with a supplied structure per column

        for (size_t i = 0; i < encoder->columnData.size(); ++i) {
            odc_encoder_t::EncodeColumn& c(encoder->columnData[i]);
            const ColumnInfo& info(encoder->columnInfo[i]);

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

void odc_encode_to_data_handle(odc_encoder_t* encoder, eckit::DataHandle& dh) {

    ASSERT(encoder);
    ASSERT(encoder->nrows > 0);
    ASSERT(encoder->columnData.size() == encoder->columnInfo.size());
    ASSERT(encoder->maxRowsPerFrame > 0);

    // TransposedData allows column-oriented data to be reordered before encoding if needed.
    std::vector<std::unique_ptr<double[]>> transposedData;
    fill_in_encoder(encoder, transposedData);

    size_t ncolumns = encoder->columnData.size();
    ASSERT(ncolumns > 0);

    std::vector<ConstStridedData> stridedData;
    stridedData.reserve(ncolumns);


    for (size_t i = 0; i < ncolumns; i++) {
        const ColumnInfo& info{encoder->columnInfo[i]};
        const odc_encoder_t::EncodeColumn& c{encoder->columnData[i]};
        stridedData.emplace_back(ConstStridedData {c.data, encoder->nrows, info.decodedSize, c.stride});
    }

    ::odc::api::encode(dh, encoder->columnInfo, stridedData, encoder->maxRowsPerFrame);
}


int odc_encode_to_stream(odc_encoder_t* encoder, void* handle, stream_write_t write_fn, long* bytes_encoded) {

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

    return wrapApiFunction([encoder, handle, write_fn, bytes_encoded] {
        WriteStreamDataHandle dh(handle, write_fn);
        dh.openForWrite(0);
        AutoClose closer(dh);
        odc_encode_to_data_handle(encoder, dh);
        (*bytes_encoded) = dh.position();
    });
}

int odc_encode_to_file_descriptor(odc_encoder_t* encoder, int fd, long* bytes_encoded) {
    return wrapApiFunction([encoder, fd, bytes_encoded] {
        FileDescHandle dh(fd);
        dh.openForWrite(0);
        AutoClose closer(dh);
        odc_encode_to_data_handle(encoder, dh);
        (*bytes_encoded) = dh.position();
    });
}

int odc_encode_to_buffer(odc_encoder_t* encoder, void* buffer, long length, long* bytes_encoded) {
    return wrapApiFunction([encoder, buffer, length, bytes_encoded] {
        MemoryHandle dh(buffer, length);
        dh.openForWrite(0);
        AutoClose closer(dh);
        odc_encode_to_data_handle(encoder, dh);
        (*bytes_encoded) = dh.position();
    });
}

//----------------------------------------------------------------------------------------------------------------------

} // extern "C"
