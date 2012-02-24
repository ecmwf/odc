#include "FunctionNVL.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

FunctionNVL::FunctionNVL(const string& name,const expression::Expressions& args)
: FunctionExpression(name, args)
{}

FunctionNVL::FunctionNVL(const FunctionNVL& other)
: FunctionExpression(other.name_, other.args_)
{}

FunctionNVL::~FunctionNVL() {}

SQLExpression* FunctionNVL::clone() const { return new FunctionNVL(*this); }

// Don't set the missing flag
double FunctionNVL::eval(bool& ) const
{
	bool missing = false;
	double x = args_[0]->eval(missing);
	return missing ? args_[1]->eval(missing) : x;
}

static FunctionMaker<FunctionNVL> make_NVL("nvl",2);

} // namespace function
} // namespace expression
} // namespace sql
} // namespace odb

