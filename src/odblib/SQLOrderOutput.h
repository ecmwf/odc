/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file SQLOrderOutput.h
/// Piotr Kuchta - ECMWF Nov 11

#ifndef SQLOrderOutput_H
#define SQLOrderOutput_H

#include "odblib/SQLOutput.h"
#include "odblib/SQLExpressionEvaluated.h"
#include "odblib/OrderByExpressions.h"

namespace odb {
namespace sql {

class SQLOrderOutput : public SQLOutput {
public:
	SQLOrderOutput(SQLOutput* output, const pair<Expressions,vector<bool> >& by);
	virtual ~SQLOrderOutput();

protected:
	virtual void print(std::ostream&) const;

private:
// No copy allowed
	SQLOrderOutput(const SQLOrderOutput&);
	SQLOrderOutput& operator=(const SQLOrderOutput&);

// -- Members
	auto_ptr<SQLOutput> output_;
	std::pair<Expressions,vector<bool> > by_;
	
	typedef map<OrderByExpressions, VectorOfExpressions> SortedResults;

	SortedResults sortedResults_;
    vector<size_t> byIndices_;

// -- Overridden methods
	virtual void size(int);
	virtual void reset();
	virtual void flush();
	virtual bool output(const Expressions&);
	virtual void prepare(SQLSelect&);
	virtual void cleanup(SQLSelect&);
	virtual unsigned long long count();

	virtual void outputReal(double, bool) const { NOTIMP; };
	virtual void outputDouble(double, bool) const { NOTIMP; };
	virtual void outputInt(double, bool) const { NOTIMP; };
	virtual void outputUnsignedInt(double, bool) const { NOTIMP; };
	virtual void outputString(double, bool) const { NOTIMP; };
	virtual void outputBitfield(double, bool) const { NOTIMP; };

	virtual const SQLOutputConfig& config();
	virtual void config(SQLOutputConfig&);

	friend std::ostream& operator<<(std::ostream& s, const SQLOrderOutput& o)
		{ o.print(s); return s; }
};

} // namespace sql
} // namespace odb

#endif
