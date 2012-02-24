// File FunctionNOT_NULL.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef FunctionNOT_NULL_H
#define FunctionNOT_NULL_H

#include "FunctionExpression.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

class FunctionNOT_NULL : public FunctionExpression {
public:
	FunctionNOT_NULL(const string&,const expression::Expressions&);
	FunctionNOT_NULL(const FunctionNOT_NULL&);
	~FunctionNOT_NULL(); // Change to virtual if base class

	SQLExpression* clone() const;
private:
// No copy allowed
	FunctionNOT_NULL& operator=(const FunctionNOT_NULL&);

// -- Overridden methods
	virtual double eval(bool& missing) const;

// -- Friends
	//friend ostream& operator<<(ostream& s,const FunctionNOT_NULL& p)
	//	{ p.print(s); return s; }

};

} // namespace function
} // namespace expression 
} // namespace sql
} // namespace odb 

#endif
