// File FunctionJOIN.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef FunctionJOIN_H
#define FunctionJOIN_H

#include "FunctionExpression.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

class FunctionJOIN : public FunctionExpression {
public:
	FunctionJOIN(const string&, const expression::Expressions&);
	FunctionJOIN(const FunctionJOIN&);
	~FunctionJOIN(); // Change to virtual if base class

	SQLExpression* clone() const;

private:
// No copy allowed
	FunctionJOIN& operator=(const FunctionJOIN&);

// -- Overridden methods
	virtual double eval(bool& missing) const;
	bool useIndex();
	SQLIndex* getIndex(double*);

// -- Friends
	//friend ostream& operator<<(ostream& s,const FunctionJOIN& p)
	//	{ p.print(s); return s; }
};

} // namespace function
} // namespace expression 
} // namespace sql
} // namespace odb 

#endif
