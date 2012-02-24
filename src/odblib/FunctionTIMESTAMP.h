// File FunctionTIMESTAMP.h
// ECMWF July 2010

#ifndef FunctionTIMESTAMP_H
#define FunctionTIMESTAMP_H

#include "FunctionExpression.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

class FunctionTIMESTAMP : public FunctionExpression {
public:
	FunctionTIMESTAMP(const string&,const expression::Expressions&);
	FunctionTIMESTAMP(const FunctionTIMESTAMP&);
	~FunctionTIMESTAMP(); // Change to virtual if base class

	SQLExpression* clone() const;

// -- Overridden methods
	virtual const odb::sql::type::SQLType* type() const;
	virtual void output(ostream& s) const;

private:
	FunctionTIMESTAMP& operator=(const FunctionTIMESTAMP&);

// -- Overridden methods
	virtual double eval(bool& missing) const;

// -- Friends
	//friend ostream& operator<<(ostream& s,const FunctionTIMESTAMP& p)
	//	{ p.print(s); return s; }
};

} // namespace function
} // namespace expression 
} // namespace sql
} // namespace odb

#endif
