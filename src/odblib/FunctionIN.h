// File FunctionIN.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef FunctionIN_H
#define FunctionIN_H

#include "FunctionExpression.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

class FunctionIN : public FunctionExpression {
public:
	FunctionIN(const string&, const expression::Expressions&);
	FunctionIN(const FunctionIN&);
	~FunctionIN();

	SQLExpression* clone() const;

private:
// No copy allowed
	FunctionIN& operator=(const FunctionIN&);

	size_t size_;

	virtual double eval(bool& missing) const;

// -- Friends
	//friend ostream& operator<<(ostream& s,const FunctionIN& p)
	//	{ p.print(s); return s; }
};

} // namespace function
} // namespace expression 
} // namespace sql
} // namespace odb

#endif
