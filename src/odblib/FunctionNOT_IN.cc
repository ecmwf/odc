#include "FunctionNOT_IN.h"
#include "FunctionEQ.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

FunctionNOT_IN::FunctionNOT_IN(const string& name,const expression::Expressions& args)
: FunctionExpression(name,args),
  size_(args.size() - 1)
{}

FunctionNOT_IN::FunctionNOT_IN(const FunctionNOT_IN& other)
: FunctionExpression(other.name_, other.args_),
  size_(other.args_.size() - 1)
{}

FunctionNOT_IN::~FunctionNOT_IN() {}

SQLExpression* FunctionNOT_IN::clone() const { return new FunctionNOT_IN(*this); }

double FunctionNOT_IN::eval(bool& missing) const
{
	const SQLExpression& x = *args_[size_];
	for(size_t i = 0; i < size_; ++i)
	{
		double y = args_[i]->eval(missing);
		if (FunctionEQ::equal(x, *args_[i], missing))
			return false;
	}

	return true;
}

static FunctionMaker<FunctionNOT_IN> make_NOT_IN("not_in");

} // namespace function
} // namespace expression
} // namespace sql
} // namespace odb

