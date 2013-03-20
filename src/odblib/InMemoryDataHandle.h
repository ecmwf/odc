/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file InMemoryDataHandle.h
/// Piotr Kuchta - ECMWF April 2009

#ifndef InMemoryDataHandle_H
#define InMemoryDataHandle_H

#include "eclib/DataHandle.h"
#include "eclib/Length.h"
#include "eclib/Offset.h"
#include "eclib/filesystem/PathName.h"
#include "eclib/TransferWatcher.h"

#include "odblib/MemoryBlock.h"

namespace odb {

class InMemoryDataHandle : public eckit::DataHandle {
public:
    InMemoryDataHandle();
    InMemoryDataHandle(const MemoryBlock&);

    virtual ~InMemoryDataHandle();

	void buffer(const MemoryBlock&);


	bool hasSomeData() { return readIterator_ != buf_.end(); }

	/// Return estimated length.
    virtual eckit::Length openForRead()
	{
		readIterator_ = buf_.begin();
		return buf_.size();
	}

	// Receive estimated length.
    void openForWrite(const eckit::Length&) { buf_.clear(); ASSERT(buf_.size() == 0); }

	// Receive estimated length
    void openForAppend(const eckit::Length&) {}

    long read(void* p, long n)
	{
		char *dst = reinterpret_cast<char *>(p);
		long i = 0;
		for ( ; i < n && readIterator_ != buf_.end(); ++i, ++readIterator_)
			dst[i] = *readIterator_;
		return i;
	}

    long write(const void* pd, long n)
	{
		const unsigned char *p = reinterpret_cast<const unsigned char*>(pd);
		buf_.insert(buf_.end(), p, p + n);
		return n;
	}

    void close() {}

    void rewind()                {}
	eckit::Length estimate()            { return buf_.size(); }
	eckit::Offset position()            { return buf_.size(); }

	void print(ostream& s) const { /*TODO*/ }
private:

	vector<unsigned char> buf_;
	vector<unsigned char>::iterator readIterator_;

	friend ostream& operator<<(ostream& s, const InMemoryDataHandle& handle) 
		{ handle.print(s); return s;}
};

} // namespace odb 

#endif
