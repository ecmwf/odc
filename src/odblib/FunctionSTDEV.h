/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File FunctionSTDEV.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef FunctionSTDEV_H
#define FunctionSTDEV_H

#include "odblib/FunctionVAR.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

class FunctionSTDEV : public FunctionVAR {
public:
	FunctionSTDEV(const string&, const expression::Expressions&);
	FunctionSTDEV(const FunctionSTDEV&);
	~FunctionSTDEV(); // Change to virtual if base class

	SQLExpression* clone() const;

private:
// No copy allowed
	FunctionSTDEV& operator=(const FunctionSTDEV&);

	virtual double eval(bool& missing) const;
	virtual const odb::sql::type::SQLType* type() const;

// -- Friends
	//friend ostream& operator<<(ostream& s,const FunctionSTDEV& p)
	//	{ p.print(s); return s; }
};

} // namespace function
} // namespace expression 
} // namespace sql
} // namespace odb

#endif
