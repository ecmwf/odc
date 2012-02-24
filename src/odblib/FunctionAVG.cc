#include "FunctionAVG.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

FunctionAVG::FunctionAVG(const string& name,const expression::Expressions& args)
: FunctionExpression(name,args),
  count_(0),
  value_(0)
{}

FunctionAVG::FunctionAVG(const FunctionAVG& other)
: FunctionExpression(other.name_, other.args_),
  count_(other.count_),
  value_(other.value_)
{}


SQLExpression* FunctionAVG::clone() const { return new FunctionAVG(*this); }


FunctionAVG::~FunctionAVG() {}

double FunctionAVG::eval(bool& missing) const
{
	if(!count_)
	{
		missing = true;
		return 0;
	}

	return value_ / count_;
}

void FunctionAVG::prepare(SQLSelect& sql)
{
	FunctionExpression::prepare(sql);
	value_ = 0;
	count_ = 0;
}

void FunctionAVG::cleanup(SQLSelect& sql)
{
//cout << "Cleanup  FunctionAVG " << count_ << " " << value_ << endl;
	FunctionExpression::cleanup(sql);
	value_ = 0;
	count_ = 0;
}

void FunctionAVG::partialResult() 
{
	bool missing = false;
	double value = args_[0]->eval(missing);
	if(!missing)
	{
		value_ += value;
		count_++;
	}
//	else cout << "missing" << endl;
}

static FunctionMaker<FunctionAVG> make_AVG("avg",1);

} // namespace function
} // namespace expression
} // namespace sql
} // namespace odb

