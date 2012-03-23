/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file SQLIteratorOutput.h
/// Piotr Kuchta - ECMWF Feb 09

#ifndef SQLIteratorOutput_H
#define SQLIteratorOutput_H

#include "odblib/SQLOutput.h"

namespace odb {

class SelectIterator;

namespace sql {

class ReaderIterator;

template <typename T = odb::SelectIterator>
class SQLIteratorOutput : public SQLOutput {
public:
	SQLIteratorOutput(T &);
	virtual ~SQLIteratorOutput(); // Change to virtual if base class

protected:

	virtual void print(ostream&) const; // Change to virtual if base class	

private:
// No copy allowed
	SQLIteratorOutput(const SQLIteratorOutput&);
	SQLIteratorOutput& operator=(const SQLIteratorOutput&);

// -- Members
	T& iterator_;
	//bool headerSaved = true;
	unsigned long long count_;

// -- Methods
	// None

// -- Overridden methods

	virtual void size(int);
	virtual void reset();
	virtual void flush();
	virtual bool output(const expression::Expressions&);
	virtual void prepare(SQLSelect&);
	virtual void cleanup(SQLSelect&);
	virtual unsigned long long count();

	virtual void outputReal(double, bool) const { NOTIMP; };
	virtual void outputDouble(double, bool) const { NOTIMP; };
	virtual void outputInt(double, bool) const { NOTIMP; };
	virtual void outputUnsignedInt(double, bool) const { NOTIMP; };
	virtual void outputString(double, bool) const { NOTIMP; };
	virtual void outputBitfield(double, bool) const { NOTIMP; };
};

} // namespace sql
} // namespace odb

#include "odblib/SQLIteratorOutput.cc"

#endif
