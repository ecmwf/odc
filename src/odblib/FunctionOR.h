// File FunctionOR.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef FunctionOR_H
#define FunctionOR_H

#include "FunctionExpression.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

class FunctionOR : public FunctionExpression {
public:
	FunctionOR(const string&,const expression::Expressions&);
	FunctionOR(const FunctionOR&);
	~FunctionOR();

// -- Overridden methods
	SQLExpression* clone() const;

	virtual double eval(bool& missing) const;
	virtual SQLExpression* simplify(bool&);

private:
// No copy allowed
	FunctionOR& operator=(const FunctionOR&);

// -- Friends
	//friend ostream& operator<<(ostream& s,const FunctionOR& p)
	//	{ p.print(s); return s; }
};

} // namespace function
} // namespace expression 
} // namespace sql
} // namespace odb

#endif
