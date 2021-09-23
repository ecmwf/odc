/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/DataHandle.h"
#include "eckit/log/Log.h"

#include "odc/core/Codec.h"
#include "odc/core/Header.h"
#include "odc/LibOdc.h"
#include "odc/Reader.h"
#include "odc/ReaderIterator.h"

using namespace eckit;
using namespace odc::core;

namespace odc {

ReaderIterator::ReaderIterator(Reader &owner)
: owner_(owner),
  columns_(0),
  lastValues_(0),
  columnOffsets_(0),
  rowDataSizeDoubles_(0),
  nrows_(0),
  rowsRemainingInTable_(0),
  f_(owner_.dataHandle()->clone()),
  newDataset_(false),
  rowDataBuffer_(0),
  noMore_(false),
  headerCounter_(0),
  byteOrder_(BYTE_ORDER_INDICATOR),
  refCount_(0)
{
	ASSERT(f_);
    f_->openForRead();

	loadHeaderAndBufferData();
}

eckit::DataHandle* ReaderIterator::dataHandle()
{
    ASSERT(f_);
    return f_.get();
}

ReaderIterator::ReaderIterator(Reader &owner, const PathName& pathName)
: owner_(owner),
  columns_(0),
  lastValues_(0),
  columnOffsets_(0),
  rowDataSizeDoubles_(0),
  nrows_(0),
  rowsRemainingInTable_(0),
  f_(pathName.fileHandle()),
  newDataset_(false),
  rowDataBuffer_(0),
  noMore_(false),
  headerCounter_(0),
  byteOrder_(BYTE_ORDER_INDICATOR),
  refCount_(0)
{
	ASSERT(f_);
	f_->openForRead();

	loadHeaderAndBufferData();
}

bool ReaderIterator::loadHeaderAndBufferData() {

    if (noMore_) return false;

    ASSERT(rowsRemainingInTable_ == 0);

    // Keep going until we find a valid header, or run out of data
    // n.b. an empty table is legit, so we need a loop.

    while (true) {

        // Check the magic. If no more data, we are done

        if (!Header::readMagic(*f_)) {
            noMore_ = true;
            return false;
        }

        // Read in the rest of the header

        Header header(columns_, properties_);
        header.loadAfterMagic(*f_);

        byteOrder_ = header.byteOrder();
        rowDataSizeDoubles_ = rowDataSizeDoublesInternal();
        ++headerCounter_;

        // Ensure the decode buffers are all set up

        initRowBuffer();

        // Read in the data into a buffer and initialise the DataStream.

        size_t dataSize = header.dataSize();

        // It is perfectly legitimate to have zero rows in an ODB. If that is the case,
        // then loop around again.

        if (dataSize == 0) {
            ASSERT(header.rowsNumber() == 0);
        } else {

            // Read the expected data into the rows buffer.

            ASSERT(header.rowsNumber() != 0);
            ASSERT(dataSize >= 2);

            if (!readBuffer(dataSize)) {
                // See ODB-376
                throw SeriousBug("Expected row data to follow table header");
            }

            // And we are done
            newDataset_ = true;
            rowsRemainingInTable_ = header.rowsNumber();
            return true;
        }
    }
}

ReaderIterator::~ReaderIterator () noexcept(false)
{
	LOG_DEBUG_LIB(LibOdc) << "ReaderIterator::~ReaderIterator: headers read: " << headerCounter_ << " rows:" << nrows_ << std::endl;

	close();
	delete [] lastValues_;
    delete [] columnOffsets_;
}


bool ReaderIterator::operator!=(const ReaderIterator& other)
{
	//ASSERT(&other == 0);
	return noMore_;
}

void ReaderIterator::initRowBuffer()
{
    int32_t numDoubles = rowDataSizeDoubles();
    size_t nCols = columns().size();

	delete [] lastValues_;
    lastValues_ = new double [numDoubles];

    codecs_.clear();
    codecs_.resize(nCols, 0);

    delete [] columnOffsets_;
    columnOffsets_ = new size_t[nCols];

    size_t offset = 0;
	for(size_t i = 0; i < nCols; i++)
	{
		codecs_[i] = &columns()[i]->coder();
        lastValues_[offset] = codecs_[i]->missingValue();
        columnOffsets_[i] = offset;
        offset += columns()[i]->dataSizeDoubles();
    }
}

size_t ReaderIterator::readBuffer(size_t dataSize) {

    // Ensure we have enough buffer space

    if (rowDataBuffer_.size() < dataSize) {
        rowDataBuffer_ = eckit::Buffer(dataSize);
    }

    // Read the data into a buffer

    size_t bytesRead = f_->read(rowDataBuffer_, dataSize);
    if (bytesRead == 0) return 0;

    if (bytesRead != dataSize) {
        std::stringstream ss;
        ss << "Failed to read " << dataSize << " bytes of encoded data";
        throw ODBIncomplete(ss.str(), Here());
    }

    // Assign the data to a DataStream.

    rowDataStream_ = GeneralDataStream(byteOrder_ != BYTE_ORDER_INDICATOR, rowDataBuffer_);

    // Assign the appropriate data stream to each of the codecs.

    for (auto& codec : codecs_) codec->setDataStream(rowDataStream_);

	return bytesRead;
}

bool ReaderIterator::next()
{
    newDataset_ = false;
    if (noMore_)
        return false; 

    if (rowsRemainingInTable_ == 0) {
        if (!loadHeaderAndBufferData()) return false;
        ASSERT(rowsRemainingInTable_ != 0);
    }

    unsigned char marker[2];
    rowDataStream_.readBytes(marker, sizeof(marker));

    int startCol = (marker[0] * 256) + marker[1];

	size_t nCols = columns().size();
    for(size_t i = startCol; i < nCols; i++) {
        codecs_[i]->decode(&lastValues_[columnOffsets_[i]]);
    }

	++nrows_ ;
    --rowsRemainingInTable_;
	return nCols;
}

size_t ReaderIterator::rowDataSizeDoublesInternal() const {

    size_t total = 0;
    for (const auto& column : columns()) {
        total += column->dataSizeDoubles();
    }
    return total;
}


bool ReaderIterator::isNewDataset() { return newDataset_; }

double& ReaderIterator::data(size_t i)
{
	ASSERT(i >= 0 && i < columns().size());
    return lastValues_[columnOffsets_[i]];
}

int ReaderIterator::close()
{
    if (f_) f_->close();
    f_.reset();
	return 0;
}


std::string ReaderIterator::property(std::string key)
{
	return properties_[key];
}


api::ColumnType ReaderIterator::columnType(unsigned long index) { return columns_[index]->type(); }
const std::string& ReaderIterator::columnName(unsigned long index) const { return columns_[index]->name(); }
const std::string& ReaderIterator::codecName(unsigned long index) const { return columns_[index]->coder().name(); }
double ReaderIterator::columnMissingValue(unsigned long index) { return columns_[index]->missingValue(); }
const eckit::sql::BitfieldDef& ReaderIterator::bitfieldDef(unsigned long index) { return columns_[index]->bitfieldDef(); }

} // namespace odc

