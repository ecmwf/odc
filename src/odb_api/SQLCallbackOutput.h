/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file SQLCallbackOutput.h
/// Piotr Kuchta - ECMWF May 2015

#ifndef SQLCallbackOutput_H
#define SQLCallbackOutput_H

#include "odb_api/SQLOutput.h"
#include "odb_api/ecml_data/ResultSet.h"

namespace odb {
namespace sql {

class SQLCallbackOutput : public SQLOutput {
public:
	SQLCallbackOutput(eckit::ExecutionContext&);
	virtual ~SQLCallbackOutput(); 

protected:
	virtual void print(std::ostream&) const; 	

private:
	SQLCallbackOutput(const SQLCallbackOutput&);
	SQLCallbackOutput& operator=(const SQLCallbackOutput&);

	unsigned long long count_;
    //std::vector<double> values_;
    ResultSet resultSet_;

// -- Overridden methods
	virtual void size(int);
	virtual void reset();
	virtual void flush(eckit::ExecutionContext*);
	virtual bool output(const expression::Expressions&, eckit::ExecutionContext*);
	virtual void prepare(SQLSelect&);
	virtual void cleanup(SQLSelect&);
	virtual unsigned long long count();

    void outputValue(double x, bool missing);
	virtual void outputReal(double, bool);
	virtual void outputDouble(double, bool);
	virtual void outputInt(double, bool);
	virtual void outputUnsignedInt(double, bool);
	virtual void outputString(double, bool);
	virtual void outputBitfield(double, bool);

};

} // namespace sql
} // namespace odb

#endif
