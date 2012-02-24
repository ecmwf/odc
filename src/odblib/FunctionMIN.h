// File FunctionMIN.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef FunctionMIN_H
#define FunctionMIN_H

#include "FunctionExpression.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

class FunctionMIN : public FunctionExpression {
public:
	FunctionMIN(const string&,const expression::Expressions&);
	FunctionMIN(const FunctionMIN&);
	~FunctionMIN(); // Change to virtual if base class

	SQLExpression* clone() const;

	virtual const odb::sql::type::SQLType* type() const;

private:
// No copy allowed
	FunctionMIN& operator=(const FunctionMIN&);

	double value_;

// -- Overridden methods
	virtual void prepare(SQLSelect&);
	virtual void cleanup(SQLSelect&);
	virtual void partialResult();
	virtual double eval(bool& missing) const;
	bool isAggregate() const { return true; }

	virtual void output(SQLOutput&) const;
// -- Friends
	//friend ostream& operator<<(ostream& s,const FunctionMIN& p)
	//	{ p.print(s); return s; }
};

} // namespace function
} // namespace expression 
} // namespace sql
} // namespace odb 

#endif
