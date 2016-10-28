/*
 * (C) Copyright 1996-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// @file   HttpHandle.h
// @author Piotr Kuchta - ECMWF 27 Oct 2016

#ifndef eckit_filesystem_HttpFileHandle_h
#define eckit_filesystem_HttpFileHandle_h

#include "eckit/io/TCPHandle.h"

namespace eckit {

class HttpHandle : public DataHandle {
public:
	HttpHandle(const std::string& url);
	~HttpHandle();
// -- Overridden methods
	// From DataHandle
    virtual Length openForRead();
    virtual void openForWrite(const Length&);
    virtual void openForAppend(const Length&);

	virtual long read(void*,long);
	virtual long write(const void*,long);
	virtual void close();
	virtual void print(std::ostream&) const;

	// From Streamable
	virtual void encode(Stream&) const;

    static std::string parseHost(const std::string&);
    static int parsePort(const std::string&);

private:
	const std::string url_;
    eckit::TCPHandle tcp_;
};

} // namespace eckit

#endif
