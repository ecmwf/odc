#include "FunctionJOIN.h"
#include "ColumnExpression.h"
#include "Exceptions.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

FunctionJOIN::FunctionJOIN(const string& name,const expression::Expressions& args)
: FunctionExpression(name, args)
{}

FunctionJOIN::FunctionJOIN(const FunctionJOIN& other)
: FunctionExpression(other.name_, other.args_)
{}

SQLExpression* FunctionJOIN::clone() const { return new FunctionJOIN(*this); }

FunctionJOIN::~FunctionJOIN() {}

double FunctionJOIN::eval(bool& missing) const
{
	return args_[0]->eval(missing) == args_[1]->eval(missing);
}

bool FunctionJOIN::useIndex()
{
//	return args_[0]->indexed() && args_[1]->indexed();
	return args_[1]->indexed();
}

SQLIndex* FunctionJOIN::getIndex(double*)
{
	ColumnExpression* c = dynamic_cast<ColumnExpression*>(args_[0]); ASSERT(c);
	return args_[1]->getIndex(c->current());
	//return args_[0]->getIndex(
	//return args_[1]->getIndex();
	return 0;
}

static FunctionMaker<FunctionJOIN> make_JOIN("join",2);

} // namespace function
} // namespace expression
} // namespace sql
} // namespace odb

