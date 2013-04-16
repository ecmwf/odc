/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File SQLType.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef SQLType_H
#define SQLType_H

#include "eckit/machine.h"

namespace odb {
namespace sql {

class SQLOutput;

namespace type {

class SQLType {
public:

	enum {
		realType    = 0,
		integerType = 1,
		stringType  = 2,
		bitmapType  = 3,
		blobType    = 4,
		doubleType  = 5
	};

	SQLType(const string&);

	/// Constructor used when defining a bitfield.
	SQLType(const string&, const string&);

	virtual ~SQLType(); // Change to virtual if base class

	const string& name() const { return name_; }

	virtual size_t size() const = 0;
	virtual void output(SQLOutput&, double, bool) const = 0;
	virtual const SQLType* subType(const string&) const;

	virtual int getKind() const = 0;

	// Formating functions (used by SQLSimpleOutput)
	virtual size_t width() const;
	typedef std::ios_base& (*manipulator)(std::ios_base&);
	virtual manipulator format() const;

	static const SQLType& lookup(const string&);

	static void createAlias(const string&, const string&);

protected:
	virtual void print(ostream&) const; // Change to virtual if base class	
	static bool exists(const string&);

private:
// No copy allowed
	SQLType(const SQLType&);
	SQLType& operator=(const SQLType&);

	string name_;

	static void registerStaticTypes();

	friend ostream& operator<<(ostream& s,const SQLType& p)
		{ p.print(s); return s; }
};

class DynamicallyCreatedTypesDestroyer {
public:
	static SQLType* registerType(SQLType*);
	~DynamicallyCreatedTypesDestroyer ();
};

} // namespace type 
} // namespace sql
} // namespace odb

#endif
