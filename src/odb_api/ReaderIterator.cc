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

	if ( (bytesRead = memDataHandle_.read(&c, 2)) == 0)
	{
        if ( (bytesRead = f_->read(&c, 2)) <= 0)
        {
            owner_.noMoreData();
			return ! (noMore_ = true);
        }
		ASSERT(bytesRead == 2);

		if (c == ODA_MAGIC_NUMBER) 
		{
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
			if (! readBuffer(dataSize))
            {
                owner_.noMoreData();
				return ! (noMore_ = true);
            }

            if( (bytesRead = memDataHandle_.read(&c, 2)) == 0)
            {
                owner_.noMoreData();
				return ! (noMore_ = true);
            }
			ASSERT(bytesRead == 2);

			newDataset_ = true;
		}
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

