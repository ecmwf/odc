/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef MemoryBlock_H
#define MemoryBlock_H

#include "eclib/machine.h"
#include "eclib/Exceptions.h"

// A simple class to implement buffers

class MemoryBlock {
public:
	MemoryBlock(size_t size);
	MemoryBlock(const string& s);
	MemoryBlock(const char*,size_t size);

	~MemoryBlock();

// -- Operators

	operator char*()                 { return (char*)buffer_; }
	operator const char*() const     { return (char*)buffer_; }

	operator unsigned char*()                 { return (unsigned char*)buffer_; }
	operator const unsigned char*() const     { return (unsigned char*)buffer_; }

	operator void*()                 { return buffer_; }
	operator const void*() const     { return buffer_; }

// -- Methods

	size_t size() const		 { return size_; }
	void size(size_t);

private:
// No copy allowed
	MemoryBlock(const MemoryBlock&);
	MemoryBlock& operator=(const MemoryBlock&);

	void create();
	void destroy();

	unsigned char *  buffer_;
	size_t size_;

};

#endif
