// File FunctionAND.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef FunctionAND_H
#define FunctionAND_H

#include "FunctionExpression.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

class FunctionAND : public FunctionExpression {
public:
	FunctionAND(const string&, const expression::Expressions&);
	FunctionAND(const FunctionAND&);
	~FunctionAND();

	SQLExpression* clone() const;

private:
// No copy allowed
	FunctionAND& operator=(const FunctionAND&);

// -- Overridden methods
	virtual double eval(bool& missing) const;
	virtual SQLExpression* simplify(bool&);
	virtual bool andSplit(expression::Expressions&);

// -- Friends
	//friend ostream& operator<<(ostream& s,const FunctionAND& p)
	//	{ p.print(s); return s; }
};

} // namespace function
} // namespace expression 
} // namespace sql
} // namespace odb 

#endif
