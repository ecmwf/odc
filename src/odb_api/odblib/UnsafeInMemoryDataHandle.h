/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file UnsafeInMemoryDataHandle.h
/// Piotr Kuchta - ECMWF August 2009

#ifndef UnsafeInMemoryDataHandle_H
#define UnsafeInMemoryDataHandle_H

#include "eckit/io/DataHandle.h"
#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"

namespace odb {

class NonVirtual {};

template <typename T = eckit::DataHandle>
class UnsafeInMemoryDataHandle : public T {
public:
    UnsafeInMemoryDataHandle(unsigned char *p) : buf_(p), p_(p) {}

    virtual ~UnsafeInMemoryDataHandle() {}

	void print(std::ostream& s) const { /*TODO*/ }

	void buffer(unsigned char *p) { buf_ = p_ = p; }
	unsigned char* buffer() { return buf_; }

	/// Return estimated length.
    virtual eckit::Length openForRead()
	{
		size_t size = p_ - buf_;
		p_ = buf_;
		return size;
	}

	// Receive estimated length.
    void openForWrite(const eckit::Length&) { p_ = buf_; }

	// Receive estimated length
    void openForAppend(const eckit::Length&) { NOTIMP; }

    long read(void* p, long n)
	{
		unsigned char *dst = reinterpret_cast<unsigned char *>(p);
		unsigned char *end = dst + n;
		for ( ; dst != end; )
			*dst++ = *p_++;
		return n;
	}

    long write(const void* pd, long n)
	{
		const unsigned char *p = reinterpret_cast<const unsigned char*>(pd);
		unsigned char *end = p_ + n;
		for ( ; p_ != end; )
			*p_++ = *p++;
		return n;
	}

    void close() {}

    void rewind()                { /*?*/ p_ = buf_; }
	eckit::Length estimate()            { /*?*/ return p_ - buf_; }
	eckit::Offset position()            { return p_ - buf_; }

private:
    UnsafeInMemoryDataHandle();

// No copy allowed

    UnsafeInMemoryDataHandle(const UnsafeInMemoryDataHandle&);
    UnsafeInMemoryDataHandle& operator=(const UnsafeInMemoryDataHandle&);

	unsigned char* buf_;
	unsigned char* p_;

	friend std::ostream& operator<<(std::ostream& s, const UnsafeInMemoryDataHandle& handle) 
		{ handle.print(s); return s;}
};

typedef UnsafeInMemoryDataHandle<NonVirtual> FastInMemoryDataHandle;
typedef UnsafeInMemoryDataHandle<eckit::DataHandle> PrettyFastInMemoryDataHandle;

} // namespace odb

#endif
