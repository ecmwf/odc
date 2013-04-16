/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file TReadOnlyMemoryDataHandle.h
/// Piotr Kuchta - ECMWF September 2010

#ifndef TReadOnlyMemoryDataHandle_H
#define TReadOnlyMemoryDataHandle_H

#include "eclib/DataHandle.h"
#include "eckit/io/Length.h"
#include "eclib/Offset.h"
#include "eclib/filesystem/PathName.h"
#include "eclib/TransferWatcher.h"

#include "odblib/MemoryBlock.h"

namespace odb {

class NonVirtualBase {};

template <typename T = eckit::DataHandle>
class TReadOnlyMemoryDataHandle : public T {
public:

	friend ostream& operator<<(ostream& s, const TReadOnlyMemoryDataHandle& handle) 
		{ handle.print(s); return s;}

    TReadOnlyMemoryDataHandle() : buffer_(0), it_(0), end_(0), bufferSize_(0) {}

    ~TReadOnlyMemoryDataHandle() { delete [] buffer_; }

// -- Methods

	unsigned char * buffer() { return buffer_; }

	void size() const { return bufferSize_; }

	void size(size_t n)
	{
		if (n > bufferSize_)
		{
			delete [] buffer_;
			buffer_ = new unsigned char [n];
			ASSERT(buffer_ && "Cannot allocate buffer_");
			bufferSize_ = n;
		}
		it_ = buffer_;
		end_ = buffer_ + n;
	}

	void print(ostream& s) const { /*TODO*/ }

	bool hasSomeData() { return it_ != end_; }

	/// Return estimated length.
    virtual eckit::Length openForRead()
	{
		it_ = buffer_;
		return end_ - it_;
	}

	// Receive estimated length.
    void openForWrite(const eckit::Length&) { NOTIMP; }

	// Receive estimated length
    void openForAppend(const eckit::Length&) {}

    long read(void* p, long n)
	{
		char *dst = reinterpret_cast<char *>(p);
		long i = 0;
		for ( ; i < n && it_ != end_; ++i, ++it_)
			dst[i] = *it_;
		return i;
	}

    long write(const void* pd, long n) { NOTIMP; }

    void close() {}

    void rewind()                { it_ = buffer_; }
	eckit::Length estimate()            { return end_ - it_; }
	eckit::Offset position()            { return it_ - buffer_; }

private:
// No copy allowed
    TReadOnlyMemoryDataHandle(const TReadOnlyMemoryDataHandle&);
    TReadOnlyMemoryDataHandle& operator=(const TReadOnlyMemoryDataHandle&);

	unsigned char *buffer_;
	unsigned char *it_;
	unsigned char *end_;
	size_t bufferSize_;
};

typedef TReadOnlyMemoryDataHandle<eckit::DataHandle> ReadOnlyMemoryDataHandle;

} // namespace odb 

#endif
