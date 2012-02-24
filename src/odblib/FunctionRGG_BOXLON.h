// File FunctionRGG_BOXLON.h
// ECMWF July 2010

#ifndef FunctionRGG_BOXLON_H
#define FunctionRGG_BOXLON_H

#include "FunctionExpression.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

class FunctionRGG_BOXLON : public FunctionExpression {
public:
	FunctionRGG_BOXLON(const string&,const expression::Expressions&);
	FunctionRGG_BOXLON(const FunctionRGG_BOXLON&);
	~FunctionRGG_BOXLON(); // Change to virtual if base class

	SQLExpression* clone() const;

private:
// No copy allowed
	FunctionRGG_BOXLON& operator=(const FunctionRGG_BOXLON&);

// -- Overridden methods
	virtual double eval(bool& missing) const;

// -- Friends
	//friend ostream& operator<<(ostream& s,const FunctionRGG_BOXLON& p)
	//	{ p.print(s); return s; }
};

} // namespace function
} // namespace expression 
} // namespace sql
} // namespace odb

#endif
