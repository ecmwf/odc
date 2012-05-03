/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File SQLIteratorSession.h
// Piotr Kuchta - ECMWF Feb 09

#ifndef SQLIteratorSession_H
#define SQLIteratorSession_H

#include "odblib/SQLSession.h"

namespace odb {

class ReaderIterator;
class SelectIterator;
class SQLOutput;
class SQLStatement;

namespace sql {

template <typename T = SelectIterator>
class SQLIteratorSession : public SQLSession {
public:
	SQLIteratorSession(T &);
	~SQLIteratorSession(); // Change to virtual if base class

	SQLStatement* statement();

private:
// No copy allowed
	SQLIteratorSession(const SQLIteratorSession&);
	SQLIteratorSession& operator=(const SQLIteratorSession&);

	SQLStatement* statement_;
	T& iterator_;

// -- Overridden methods

	void       statement(SQLStatement*);
	SQLOutput* defaultOutput();

// -- Friends

	//friend ostream& operator<<(ostream& s,const SQLIteratorSession& p)
	//	{ p.print(s); return s; }

};

} // namespace sql
} // namespace odb

#include "odblib/SQLIteratorSession.cc"

#endif
