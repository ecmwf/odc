#include "FunctionOR.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

FunctionOR::FunctionOR(const string& name,const expression::Expressions& args)
: FunctionExpression(name,args)
{}

FunctionOR::FunctionOR(const FunctionOR& other)
: FunctionExpression(other.name_, other.args_)
{}

FunctionOR::~FunctionOR() {}

SQLExpression* FunctionOR::clone() const { return new FunctionOR(*this); }

double FunctionOR::eval(bool& missing) const
{
	return args_[0]->eval(missing) || args_[1]->eval(missing);
}

SQLExpression* FunctionOR::simplify(bool& changed) 
{
	SQLExpression* x = FunctionExpression::simplify(changed);
	if(x) return x;

	for(int i = 0; i < 2 ; i++)
	{
		bool missing = false;
		if(args_[i]->isConstant())
			if(args_[i]->eval(missing))
			{
				cout << "SYMPLIFY " << *this << " to 1" << endl;;
				changed = true;
				return SQLExpression::number(1);
			}
	}

	return 0;
}

static FunctionMaker<FunctionOR> make_OR("or",2);

} // namespace function
} // namespace expression
} // namespace sql
} // namespace odb
