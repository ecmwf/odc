// File FunctionDOTP.h
// ECMWF July 2010

#ifndef FunctionDOTP_H
#define FunctionDOTP_H

#include "FunctionExpression.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

class FunctionDOTP : public FunctionExpression {
public:
	FunctionDOTP(const string&,const expression::Expressions&);
	FunctionDOTP(const FunctionDOTP&);
	~FunctionDOTP(); // Change to virtual if base class

	SQLExpression* clone() const;

private:
// No copy allowed
	FunctionDOTP& operator=(const FunctionDOTP&);

	double value_;

// -- Overridden methods
	virtual void prepare(SQLSelect&);
	virtual void cleanup(SQLSelect&);
	virtual void partialResult();
	virtual double eval(bool& missing) const;

	bool isAggregate() const { return true; }

	bool resultNULL_;

// -- Friends
	//friend ostream& operator<<(ostream& s,const FunctionDOTP& p)
	//	{ p.print(s); return s; }
};

} // namespace function
} // namespace expression 
} // namespace sql
} // namespace odb

#endif
