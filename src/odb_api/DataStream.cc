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
/// \file DataStream.cc
///
/// @author Piotr Kuchta, March 2009

#include "eckit/io/DataHandle.h"
#include "eckit/io/Length.h"
#include "eckit/log/CodeLocation.h"
#include "odb_api/MemoryBlock.h"

namespace odb {

inline void checkRead(int code, const char *msg, const eckit::CodeLocation& loc)
{
    if(code != 0)
    {
        throw eckit::ReadError("DataStream");
    }
}

#define CHECK_READ(a)     ::odb::checkRead(!(a),#a,Here())

template <typename T, typename D>
DataStream<T,D>::DataStream() : f() {}

template <typename T, typename D>
DataStream<T,D>::DataStream(D *dh) : f(dh) {}

template <typename T, typename D>
DataStream<T,D>::DataStream(D &dh) : f(&dh) {}

template <typename T, typename D>
void DataStream<T,D>::close() { f->close(); }

template <typename T, typename D>
long DataStream<T,D>::read(void* p, long l) { return f->read(p, l);
}

template <typename T, typename D>
void DataStream<T,D>::readInt32(int32_t& i)
{
	CHECK_READ(f->read(&i, sizeof(int32_t)) == sizeof(int32_t));
	T::swap(i);
}

template <typename T, typename D>
void DataStream<T,D>::readInt64(int64_t& i)
{
	CHECK_READ(f->read(&i, sizeof(int64_t)) == sizeof(int64_t));
	T::swap(i);
}

template <typename T, typename D>
void DataStream<T,D>::writeInt32(int32_t i)
{
	T::swap(i);
	ASSERT(f->write(&i, sizeof(int32_t)) == sizeof(int32_t));
}

template <typename T, typename D>
void DataStream<T,D>::writeInt64(int64_t i)
{
	T::swap(i);
	ASSERT(f->write(&i, sizeof(int64_t)) == sizeof(int64_t));
}

template <typename T, typename D>
void DataStream<T,D>::readInt16(int16_t& i)
{
	CHECK_READ(f->read(&i, sizeof(int16_t)) == sizeof(int16_t));
	T::swap(i);
}

template <typename T, typename D>
void DataStream<T,D>::writeInt16(int16_t i)
{
	T::swap(i);
	ASSERT(f->write(&i, sizeof(int16_t)) == sizeof(int16_t));
}

template <typename T, typename D>
void DataStream<T,D>::readUInt16(uint16_t& i)
{
	CHECK_READ(f->read(&i, sizeof(uint16_t)) == sizeof(uint16_t));
	T::swap(i);
}

template <typename T, typename D>
void DataStream<T,D>::writeUInt16(uint16_t i)
{
	T::swap(i);
	ASSERT(f->write(&i, sizeof(uint16_t)) == sizeof(uint16_t));
}

template <typename T, typename D>
void DataStream<T,D>::readBuffer(MemoryBlock &buffer)
{
	int32_t size;
	readInt32(size);
	buffer.size(size);
	CHECK_READ(f->read(buffer, size) == size);
}

template <typename T, typename D>
void DataStream<T,D>::writeBuffer(const MemoryBlock &buffer)
{
	writeInt32(buffer.size());
	eckit::Length writtenBytes = f->write(buffer, buffer.size());
	ASSERT(long (writtenBytes) == long (buffer.size()));
}

template <typename T, typename D>
void DataStream<T,D>::readString(std::string &s)
{
	int32_t len;
	readInt32(len);

#ifdef _HPUX_SOURCE
	char buff[8 * 1024 * 1024];
	ASSERT(len < sizeof(buff));
#else
	char buff[len];
#endif
	CHECK_READ(f->read(&buff, len) == len);
	
	std::string r(buff, len);

	s = r;
}

template <typename T, typename D>
void DataStream<T,D>::writeString(const std::string &s)
{
	int32_t len = s.size();
	writeInt32(len);
	ASSERT(f->write(s.c_str(), len) == len);
}

template <typename T, typename D>
void DataStream<T,D>::readChar(char &c)
{
	CHECK_READ(f->read(&c, sizeof(char)) == sizeof(char));
}

template <typename T, typename D>
void DataStream<T,D>::writeChar(char c)
{
	size_t n = f->write(&c, sizeof(char));
	ASSERT(n == sizeof(char));
}

template <typename T, typename D>
void DataStream<T,D>::readUChar(unsigned char &c)
{
	size_t n = f->read(&c, sizeof(unsigned char));
	ASSERT(n == sizeof(unsigned char));
}

template <typename T, typename D>
void DataStream<T,D>::writeUChar(const unsigned char c)
{
	ASSERT(f->write(&c, sizeof(unsigned char)) == sizeof(unsigned char));
}

template <typename T, typename D>
void DataStream<T,D>::readBytes(char *buff, size_t &len)
{
	CHECK_READ(f->read(buff, len) == static_cast<long>(len));
}

template <typename T, typename D>
void DataStream<T,D>::writeBytes(const char *buff, size_t len)
{
	ASSERT(f->write(buff, len) == static_cast<long>(len));
}

template <typename T, typename D>
void DataStream<T,D>::readDouble(double &d)
{
	CHECK_READ(f->read(&d, sizeof(double)) == sizeof(double));
	T::swap(d);
}

template <typename T, typename D>
void DataStream<T,D>::writeDouble(double d)
{
	T::swap(d);
	ASSERT(f->write(&d, sizeof(double)) == sizeof(double));
}

template <typename T, typename D>
void DataStream<T,D>::readFloat(float &d)
{
	CHECK_READ(f->read(&d, sizeof(float)) == sizeof(float));
	T::swap(d);
}

template <typename T, typename D>
void DataStream<T,D>::writeFloat(float d)
{
	T::swap(d);
	ASSERT(f->write(&d, sizeof(float)) == sizeof(float));
}

template <typename T, typename D>
void DataStream<T,D>::readProperties(Properties &p)
{
	int32_t size = 0;
	readInt32(size);

	for (int i = 0; i < size; i++)
	{
		std::string key, value;
		readString(key);
		readString(value);
		p[key] = value;
	}
}

template <typename T, typename D>
void DataStream<T,D>::writeProperties(const Properties &p)
{
	int32_t size = p.size();
	writeInt32(size);

	for(Properties::const_iterator i = p.begin(); i != p.end(); ++i)
	{
		writeString(i->first);
		writeString(i->second);
	}
}

template <typename T, typename D>
void DataStream<T,D>::readFlags(Flags &v)
{
	int32_t size = 0;
	readInt32(size);

	v.clear();
	v.resize(size);

	for (int32_t i = 0; i < size; ++i)
		readDouble(v[i]);
}

template <typename T, typename D>
void DataStream<T,D>::writeFlags(const Flags &v)
{
	int32_t size = v.size();
	writeInt32(size);

	for (int32_t i = 0; i < size; ++i)
		writeDouble(v[i]);
}

template <typename T, typename D>
void DataStream<T,D>::readBitfieldDef(eckit::sql::BitfieldDef &v)
{
    eckit::sql::FieldNames& names = v.first;
    eckit::sql::Sizes& sizes = v.second;
	
	names.clear();
	sizes.clear();

	int32_t namesSize = 0;
	readInt32(namesSize);

	for (int32_t i = 0; i < namesSize; i++)
	{
		std::string s;
		readString(s);
		names.push_back(s);
	}

	int32_t sizesSize = 0;
	readInt32(sizesSize);

	ASSERT(namesSize == sizesSize);

	for (int32_t i = 0; i < sizesSize; i++)
	{
		int32_t n;
		readInt32(n);
		sizes.push_back(n);
	}
}

template <typename T, typename D>
void DataStream<T,D>::writeBitfieldDef(const eckit::sql::BitfieldDef &v)
{
    const eckit::sql::FieldNames& names = v.first;
    const eckit::sql::Sizes& sizes = v.second;

	writeInt32(names.size());
	for (size_t i = 0; i < names.size(); i++)
		writeString(names[i]);

	writeInt32(sizes.size());
	for (size_t i = 0; i < sizes.size(); i++)
		writeInt32(sizes[i]);
}

#undef CHECK_READ

} // namespace odb

