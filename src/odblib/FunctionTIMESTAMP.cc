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
#include <limits.h>

#include "odblib/FunctionTIMESTAMP.h"

#define RMDI   -2147483647
#define trunc(x) ((x) -fmod((x), 1))

namespace odb {
namespace sql {
namespace expression {
namespace function {

FunctionTIMESTAMP::FunctionTIMESTAMP(const string& name,const expression::Expressions& args)
: FunctionExpression(name,args)
{}

FunctionTIMESTAMP::FunctionTIMESTAMP(const FunctionTIMESTAMP& other)
: FunctionExpression(other.name_, other.args_)
{}

SQLExpression* FunctionTIMESTAMP::clone() const { return new FunctionTIMESTAMP(*this); }

FunctionTIMESTAMP::~FunctionTIMESTAMP() {}

double FunctionTIMESTAMP::eval(bool& missing) const
{
    double indate = args_[0]->eval(missing);
    double intime = args_[1]->eval(missing);
// Merge "YYYYMMDD" and "HHMMSS" into "YYYYMMDDHHMMSS"
    double outstamp = RMDI; // initialized to missing data indicator; indicates error
    if (indate >=0 && indate <= INT_MAX &&
        intime >= 0 && intime <= 240000) {
      long long int lldate = (long long int) indate;
      long long int lltime = (long long int) intime;
      long long int tstamp = lldate * 1000000ll + lltime;
      outstamp = tstamp;
      outstamp = trunc(outstamp);
    }

	return outstamp;
}

const odb::sql::type::SQLType* FunctionTIMESTAMP::type() const
{
	return &odb::sql::type::SQLType::lookup("integer");
}

void FunctionTIMESTAMP::output(ostream& s) const
{
	bool missing;
    s << static_cast<long long int>(eval(missing));
}

static FunctionMaker<FunctionTIMESTAMP> make_TIMESTAMP("timestamp",2);

} // namespace function
} // namespace expression
} // namespace sql
} // namespace odb

