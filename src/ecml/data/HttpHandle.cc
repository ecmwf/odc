/*
 * (C) Copyright 1996-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "ecml/data/HttpHandle.h"

namespace eckit {

void HttpHandle::print(std::ostream& s) const
{
	s << "HttpHandle[" << url_ << ']';
}

void HttpHandle::encode(Stream& s) const
{
	NOTIMP;
}

HttpHandle::HttpHandle(const std::string &url)
: url_(url)
{
}

HttpHandle::~HttpHandle()
{
}

Length HttpHandle::openForRead()
{
	return 0;
}

void HttpHandle::openForWrite(const Length&)
{
}

void HttpHandle::openForAppend(const Length&)
{
}

long HttpHandle::read(void* buffer, long length)
{
    long n(0);
    // TODO:
	return n;
}

long HttpHandle::write(const void* buffer, long length)
{
    long n(0);
    // TODO:
	return n;
}

void HttpHandle::close()
{
}

} // namespace ecml

