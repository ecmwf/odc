/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Piotr Kuchta
/// @date July 2010
/// @author Simon Smart
/// @date January 2019

#ifndef odc_sql_FunctionRGG_BOXLON_H
#define odc_sql_FunctionRGG_BOXLON_H

#include "eckit/sql/expression/function/FunctionExpression.h"

namespace odc {
namespace sql {
namespace function {

//----------------------------------------------------------------------------------------------------------------------

class FunctionRGG_BOXLON : public eckit::sql::expression::function::FunctionExpression {
public:
    FunctionRGG_BOXLON(const std::string&, const eckit::sql::expression::Expressions&);
	FunctionRGG_BOXLON(const FunctionRGG_BOXLON&);
	~FunctionRGG_BOXLON(); 

	std::shared_ptr<SQLExpression> clone() const;

    static int arity() { return 3; }

private:
// No copy allowed
	FunctionRGG_BOXLON& operator=(const FunctionRGG_BOXLON&);

// -- Overridden methods
	virtual const eckit::sql::type::SQLType* type() const;
	virtual double eval(bool& missing) const;

// -- Friends
	//friend std::ostream& operator<<(std::ostream& s,const FunctionRGG_BOXLON& p)
	//	{ p.print(s); return s; }
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace function
} // namespace sql
} // namespace odc

#endif
