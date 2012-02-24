#include "FunctionSTDEV.h"
#include <math.h>

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

