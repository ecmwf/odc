// File FunctionEQ_BOXLON.h
// ECMWF July 2010

#ifndef FunctionEQ_BOXLON_H
#define FunctionEQ_BOXLON_H

#include "FunctionExpression.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

class FunctionEQ_BOXLON : public FunctionExpression {
public:
	FunctionEQ_BOXLON(const string&,const expression::Expressions&);
	FunctionEQ_BOXLON(const FunctionEQ_BOXLON&);
	~FunctionEQ_BOXLON(); // Change to virtual if base class

	SQLExpression* clone() const;

private:
// No copy allowed
	FunctionEQ_BOXLON& operator=(const FunctionEQ_BOXLON&);

// -- Overridden methods
	virtual double eval(bool& missing) const;

// -- Friends
	//friend ostream& operator<<(ostream& s,const FunctionEQ_BOXLON& p)
	//	{ p.print(s); return s; }

};

} // namespace function
} // namespace expression 
} // namespace sql
} // namespace odb

#endif
