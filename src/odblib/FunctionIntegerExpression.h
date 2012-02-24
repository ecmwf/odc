// File FunctionIntegerExpression.h
// ECMWF July 2010

#ifndef FUNCTION_INTEGER_EXPRESSION_H
#define FUNCTION_INTEGER_EXPRESSION_H

#include "FunctionExpression.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

class FunctionIntegerExpression : public FunctionExpression {
public:
	FunctionIntegerExpression(const string&,const expression::Expressions&);
	~FunctionIntegerExpression(); // Change to virtual if base class

// -- Overridden methods
	virtual const odb::sql::type::SQLType* type() const;
	virtual void output(ostream& s) const;

private:
// No copy allowed
	FunctionIntegerExpression(const FunctionIntegerExpression&);
	FunctionIntegerExpression& operator=(const FunctionIntegerExpression&);

// -- Friends
	//friend ostream& operator<<(ostream& s,const FunctionIntegerExpression& p)
	//	{ p.print(s); return s; }
};

} // namespace function
} // namespace expression 
} // namespace sql
} // namespace odb

#endif
