/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eclib/machine.h"
#include "eclib/Bytes.h"
#include "eclib/FileHandle.h"
#include "eclib/Timer.h"

#include "odblib/InMemoryDataHandle.h"

namespace odb {

InMemoryDataHandle::InMemoryDataHandle()
: buf_(), readIterator_(buf_.begin())
{}

InMemoryDataHandle::InMemoryDataHandle(const MemoryBlock& buffer)
: buf_((const unsigned char *) buffer, (const unsigned char *) buffer + buffer.size()),
  readIterator_(buf_.begin())
{
}

void InMemoryDataHandle::buffer(const MemoryBlock& buffer)
{
	buf_ = vector<unsigned char>((const unsigned char *) buffer, (const unsigned char *) buffer + buffer.size());
	readIterator_ = buf_.begin();
}

InMemoryDataHandle::~InMemoryDataHandle()
{}

} // namespace odb

