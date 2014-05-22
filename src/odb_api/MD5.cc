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
#include "odb_api/MD5.h"

MD5::MD5() { md5_init(&state_); }

MD5::~MD5() {}

void MD5::add(const void* buffer, long length)
{
	ASSERT(length > 0);

	md5_add(&state_, static_cast<const unsigned char*>(buffer), length);
}

std::string MD5::digest()
{
	if(digest_.length() == 0)
	{
		char digest[33];
		md5_end(&state_, digest);
		digest[32] = 0;
		digest_ = digest;
	}
	return digest_;
}
