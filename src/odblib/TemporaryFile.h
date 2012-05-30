/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File TemporaryFile.h
// Baudouin Raoult - ECMWF Sep 01

#ifndef TemporaryFile_H
#define TemporaryFile_H

#include "eclib/PathName.h"

class TemporaryFile : public PathName {
public:
	TemporaryFile();
	~TemporaryFile(); // Change to virtual if base class
private:
// No copy allowed
	TemporaryFile(const TemporaryFile&);
	TemporaryFile& operator=(const TemporaryFile&);

	//friend ostream& operator<<(ostream& s,const TemporaryFile& p)
	//	{ p.print(s); return s; }
};

#endif
