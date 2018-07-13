/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file NullColumn.h

#ifndef NullColumn_H
#define NullColumn_H

#include "odb_api/sql/SQLColumn.h"

namespace odb {
namespace sql {

class SQLTable;

class NullColumn : public SQLColumn {
public:
	NullColumn(const type::SQLType&, SQLTable&, const std::string&, int, bool hasMissingValue, double missingValue, const BitfieldDef&, double*);
	NullColumn(const type::SQLType&, SQLTable&, const std::string&, int, bool hasMissingValue, double missingValue, double*);
	~NullColumn();

	void value(double* p) { value_ = p; }
	double * value() const { return const_cast<double *>(&missing_); }

private:
	NullColumn(const NullColumn&);
	NullColumn& operator=(const NullColumn&);

	double* value_;
	double  missing_;

	virtual void rewind();
	virtual double next(bool& missing);
	virtual void advance(unsigned long);

// -- Friends
	//friend std::ostream& operator<<(std::ostream& s,const NullColumn& p)
	//	{ p.print(s); return s; }
};

} // namespace sql 
} // namespace odb 

#endif
