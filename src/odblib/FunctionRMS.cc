#include "FunctionRMS.h"
#include <math.h>

namespace odb {
namespace sql {
namespace expression {
namespace function {

FunctionRMS::FunctionRMS(const string& name, const expression::Expressions& args)
: FunctionExpression(name, args),
  count_(0),
  squares_(0)
{}

FunctionRMS::FunctionRMS(const FunctionRMS& other)
: FunctionExpression(other.name_, other.args_),
  count_(other.count_),
  squares_(other.squares_)
{}

FunctionRMS::~FunctionRMS() {}

SQLExpression* FunctionRMS::clone() const { return new FunctionRMS(*this); } 

double FunctionRMS::eval(bool& missing) const
{
	if(! count_) {
		missing = true;
		return 0;
	}

	return sqrt(squares_/count_);
}

void FunctionRMS::prepare(SQLSelect& sql)
{
	FunctionExpression::prepare(sql);
	count_ = 0;
	squares_ = 0;
}

void FunctionRMS::cleanup(SQLSelect& sql)
{
//cout << "Cleanup  FunctionRMS " << count_ << " " << value_ << endl;
	FunctionExpression::cleanup(sql);
	squares_ = 0;
	count_ = 0;
}

void FunctionRMS::partialResult() 
{
	bool missing = false;
	double value = args_[0]->eval(missing);
	if(! missing)
	{
		squares_ += value * value;
		count_++;
	}
//	else cout << "missing" << endl;
}

static FunctionMaker<FunctionRMS> make_RMS("rms",1);

} // namespace function
} // namespace expression
} // namespace sql
} // namespace odb

