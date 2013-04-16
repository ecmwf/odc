/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File MD5.h
// Baudouin Raoult - ECMWF Dec 04

#ifndef MD5_H
#define MD5_H

extern "C" {
#include "md5_hash.h"
} // extern "C"

#include "eckit/machine.h"

// Forward declarations

class MD5 {
public:
	MD5();
	~MD5(); // Change to virtual if base class

	void add(const void*, long);
	string digest();

private:
// No copy allowed
	MD5(const MD5&);
	MD5& operator=(const MD5&);

	md5_state state_;
	string digest_;
};

#endif
