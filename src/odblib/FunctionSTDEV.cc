/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <math.h>

#include "odblib/FunctionSTDEV.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

FunctionSTDEV::FunctionSTDEV(const string& name,const expression::Expressions& args)
: FunctionVAR(name, args)
{}

FunctionSTDEV::FunctionSTDEV(const FunctionSTDEV& other)
: FunctionVAR(other.name_, other.args_)
{}

FunctionSTDEV::~FunctionSTDEV() {}

SQLExpression* FunctionSTDEV::clone() const { return new FunctionSTDEV(*this); }

double FunctionSTDEV::eval(bool& missing) const
{
	double x = FunctionVAR::eval(missing);
	if (x < 0) x = 0;
	return sqrt(x);
}

static FunctionMaker<FunctionSTDEV> make_STDEV("stdev",1);
static FunctionMaker<FunctionSTDEV> make_STDEVP("stdevp",1);

} // namespace function
} // namespace expression
} // namespace sql 
} // namespace odb

