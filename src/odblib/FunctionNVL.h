// File FunctionNVL.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef FunctionNVL_H
#define FunctionNVL_H

#include "FunctionExpression.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

class FunctionNVL : public FunctionExpression {
public:
	FunctionNVL(const string&,const expression::Expressions&);
	FunctionNVL(const FunctionNVL&);
	~FunctionNVL(); // Change to virtual if base class

	SQLExpression* clone() const;

private:
// No copy allowed
	FunctionNVL& operator=(const FunctionNVL&);

// -- Overridden methods
	virtual double eval(bool& missing) const;

// -- Friends
	//friend ostream& operator<<(ostream& s,const FunctionNVL& p)
	//	{ p.print(s); return s; }
};

} // namespace function
} // namespace expression 
} // namespace sql
} // namespace odb

#endif
