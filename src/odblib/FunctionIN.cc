#include "FunctionIN.h"
#include "FunctionEQ.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

FunctionIN::FunctionIN(const string& name, const expression::Expressions& args)
: FunctionExpression(name, args),
  size_(args.size() - 1)
{}

FunctionIN::FunctionIN(const FunctionIN& other)
: FunctionExpression(other.name_, other.args_),
  size_(other.args_.size() - 1)
{}

FunctionIN::~FunctionIN() {}

SQLExpression* FunctionIN::clone() const { return new FunctionIN(*this); }

double FunctionIN::eval(bool& missing) const
{
	const SQLExpression& x = *args_[size_];
	for(size_t i = 0 ; i < size_ ; ++i)
		if (FunctionEQ::equal(x, *args_[i], missing))
			return true;
	return false;
}

static FunctionMaker<FunctionIN> make_IN("in");

} // namespace function
} // namespace expression
} // namespace sql
} // namespace odb 
