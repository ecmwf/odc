// File FunctionMAX.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef FunctionMAX_H
#define FunctionMAX_H

#include "FunctionExpression.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

class FunctionMAX : public FunctionExpression {
public:
	FunctionMAX(const string&,const expression::Expressions&);
	FunctionMAX(const FunctionMAX&);
	~FunctionMAX(); // Change to virtual if base class

	SQLExpression* clone() const;

	virtual const odb::sql::type::SQLType* type() const;

private:
// No copy allowed
	FunctionMAX& operator=(const FunctionMAX&);

	double value_;

// -- Overridden methods
	virtual void prepare(SQLSelect&);
	virtual void cleanup(SQLSelect&);
	virtual void partialResult();
	virtual double eval(bool& missing) const;
	bool isAggregate() const { return true; }

	virtual void output(SQLOutput&) const;

// -- Friends
	//friend ostream& operator<<(ostream& s,const FunctionMAX& p)
	//	{ p.print(s); return s; }
};

} // namespace function
} // namespace expression 
} // namespace sql
} // namespace odb

#endif
