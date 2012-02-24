#include "FunctionNORM.h"
#include "math.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

FunctionNORM::FunctionNORM(const string& name,const expression::Expressions& args)
: FunctionExpression(name,args),
  value_(0),
  resultNULL_(true)
{}

FunctionNORM::FunctionNORM(const FunctionNORM& other)
: FunctionExpression(other.name_, other.args_),
  value_(other.value_),
  resultNULL_(other.resultNULL_)
{}

SQLExpression* FunctionNORM::clone() const { return new FunctionNORM(*this); }

FunctionNORM::~FunctionNORM() {}

double FunctionNORM::eval(bool& missing) const
{
	if (resultNULL_)
	{
		missing = true;
		return (double) 0;
	}
    double lvalue =  (value_ > 0) ? sqrt(value_) : (double)0;
	return lvalue;
}

void FunctionNORM::prepare(SQLSelect& sql)
{
	FunctionExpression::prepare(sql);
	value_ = 0;
}

void FunctionNORM::cleanup(SQLSelect& sql)
{
	FunctionExpression::cleanup(sql);
	value_ = 0;
}

void FunctionNORM::partialResult() 
{
	bool missing = false;
	double x = args_[0]->eval(missing);
	double y = args_[1]->eval(missing);
	if(! missing)
	{
		value_ += x*y;
		resultNULL_ = false;
	}
}

static FunctionMaker<FunctionNORM> make_NORM("norm",2);

} // namespace function
} // namespace expression
} // namespace sql
} // namespace odb

