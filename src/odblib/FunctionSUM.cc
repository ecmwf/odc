#include "FunctionSUM.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

FunctionSUM::FunctionSUM(const string& name,const expression::Expressions& args)
: FunctionExpression(name,args),
  value_(0),
  resultNULL_(true)
{}

FunctionSUM::FunctionSUM(const FunctionSUM& other)
: FunctionExpression(other.name_, other.args_),
  value_(other.value_),
  resultNULL_(other.resultNULL_)
{}

FunctionSUM::~FunctionSUM() {}

SQLExpression* FunctionSUM::clone() const { return new FunctionSUM(*this); }

double FunctionSUM::eval(bool& missing) const
{
	if (resultNULL_)
		missing = true;
	return value_;
}

void FunctionSUM::prepare(SQLSelect& sql)
{
	FunctionExpression::prepare(sql);
	value_ = 0;
}

void FunctionSUM::cleanup(SQLSelect& sql)
{
	FunctionExpression::cleanup(sql);
	value_ = 0;
}

void FunctionSUM::partialResult() 
{
	bool missing = false;
	double value = args_[0]->eval(missing);
	if(! missing)
	{
		value_ += value;
		resultNULL_ = false;
	}
}

static FunctionMaker<FunctionSUM> make_SUM("sum",1);

} // namespace function
} // namespace expression
} // namespace sqol
} // namespace odb 
