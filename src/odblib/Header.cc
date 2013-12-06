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
/// \file Header.cc
///
/// @author Piotr Kuchta, Feb 2009

#include "odblib/DataStream.h"
#include "odblib/InMemoryDataHandle.h"
#include "odblib/MD5.h"
#include "odblib/MetaData.h"
#include "odblib/UnsafeInMemoryDataHandle.h"

namespace odb {

template <typename OWNER>
Header<OWNER>::Header(OWNER& owner)
: owner_(owner),
  dataSize_(0),
  rowsNumber_(0),
  byteOrder_(BYTE_ORDER_INDICATOR)
{} 

template <typename OWNER>
Header<OWNER>::~Header ()
{}

template <typename OWNER>
void Header<OWNER>::load()
{
	DataStream<SameByteOrder> f(owner_.f);

	uint16_t c;
	f.readUInt16(c);
	ASSERT(c == ODA_MAGIC_NUMBER);

	unsigned char cc;
	f.readUChar(cc); ASSERT(cc == 'O');
	f.readUChar(cc); ASSERT(cc == 'D');
	f.readUChar(cc); ASSERT(cc == 'A');

	loadAfterMagic();
}

template <typename OWNER>
void Header<OWNER>::loadAfterMagic()
{
	DataStream<SameByteOrder> f(owner_.f);

	f.readInt32(byteOrder_);

	if (byteOrder_ != BYTE_ORDER_INDICATOR)
	{
		DataStream<OtherByteOrder> ds(owner_.f);
		load(ds);
	}
	else
	{
		DataStream<SameByteOrder> ds(owner_.f);
		load(ds);
	}
}

template <typename OWNER>
template <typename DATASTREAM>
void Header<OWNER>::load(DATASTREAM &ff)
{
	int32_t formatVersionMajor;
	ff.readInt32(formatVersionMajor);
	ASSERT("File format version not supported" && formatVersionMajor <= FORMAT_VERSION_NUMBER_MAJOR);

	int32_t formatVersionMinor;
	ff.readInt32(formatVersionMinor);
	ASSERT("File format version not supported" && formatVersionMinor <= FORMAT_VERSION_NUMBER_MINOR && formatVersionMinor > 3);

	std::string headerDigest; 
	ff.readString(headerDigest);

	MemoryBlock buffer(0);
	ff.readBuffer(buffer);

	MD5 md5;
	md5.add(buffer, buffer.size());
	std::string actualHeaderDigest = md5.digest();

	if (! (headerDigest == actualHeaderDigest))
	{
		//eckit::Log::debug() << "headerDigest(" << headerDigest.size() << "):       '" << headerDigest << "'" << std::endl;
		//eckit::Log::debug() << "actualHeaderDigest(" << actualHeaderDigest.size() << "): '" << actualHeaderDigest << "'" << std::endl;
		ASSERT(headerDigest == actualHeaderDigest);
	}
	
	PrettyFastInMemoryDataHandle memoryHandle(buffer);
	DataStream<typename DATASTREAM::ByteOrderType, PrettyFastInMemoryDataHandle> f(memoryHandle);

	// 0 means we don't know offset of next header.
	int64_t nextFrameOffset;
	f.readInt64(nextFrameOffset);
	dataSize_ = nextFrameOffset;
	owner_.columns().dataSize(dataSize_);

	// Reserved, not used yet.
	int64_t prevFrameOffset;
	f.readInt64(prevFrameOffset);
	ASSERT(prevFrameOffset == 0);

	// TODO: increase file format version

	int64_t numberOfRows;
	f.readInt64(numberOfRows);
	rowsNumber_ = numberOfRows;
	owner_.columns().rowsNumber(rowsNumber_);

	eckit::Log::debug() << "Header<OWNER>::load: numberOfRows = " << numberOfRows << std::endl;

	// Flags -> ODAFlags
	Flags flags;
	f.readFlags(flags);

	f.readProperties(owner_.properties_);

	owner_.columns().load(f);
}

template <typename BYTEORDER, typename DATAHANDLE>
void serializeHeader(DATAHANDLE &dh, size_t dataSize, size_t rowsNumber, const Properties& properties, const MetaData& columns)
{
	DataStream<BYTEORDER, DATAHANDLE> ff(dh);

	// Header.
	uint16_t c = ODA_MAGIC_NUMBER;
	ff.writeUInt16(c);

	c = 'O'; ff.writeChar(c); 
	c = 'D'; ff.writeChar(c); 
	c = 'A'; ff.writeChar(c);

	int32_t byteOrder = BYTE_ORDER_INDICATOR;
	ff.writeInt32(byteOrder);

	int32_t versionMajor = FORMAT_VERSION_NUMBER_MAJOR;
	int32_t versionMinor = FORMAT_VERSION_NUMBER_MINOR;
	ff.writeInt32(versionMajor);
	ff.writeInt32(versionMinor);

	InMemoryDataHandle memoryHandle;
	DataStream<BYTEORDER> f(memoryHandle);

	// Reserved.	
	int64_t nextFrameOffset = dataSize;
	f.writeInt64(nextFrameOffset);

	// Reserved.	
	int64_t prevFrameOffset = 0;
	f.writeInt64(prevFrameOffset);

	int64_t numberOfRows = rowsNumber;
	f.writeInt64(numberOfRows);

	Flags flags(10, 0);
	f.writeFlags(flags);

	f.writeProperties(properties);

	columns.save(f);

	eckit::Length len = memoryHandle.openForRead();

	MemoryBlock buffer(len);
	eckit::Length readBytes = memoryHandle.read(buffer, len);
	ASSERT(len == readBytes);

	MD5 md5;
	md5.add(buffer, len);
	std::string headerDigest = md5.digest();
	ff.writeString(headerDigest);

	ff.writeBuffer(buffer);
}

} //namespace odb 

