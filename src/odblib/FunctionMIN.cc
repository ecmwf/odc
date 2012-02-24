#include "FunctionMIN.h"
#include <values.h>

namespace odb {
namespace sql {
namespace expression {
namespace function {

FunctionMIN::FunctionMIN(const string& name,const expression::Expressions& args)
: FunctionExpression(name, args),
  value_(DBL_MAX)
{}

FunctionMIN::FunctionMIN(const FunctionMIN& other)
: FunctionExpression(other.name_, other.args_),
  value_(other.value_)
{}

SQLExpression* FunctionMIN::clone() const { return new FunctionMIN(*this); }

const odb::sql::type::SQLType* FunctionMIN::type() const { return args_[0]->type(); }

FunctionMIN::~FunctionMIN() {}

double FunctionMIN::eval(bool& missing) const
{
	if (value_ == DBL_MAX)
		missing = true;

	return value_;
}

void FunctionMIN::prepare(SQLSelect& sql)
{
	FunctionExpression::prepare(sql);
	value_ = DBL_MAX;
}

void FunctionMIN::cleanup(SQLSelect& sql)
{
	FunctionExpression::cleanup(sql);
	value_ = DBL_MAX;
}

void FunctionMIN::output(SQLOutput& s) const 
{ 
	bool missing = false;
	double d = eval(missing); 
	type()->output(s, d, missing);
}

void FunctionMIN::partialResult() 
{
	bool missing = false;
	double value = args_[0]->eval(missing);
	if(!missing)
		if(value < value_)
			value_ = value;
}

static FunctionMaker<FunctionMIN> make_MIN("min",1);

} // namespace function
} // namespace expression
} // namespace sql
} // namespace odb
