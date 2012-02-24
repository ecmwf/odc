#include "FunctionVAR.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

FunctionVAR::FunctionVAR(const string& name,const expression::Expressions& args)
: FunctionExpression(name,args),
  count_(0),
  value_(0),
  squares_(0)
{}

FunctionVAR::FunctionVAR(const FunctionVAR& other)
: FunctionExpression(other.name_, other.args_),
  count_(other.count_),
  value_(other.value_),
  squares_(other.squares_)
{}

FunctionVAR::~FunctionVAR() {}

SQLExpression* FunctionVAR::clone() const { return new FunctionVAR(*this); }

double FunctionVAR::eval(bool& missing) const
{
	double x,y;

	if(!count_) {
		missing = true;
		return 0;
	}

	x = value_   / count_;
	y = squares_ / count_ ;

	return y - x*x;
}

void FunctionVAR::prepare(SQLSelect& sql)
{
	FunctionExpression::prepare(sql);
	value_ = 0;
	count_ = 0;
	squares_ = 0;
}

void FunctionVAR::cleanup(SQLSelect& sql)
{
//cout << "Cleanup  FunctionVAR " << count_ << " " << value_ << endl;
	FunctionExpression::cleanup(sql);
	squares_ = 0;
	value_ = 0;
	count_ = 0;
}

void FunctionVAR::partialResult() 
{
	bool missing = false;
	double value = args_[0]->eval(missing);
	if(!missing)
	{
		value_   += value;
		squares_ += value * value;
		count_++;
	}
//	else cout << "missing" << endl;
}

static FunctionMaker<FunctionVAR> make_VAR("var",1);
static FunctionMaker<FunctionVAR> make_VARP("varp",1);

} // namespace function
} // namespace expression
} // namespace sql
} // namespace odb

