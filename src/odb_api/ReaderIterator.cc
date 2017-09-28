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
/// \file ReaderIterator.cc
///
/// @author Piotr Kuchta, Feb 2009

#include <arpa/inet.h>

#include "ecml/data/DataHandleFactory.h"

#include "odb_api/Header.h"
#include "odb_api/Reader.h"
#include "odb_api/ReaderIterator.h"

using namespace eckit;

namespace odb {

ReaderIterator::ReaderIterator(Reader &owner)
: owner_(owner),
  columns_(0),
  lastValues_(0),
  codecs_(0),
  nrows_(0),
  f_(0),
  newDataset_(false),
  noMore_(false),
  ownsF_(false),
  headerCounter_(0),
  byteOrder_(BYTE_ORDER_INDICATOR),
  refCount_(0)
{
	f_ = owner.dataHandle();
	ASSERT(f_);

	loadHeaderAndBufferData();
}

ReaderIterator::ReaderIterator(Reader &owner, ecml::ExecutionContext*)
: owner_(owner),
  columns_(0),
  lastValues_(0),
  codecs_(0),
  nrows_(0),
  f_(0),
  newDataset_(false),
  noMore_(false),
  ownsF_(false),
  headerCounter_(0),
  byteOrder_(BYTE_ORDER_INDICATOR),
  refCount_(0)
{
	f_ = owner.dataHandle();
	ASSERT(f_);

	loadHeaderAndBufferData();
}

eckit::DataHandle* ReaderIterator::dataHandle()
{
    return f_;
}

ReaderIterator::ReaderIterator(Reader &owner, const PathName& pathName)
: owner_(owner),
  columns_(0),
  lastValues_(0),
  codecs_(0),
  nrows_(0),
  f_(0),
  newDataset_(false),
  noMore_(false),
  ownsF_(false),
  headerCounter_(0),
  byteOrder_(BYTE_ORDER_INDICATOR),
  refCount_(0)
{
    f_ = ecml::DataHandleFactory::openForRead(pathName);
	ASSERT(f_);
	ownsF_ = true;

	loadHeaderAndBufferData();
}

ReaderIterator::ReaderIterator(Reader &owner, const PathName& pathName, ecml::ExecutionContext*)
: owner_(owner),
  columns_(0),
  lastValues_(0),
  codecs_(0),
  nrows_(0),
  f_(0),
  newDataset_(false),
  noMore_(false),
  ownsF_(false),
  headerCounter_(0),
  byteOrder_(BYTE_ORDER_INDICATOR),
  refCount_(0)
{
	f_ = ecml::DataHandleFactory::openForRead(pathName);
	ASSERT(f_);
	ownsF_ = true;

	loadHeaderAndBufferData();
}

void ReaderIterator::loadHeaderAndBufferData()
{
	Header<ReaderIterator> header(*this);
	header.load();
	byteOrder_ = header.byteOrder();
	++headerCounter_;

	initRowBuffer();

	size_t dataSize = header.dataSize();
	memDataHandle_.size(dataSize);
	unsigned long bytesRead = f_->read(reinterpret_cast<char*>(memDataHandle_.buffer()), dataSize);

    if (bytesRead != dataSize)
        throw eckit::SeriousBug("Could not read the amount of data indicated by file's header");

    newDataset_ = true;
}

ReaderIterator::~ReaderIterator ()
{
	Log::debug() << "ReaderIterator::~ReaderIterator: headers read: " << headerCounter_ << " rows:" << nrows_ << std::endl;

	close();
	delete [] lastValues_;
	delete [] codecs_;
}


bool ReaderIterator::operator!=(const ReaderIterator& other)
{
	//ASSERT(&other == 0);
	return noMore_;
}

void ReaderIterator::initRowBuffer()
{
	size_t nCols = columns().size();

	delete [] lastValues_;
	lastValues_ = new double [nCols];

	delete [] codecs_;
	codecs_ = new odb::codec::Codec* [nCols];

	for(size_t i = 0; i < nCols; i++)
	{
		codecs_[i] = &columns()[i]->coder();
		lastValues_[i] = codecs_[i]->missingValue(); 
		codecs_[i]->dataHandle(&memDataHandle_);
	}
}

size_t ReaderIterator::readBuffer(size_t dataSize)
{
	memDataHandle_.size(dataSize);

	unsigned long bytesRead;
	if( (bytesRead = f_->read(memDataHandle_.buffer(), dataSize)) == 0)
		return 0;
	ASSERT(bytesRead == dataSize);
	return bytesRead;
}

bool ReaderIterator::next(ecml::ExecutionContext* context)
{
    newDataset_ = false;
    if (noMore_)
        return false; 

    uint16_t c = 0;
    long bytesRead = 0;

    if ( (bytesRead = memDataHandle_.read(&c, 2)) == 0) {

        // Keep going until we find a valid header, or run out of data
        // n.b. an empty table is legit, so we need a loop.

        bool found = false;
        do {

            // If we are at the end of the data, we are done.
            if ( (bytesRead = f_->read(&c, 2)) <= 0) {
                owner_.noMoreData();
                return ! (noMore_ = true);
            }
            ASSERT(bytesRead == 2);

            // The only legit thing to follow a table, is another table

            if (c != ODA_MAGIC_NUMBER)  {
                // memDataHandle_ is preloaded with all of the data associated with an ODB table according
                // to its header. If we have read data beyond the full table without finding the magic for
                // a new table, then this is a corrupt file, and we should report it as such, rather than
                // just treating this as more row data.

                // See ODB-376

                std::stringstream ss;
                ss << "Unexpected data found in ODB file \"" << f_->name()
                   << "\" at position " << (static_cast<long long>(f_->position())-2);
                throw BadValue(ss.str());
            }

            // Read in the rest of the header

            DataStream<SameByteOrder> ds(f_);

            unsigned char cc;
            ds.readUChar(cc); ASSERT(cc == 'O');
            ds.readUChar(cc); ASSERT(cc == 'D');
            ds.readUChar(cc); ASSERT(cc == 'A');

            Header<ReaderIterator> header(*this);
            header.loadAfterMagic();
            byteOrder_ = header.byteOrder();
            ++headerCounter_;
            initRowBuffer();

            size_t dataSize = header.dataSize();

            // If there are no rows in this table, loop around again to read the _next_ table

            if (dataSize == 0) {
                ASSERT(header.rowsNumber() == 0);
            } else {

                // We are now expecting rows. Read the data into the rows buffer, and continue with row reading

                ASSERT(header.rowsNumber() != 0);
                ASSERT(dataSize >= 2);

                if (!readBuffer(dataSize)) {
                    // See ODB-376
                    throw SeriousBug("Expected row data to follow table header");
                }

                bytesRead = memDataHandle_.read(&c, 2);
                ASSERT(bytesRead == 2);

                // We have found a new, valid table. Break out of the loop.
                found = true;
                newDataset_ = true;
            }

        } while(!found);
    }
	c = ntohs(c);

	size_t nCols = columns().size();
	for(size_t i = c; i < nCols; i++)
		lastValues_[i] = codecs_[i]->decode();

	++nrows_ ;
	return nCols;
}

bool ReaderIterator::isNewDataset() { return newDataset_; }

double& ReaderIterator::data(size_t i)
{
	ASSERT(i >= 0 && i < columns().size());
	return lastValues_[i];
}

int ReaderIterator::close()
{
	if (ownsF_ && f_)
	{
		f_->close();
		delete f_;
		f_ = 0;
	}

	return 0;
}


void ReaderIterator::property(std::string key, std::string value)
{
	properties_[key] = value;
}

std::string ReaderIterator::property(std::string key)
{
	return properties_[key];
}


ColumnType ReaderIterator::columnType(unsigned long index) { return columns_[index]->type(); }
const std::string& ReaderIterator::columnName(unsigned long index) const { return columns_[index]->name(); }
const std::string& ReaderIterator::codecName(unsigned long index) const { return columns_[index]->coder().name(); }
double ReaderIterator::columnMissingValue(unsigned long index) { return columns_[index]->missingValue(); }
const BitfieldDef& ReaderIterator::bitfieldDef(unsigned long index) { return columns_[index]->bitfieldDef(); }

} // namespace odb

