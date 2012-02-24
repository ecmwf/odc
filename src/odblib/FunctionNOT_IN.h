// File FunctionNOT_IN.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef FunctionNOT_IN_H
#define FunctionNOT_IN_H

#include "FunctionExpression.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

class FunctionNOT_IN : public FunctionExpression {
public:
	FunctionNOT_IN(const string&, const expression::Expressions&);
	FunctionNOT_IN(const FunctionNOT_IN&);
	~FunctionNOT_IN();

	SQLExpression* clone() const;

private:
// No copy allowed
	FunctionNOT_IN& operator=(const FunctionNOT_IN&);

	int size_;

// -- Overridden methods
	virtual double eval(bool& missing) const;

// -- Friends
	//friend ostream& operator<<(ostream& s,const FunctionNOT_IN& p)
	//	{ p.print(s); return s; }
};

} // namespace function
} // namespace expression 
} // namespace sql
} // namespace odb

#endif
