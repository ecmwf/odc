#include "FunctionDOTP.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

FunctionDOTP::FunctionDOTP(const string& name,const expression::Expressions& args)
: FunctionExpression(name,args),
  value_(0),
  resultNULL_(true)
{}

FunctionDOTP::FunctionDOTP(const FunctionDOTP& other)
: FunctionExpression(other.name_, other.args_),
  value_(other.value_),
  resultNULL_(other.resultNULL_)
{}

SQLExpression* FunctionDOTP::clone() const { return new FunctionDOTP(*this); }

FunctionDOTP::~FunctionDOTP() {}

double FunctionDOTP::eval(bool& missing) const
{
	if (resultNULL_)
		missing = true;
	return value_;
}

void FunctionDOTP::prepare(SQLSelect& sql)
{
	FunctionExpression::prepare(sql);
	value_ = 0;
}

void FunctionDOTP::cleanup(SQLSelect& sql)
{
	FunctionExpression::cleanup(sql);
	value_ = 0;
}

void FunctionDOTP::partialResult() 
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

static FunctionMaker<FunctionDOTP> make_DOTP("dotp",2);

} // namespace function
} // namespace expression
} // namespace sql
} // namespace odb

