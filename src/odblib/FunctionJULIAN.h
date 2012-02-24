// File FunctionJULIAN.h
// ECMWF July 2010

#ifndef FunctionJULIAN_H
#define FunctionJULIAN_H

#include "FunctionExpression.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

class FunctionJULIAN : public FunctionExpression {
public:
	FunctionJULIAN(const string&, const expression::Expressions&);
	FunctionJULIAN(const FunctionJULIAN&);
	~FunctionJULIAN(); // Change to virtual if base class

	SQLExpression* clone() const;
private:
// No copy allowed
	FunctionJULIAN& operator=(const FunctionJULIAN&);

// -- Overridden methods
	virtual double eval(bool& missing) const;

// -- Friends
	//friend ostream& operator<<(ostream& s,const FunctionJULIAN& p)
	//	{ p.print(s); return s; }
};

} // namespace function
} // namespace expression 
} // namespace sql
} // namespace odb

#endif
