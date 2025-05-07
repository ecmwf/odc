/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

///
/// \file WriterBufferingIterator.h
///
/// @author Piotr Kuchta, August 2009

#ifndef odc_WriterBufferingIterator_H
#define odc_WriterBufferingIterator_H

#include "eckit/filesystem/PathName.h"
#include "eckit/io/HandleHolder.h"
#include "eckit/log/Log.h"

#include "odc/IteratorProxy.h"
#include "odc/LibOdc.h"
#include "odc/codec/CodecOptimizer.h"

namespace eckit {
class PathName;
}
namespace eckit {
class DataHandle;
}

namespace odc {

//----------------------------------------------------------------------------------------------------------------------

template <typename I>
class Writer;
namespace sql {
class TableDef;
}

class WriterBufferingIterator : public eckit::HandleHolder {
public:

    typedef Writer<WriterBufferingIterator> Owner;

    // WriterBufferingIterator (Owner &owner, eckit::DataHandle *, bool openDataHandle=true);
    WriterBufferingIterator(Owner& owner, eckit::DataHandle*, bool openDataHandle,
                            const odc::sql::TableDef* tableDef = 0);
    WriterBufferingIterator(Owner& owner, eckit::DataHandle&, bool openDataHandle,
                            const odc::sql::TableDef* tableDef = 0);

    ~WriterBufferingIterator();

    int open();

    double* data();
    double& data(size_t i);

    int setColumn(size_t index, std::string name, api::ColumnType type);
    int setBitfieldColumn(size_t index, std::string name, api::ColumnType type, eckit::sql::BitfieldDef b);

    void missingValue(size_t i, double);

    template <typename T>
    unsigned long pass1(T&, const T&);
    unsigned long gatherStats(const double* values, unsigned long count);

    int close();

    const core::MetaData& columns() const { return columns_; }
    const core::MetaData& columns(const core::MetaData& md) {
        columns_ = md;
        for (auto& col : columns_)
            col->resetCodec<core::SameByteOrder>();  // We want to use the default codecs for encoding
        initialisedColumns_ = columns_.allColumnsInitialised();
        return columns_;
    }

    void setNumberOfColumns(size_t n) { columns_.setSize(n); }

    Owner& owner() { return owner_; }

    eckit::DataHandle& dataHandle() { return handle(); }

    void property(std::string key, std::string value) { properties_[key] = value; }

    /// The offset of a given column in the doubles[] data array
    size_t dataOffset(size_t i) const {
        ASSERT(columnOffsets_);
        return columnOffsets_[i];
    }

    // protected:

    int setOptimalCodecs();

    void writeHeader();

    int writeRow(const double* values, unsigned long count);

    // Get the number of doubles per row.
    size_t rowDataSizeDoubles() const { return rowDataSizeDoubles_; }

    size_t rowsBufferSize() { return rowsBufferSize_; }
    void rowsBufferSize(size_t n) { rowsBufferSize_ = n; }

    void flush();

    std::vector<eckit::PathName> outputFiles();
    bool next();

    // If we are encoding strings, and the relevant string column size changes, we need
    // to restart the encoding process
    void flushAndResetColumnSizes(const std::map<std::string, size_t>& resetColumnSizeDoubles);

private:

    size_t rowDataSizeDoublesInternal() const;

public:

    int refCount_;

protected:

    Owner& owner_;
    core::MetaData columns_;
    double* lastValues_;
    double* nextRow_;
    size_t* columnOffsets_;  // in doubles
    size_t* columnByteSizes_;
    unsigned long long nrows_;

    eckit::PathName path_;

private:

    // No copy allowed.
    WriterBufferingIterator(const WriterBufferingIterator&);
    WriterBufferingIterator& operator=(const WriterBufferingIterator&);

    template <typename T>
    void pass1init(T&, const T&);

    std::pair<eckit::Buffer, size_t> serializeHeader(size_t dataSize, size_t rowsNumber);

    void allocBuffers();
    void allocRowsBuffer();
    void resetColumnsBuffer();

    int doWriteRow(core::DataStream<core::SameByteOrder>& stream, const double* values);

    bool initialisedColumns_;
    core::Properties properties_;

    eckit::Buffer rowsBuffer_;
    unsigned char* nextRowInBuffer_;

    size_t rowsBufferSize_;
    size_t rowDataSizeDoubles_;
    size_t rowByteSize_;

    codec::CodecOptimizer codecOptimizer_;

    const odc::sql::TableDef* tableDef_;

private:

    bool openDataHandle_;

    friend class IteratorProxy<WriterBufferingIterator, Owner>;
};

template <typename T>
void WriterBufferingIterator::pass1init(T& it, const T& end) {
    LOG_DEBUG_LIB(LibOdc) << "WriterBufferingIterator::pass1init" << std::endl;

    // Copy columns from the input iterator.
    columns(it->columns());

    columns_.resetCodecs<core::SameByteOrder>();
    columns_.resetStats();

    size_t nCols = it->columns().size();
    ASSERT(nCols > 0);

    allocRowsBuffer();
}

template <typename T>
unsigned long WriterBufferingIterator::pass1(T& it, const T& end) {
    LOG_DEBUG_LIB(LibOdc) << "WriterBufferingIterator::pass1" << std::endl;

    pass1init(it, end);
    writeHeader();

    unsigned long nrows = 0;
    for (; it != end; ++it, ++nrows) {
        if (it->isNewDataset() && it->columns() != columns()) {
            LOG_DEBUG_LIB(LibOdc) << "WriterBufferingIterator::pass1: Change of input metadata." << std::endl;
            flush();
            pass1init(it, end);
            writeHeader();
        }

        writeRow(it->data(), it->columns().size());
    }

    LOG_DEBUG_LIB(LibOdc) << "Flushing rest of the buffer..." << std::endl;
    flush();

    LOG_DEBUG_LIB(LibOdc) << "WriterBufferingIterator::pass1: processed " << nrows << " row(s)." << std::endl;
    ASSERT(close() == 0);
    return nrows;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace odc

#endif
