/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/exception/Exceptions.h"
#include "odblib/MemoryBlock.h"

MemoryBlock::MemoryBlock(size_t size):
	buffer_(0),
	size_(size)
{
	create();
}

MemoryBlock::MemoryBlock(const char* p,size_t size):
	buffer_(0),
	size_(size)
{
	create();
	::memcpy(buffer_,p,size);
}

MemoryBlock::MemoryBlock(const string& s):
	buffer_(0),
	size_(s.length()+1)
{
	create();
	::strcpy((char*)buffer_,s.c_str());
}

void MemoryBlock::size(size_t newSize)
{
	destroy();
	size_ = newSize;
	create();
}

MemoryBlock::~MemoryBlock()
{ 
	destroy();
}

void MemoryBlock::create()
{
	buffer_ = new unsigned char[size_];
}

void MemoryBlock::destroy()
{
	delete [] buffer_;
}
