/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file SQLODAOutput.h
/// Piotr Kuchta - ECMWF Jan 09

#ifndef SQLODAOutput_H
#define SQLODAOutput_H

#include "eckit/exception/Exceptions.h"

#include "odblib/SQLOutput.h"
#include "odblib/SQLBitfield.h"
#include "odblib/MetaData.h"

namespace odb {
namespace sql {

class Reader;

template<typename ITERATOR>
class SQLODAOutput : public SQLOutput {
public:
	SQLODAOutput(ITERATOR);
	SQLODAOutput(ITERATOR, const MetaData&);
	virtual ~SQLODAOutput(); // Change to virtual if base class

protected:
	virtual void print(std::ostream&) const; 	

private:
	SQLODAOutput(const SQLODAOutput&);
	SQLODAOutput& operator=(const SQLODAOutput&);

// -- Members
	ITERATOR writer_;
    MetaData metaData_;

	unsigned long long count_;

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

#endif
