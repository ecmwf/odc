/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File ODADatabase.h
// Baudouin Raoult - ECMWF Oct 04

#ifndef ODADatabase_H
#define ODADatabase_H

namespace eckit { class PathName; }

#include "odblib/SQLDatabase.h"

namespace odb {
namespace sql {

class ODADatabase : public SQLDatabase {
public:
	ODADatabase(const eckit::PathName&,const string&);
	~ODADatabase(); 

private:
// No copy allowed
	ODADatabase(const ODADatabase&);
	ODADatabase& operator=(const ODADatabase&);

// -- Methods
	// None

// -- Overridden methods
	// From SQLDatabase

	virtual void open();
	virtual void close();
	virtual SQLTable* table(const string&);
	virtual SQLTable* openDataHandle(eckit::DataHandle&, DataFormat = ODA);
	virtual SQLTable* openDataStream(istream&, const string& delimiter, DataFormat = CSV); 

// -- Friends
	//friend ostream& operator<<(ostream& s,const ODADatabase& p)
	//	{ p.print(s); return s; }
};

} // namespace sql 
} // namespace odb 

#endif
