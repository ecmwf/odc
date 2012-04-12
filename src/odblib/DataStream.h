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
/// \file DataStream.h
///
/// @author Piotr Kuchta, Feb 2009

#ifndef DataStream_H
#define DataStream_H

#include "eclib/machine.h"

#include "odblib/SQLBitfield.h"

class DataHandle;
class MemoryBlock;

namespace odb {

typedef std::map<std::string, std::string> Properties;
typedef std::vector<double> Flags;

class Column;
class Reader;
class HashTable;
class SQLIteratorSession;

class SameByteOrder {
public:
	template<typename T> static void swap(T &) {}
};

class OtherByteOrder {
public:
	template<typename T> static void swap(T &o)
	{
		char *p = reinterpret_cast<char *>(&o);
		size_t size = sizeof(T);
		for (size_t i=0; i < size / 2; ++i)
			std::swap(p[i], p[size - i - 1]);
	}
};

template <typename T = SameByteOrder, typename D = DataHandle>
class DataStream {
public:
	typedef T ByteOrderType;
	typedef D DataHandleType;

	DataStream (D &);
	DataStream (D *);
	DataStream ();

	void close();

	long read(void* p, long l); 
	D *dataHandle() { return f; }
	void dataHandle(D *dh) { f = dh; }

	void readInt64(int64_t &);
	void writeInt64(int64_t);

	void readInt32(int32_t &);
	void writeInt32(int32_t);

	void readInt16(int16_t &);
	void writeInt16(int16_t);

	void readUInt16(uint16_t &);
	void writeUInt16(uint16_t);

	void readString(string &);
	void writeString(const string &);

	void readBuffer(MemoryBlock &);
	void writeBuffer(const MemoryBlock &);

	void readChar(char &);
	void writeChar(char);

	void readUChar(unsigned char &);
	void writeUChar(unsigned char);

	void readBytes(char *, size_t &);
	void writeBytes(const char *, size_t);

	void readDouble(double &);
	void writeDouble(double);

	void readFloat(float &);
	void writeFloat(float);

	void readProperties(Properties &);
	void writeProperties(const Properties &);

	void readFlags(Flags &);
	void writeFlags(const Flags &);

	void readBitfieldDef(BitfieldDef &);
	void writeBitfieldDef(const BitfieldDef &);
private:
	D * f;

// No copy allowed.
    DataStream(const DataStream&);
    DataStream& operator=(const DataStream&);
};

} // namespace odb {

#include "DataStream.cc"

#endif
