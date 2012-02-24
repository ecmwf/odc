/// \file FunctionTHIN.h
/// ECMWF July 2010

#ifndef FunctionTHIN_H
#define FunctionTHIN_H

#include "FunctionExpression.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

class FunctionTHIN : public FunctionExpression {
public:
	FunctionTHIN(const string&, const expression::Expressions&);
	FunctionTHIN(const FunctionTHIN&);
	~FunctionTHIN(); // Change to virtual if base class

	SQLExpression* clone() const;

// -- Overridden methods
	virtual const odb::sql::type::SQLType* type() const;
	virtual void output(ostream& s) const;
protected:
// -- Overridden methods
	virtual void print(ostream& s) const;
	virtual void prepare(SQLSelect&);
	virtual void cleanup(SQLSelect&);
	virtual bool isConstant() const;
	virtual double eval(bool& missing) const;
	virtual SQLExpression* simplify(bool&);
	bool isAggregate() const { return false; }

private:
// No copy allowed
	FunctionTHIN& operator=(const FunctionTHIN&);

	unsigned long long* count_;
// -- Friends
	//friend ostream& operator<<(ostream& s,const FunctionTHIN& p)
	//	{ p.print(s); return s; }
};

} // namespace function
} // namespace expression 
} // namespace sql
} // namespace odb 

#endif
