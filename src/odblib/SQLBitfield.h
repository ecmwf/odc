/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File SQLBitfield.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef SQLBitfield_H
#define SQLBitfield_H

#include "SQLType.h"

namespace odb {

typedef vector<string>           FieldNames;
typedef vector<int32_t>          Sizes;
typedef pair<FieldNames, Sizes>  BitfieldDef;
typedef map<string, BitfieldDef> BitfieldDefs;

namespace sql {
namespace type {

class SQLBitfield : public SQLType {
public:
	SQLBitfield(const string&, const FieldNames&, const Sizes&, const string&);
	~SQLBitfield(); // Change to virtual if base class

	unsigned long mask(const string& n) const;
	unsigned long shift(const string& n) const;

	const BitfieldDef& bitfieldDef() const { return bitfieldDef_; }
	const FieldNames& fields() const { return bitfieldDef_.first; }
	const Sizes& sizes() const { return bitfieldDef_.second; }

	static string make(const string&, const FieldNames&, const Sizes&, const char *ddlName = NULL);

private:
	SQLBitfield(const SQLBitfield&);
	SQLBitfield& operator=(const SQLBitfield&);


	//FieldNames	fields_;
	//Sizes		sizes_;
	BitfieldDef bitfieldDef_;
	map<string, unsigned long> mask_;
	map<string, unsigned long> shift_;


	virtual size_t size() const;
	virtual void output(SQLOutput& s, double, bool) const;
	virtual const SQLType* subType(const string&) const;
	virtual int getKind() const { return bitmapType; }

	//friend ostream& operator<<(ostream& s,const SQLBitfield& p)
	//	{ p.print(s); return s; }
};

} // namespace type 
} // namespace sql 
} // namespace odb 

#endif
