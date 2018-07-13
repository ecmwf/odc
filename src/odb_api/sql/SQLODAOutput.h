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

#ifndef odb_api_SQLODAOutput_H
#define odb_api_SQLODAOutput_H

#include "eckit/exception/Exceptions.h"

#include "odb_api/sql/SQLOutput.h"
#include "odb_api/sql/type/SQLBitfield.h"
#include "odb_api/MetaData.h"

namespace odb {
namespace sql {

class Reader;

template<typename WRITER>
class SQLODAOutput : public SQLOutput {
public:
	SQLODAOutput(WRITER*);
	SQLODAOutput(WRITER*, const MetaData&);
	virtual ~SQLODAOutput(); // Change to virtual if base class

protected:
	virtual void print(std::ostream&) const; 	

private:
	SQLODAOutput(const SQLODAOutput&);
	SQLODAOutput& operator=(const SQLODAOutput&);

// -- Members
	WRITER* writer_;
	typename WRITER::iterator it_;
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

	virtual void outputReal(double, bool) { NOTIMP; };
	virtual void outputDouble(double, bool) { NOTIMP; };
	virtual void outputInt(double, bool) { NOTIMP; };
	virtual void outputUnsignedInt(double, bool) { NOTIMP; };
	virtual void outputString(double, bool) { NOTIMP; };
	virtual void outputBitfield(double, bool) { NOTIMP; };
};

} // namespace sql
} // namespace odb

#endif
