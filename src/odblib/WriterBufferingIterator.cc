/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

///
/// \file WriterBufferingIterator.cc
///
/// @author Piotr Kuchta, Feb 2009

#include <arpa/inet.h>

#include "eclib/Resource.h"

#include "odblib/oda.h"

#include "odblib/Codec.h"
#include "odblib/CodecOptimizer.h"
#include "odblib/DataStream.h"
#include "odblib/InMemoryDataHandle.h"
#include "odblib/MemoryBlock.h"
#include "odblib/ODBAPISettings.h"




namespace odb {

WriterBufferingIterator::WriterBufferingIterator(Owner &owner, DataHandle *dh, bool openDataHandle)
: owner_(owner),
  columns_(0),
  lastValues_(0),
  nextRow_(0),
  nrows_(0),
  f(dh),
  buffer_(0),
  refCount_(0),
  properties_(),
  blockBuffer_(),
  rowsBuffer_(),
  p_(0),
  memoryDataHandle_(0),
  columnsBuffer_(0),
  rowsBufferSize_(owner.rowsBufferSize()),
  setvBuffer_(0),
  maxAnticipatedHeaderSize_(ODBAPISettings::instance().headerBufferSize() )
{
	//Log::debug() << "WriterBufferingIterator::WriterBufferingIterator: @" << this << endl;

	if (openDataHandle)	
		open();
//FIXME: Move the code inside FileHandle
#if 0
	FileHandle* fh = dynamic_cast<FileHandle*>(dh);
	if (fh)
	{
		FILE* cFile = fh->file();
		long size = ODBAPISettings::instance().setvbufferSize();

		//Log::debug(Here()) << "WriterBufferingIterator::WriterBufferingIterator: setvbuf = " << size << endl;

		setvBuffer_.size(size);
		::setvbuf(cFile, setvBuffer_, _IOFBF, size);
	}
#endif
}

WriterBufferingIterator::~WriterBufferingIterator()
{
	close();
	delete [] lastValues_;
	delete [] nextRow_;
	delete f;
}


unsigned long WriterBufferingIterator::gatherStats(const double* values, unsigned long count)
{
	ASSERT(count == columns().size());

	for(size_t i = 0; i < count; i++)
	{
		codec::Codec& codec = columns_[i]->coder();
		codec.gatherStats(values[i]);
	}

	return 0;
} 

template <typename DATASTREAM>
int WriterBufferingIterator::setOptimalCodecs()
{
	if (columnsBuffer_.size() == 0)
	{
		columnsBuffer_ = columns();
		columnsBuffer_.resetStats();
	}

	//return codec::CodecOptimizer::setOptimalCodecs<DATASTREAM>(columns());
	return codec::CodecOptimizer::setOptimalCodecs<DATASTREAM>(columns());
}

void WriterBufferingIterator::allocBuffers()
{
	delete [] lastValues_;
	delete [] nextRow_;
	int32_t colSize = columns().size();
	double* last = new double [colSize];
	lastValues_ = last;
	nextRow_ = new double [colSize];
	ASSERT(last);

	for (int i = 0; i < colSize; i++)
		nextRow_[i] = last[i] = columns_[i]->missingValue();

	nrows_ = 0;

	buffer_.size(colSize * sizeof(double));
}

void WriterBufferingIterator::allocRowsBuffer()
{
	size_t nCols = columns().size();
	blockBuffer_.size(maxAnticipatedHeaderSize_ + rowsBufferSize_ * nCols * sizeof(double));
	rowsBuffer_.share(static_cast<unsigned char*>(blockBuffer_) + maxAnticipatedHeaderSize_, rowsBufferSize_ * nCols);

	memoryDataHandle_.buffer(rowsBuffer_.cast<unsigned char>());
	p_ = rowsBuffer_;
}

void WriterBufferingIterator::writeHeader()
{
	allocBuffers();
	for (size_t i = 0; i < columns_.size(); ++i)
		columns_[i]->coder().resetStats();
}

bool WriterBufferingIterator::next() { return writeRow(nextRow_, columns().size()) == 0; }

double* WriterBufferingIterator::data() { return nextRow_; }
//double& WriterBufferingIterator::data(int i) { ASSERT(i >= 0 && i < columns().size()); return nextRow_[i]; }

int WriterBufferingIterator::writeRow(const double* data, unsigned long nCols)
{
	ASSERT(nCols == columns().size());

	if (rowsBuffer_ == 0)
		allocRowsBuffer();

	gatherStats(data, nCols);

	copy(data, data + nCols, p_);
	p_ += nCols;

	ASSERT(p_ <= rowsBuffer_ + rowsBuffer_.size());
	if (p_ == rowsBuffer_ + rowsBuffer_.size())
		flush();

	return 0;
}

int WriterBufferingIterator::doWriteRow(const double* values, unsigned long count)
{
	if (lastValues_ == 0)
		allocBuffers();

	uint16_t k = 0;
	while(k < count && values[k] == lastValues_[k])
		++k;

	unsigned char *p = buffer_;
	uint16_t nk = htons(k);
	unsigned char *pk = reinterpret_cast<unsigned char *>(&nk);
	*p++ =  *pk++;
	*p++ =  *pk;

	for ( ; k < count; k++) 
	{
		Column *col = columns_[k];
		p = col->coder().encode(p, values[k]);
		lastValues_[k] = values[k];
	}

	size_t len = p - buffer_;

	DataStream<SameByteOrder, FastInMemoryDataHandle> f(memoryDataHandle_);
	f.writeBytes(buffer_.cast<char>(), len);

	nrows_++;

	return 0;
} 

int WriterBufferingIterator::open()
{
	//Log::debug() << "WriterBufferingIterator::open@" << this << ": Opening data handle " << f << endl;

	ASSERT(f);

	Length estimatedLen = MEGA(20);
	f->openForWrite(estimatedLen);

	return 0;
}


int WriterBufferingIterator::setColumn(size_t index, std::string name, ColumnType type)
{
	//Log::debug() << "WriterBufferingIterator::setColumn: " << endl;

	ASSERT(index < columns().size());
	Column* col = columns_[index];
	ASSERT(col);

	col->name(name); 
	col->type<DataStream<SameByteOrder, FastInMemoryDataHandle> >(type, false);
	//col->hasMissing(hasMissing);
	//col->missingValue(missingValue);
	return 0;
}

int WriterBufferingIterator::setBitfieldColumn(size_t index, std::string name, ColumnType type, BitfieldDef b)
{
	//Log::debug() << "WriterBufferingIterator::setBitfieldColumn: " << endl;

	ASSERT(index < columns().size());
	Column* col = columns_[index];
	ASSERT(col);

	col->name(name); 
	col->type<DataStream<SameByteOrder, FastInMemoryDataHandle> >(type, false);
	//col->hasMissing(hasMissing);
	//col->missingValue(missingValue);
    col->bitfieldDef(b);
	col->missingValue(0);
	return 0;
}

void WriterBufferingIterator::missingValue(size_t i, double missingValue)
{
	ASSERT(i < columns().size());
	Column* col = columns_[i];
	ASSERT(col);

	col->missingValue(missingValue);
}

void WriterBufferingIterator::flush()
{
	if (p_ == rowsBuffer_)
		return;
	
	setOptimalCodecs<DataStream<SameByteOrder, FastInMemoryDataHandle> >();

	unsigned long rowsWritten = 0;
	const size_t nCols = columns().size();
	double *pr = rowsBuffer_;
	for (; pr < p_; pr += nCols, ++rowsWritten)
		doWriteRow(pr, nCols);

	InMemoryDataHandle bufferForHeader;
	doWriteHeader(bufferForHeader, memoryDataHandle_.position(), rowsWritten);

	//Log::debug(Here()) << "WriterBufferingIterator::flush: header size: " << bufferForHeader.position() << endl;

	MemoryBlock buff(bufferForHeader.position());
	bufferForHeader.openForRead();
	bufferForHeader.read(buff, buff.size());

	if (buff.size() < maxAnticipatedHeaderSize_)
	{
		//Log::debug(Here()) << "WriterBufferingIterator::flush: writing header and data in one go. "
		//	"Block size: " << buff.size() + memoryDataHandle_.position() << endl;
		copy(static_cast<unsigned char*>(buff), static_cast<unsigned char*>(buff) + buff.size(),
			memoryDataHandle_.buffer() - buff.size());
		this->f->write(memoryDataHandle_.buffer() - buff.size(),
			static_cast<size_t>(memoryDataHandle_.position()) + buff.size()); // Write encodedd data
	}
	else
	{
		Log::debug(Here()) << "WriterBufferingIterator::flush: writing header and data separately." << endl;

		this->f->write(buff, buff.size()); // Write header
		this->f->write(memoryDataHandle_.buffer(), memoryDataHandle_.position()); // Write encodedd data
	}

	//Log::debug(Here()) << "WriterBufferingIterator::flush: flushed " << rowsWritten << " rows." << endl;

	memoryDataHandle_.buffer(rowsBuffer_.cast<unsigned char>());

	p_ = rowsBuffer_;

	columns() = columnsBuffer_;
	columns().resetStats();
}


template <typename T>
void WriterBufferingIterator::doWriteHeader(T& dataHandle, size_t dataSize, size_t rowsNumber)
{
	//Log::debug() << "WriterBufferingIterator::doWriteHeader: dataSize=" << dataSize << ", rowsNumber=" << rowsNumber << endl;

	allocBuffers();

	//  FIXME: copy properties from input - props are not initialised now
	serializeHeader<SameByteOrder,T>(dataHandle, dataSize, rowsNumber, properties_, columns()); 
}

int WriterBufferingIterator::close()
{
	flush();

	if (f)
	{
		f->close();
		f = 0;
	}
	return 0;
}

} // namespace odb 

