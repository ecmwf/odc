#include "FunctionNULL.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

FunctionNULL::FunctionNULL(const string& name,const expression::Expressions& args)
: FunctionExpression(name, args)
{} 

FunctionNULL::FunctionNULL(const FunctionNULL& other)
: FunctionExpression(other.name_, other.args_)
{} 

FunctionNULL::~FunctionNULL() {}

SQLExpression* FunctionNULL::clone() const { return new FunctionNULL(*this); }

// Don't set the missing flag
double FunctionNULL::eval(bool& ) const
{
	bool missing = false;
	args_[0]->eval(missing);
	return missing;
}

static FunctionMaker<FunctionNULL> make_NULL("null",1);
static FunctionMaker<FunctionNULL> make_ISNULL("isnull",1);

} // namespace function
} // namespace expression
} // namespace sql
} // namespace odb

