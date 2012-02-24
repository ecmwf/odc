// File FunctionSUM.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef FunctionSUM_H
#define FunctionSUM_H

#include "FunctionExpression.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

class FunctionSUM : public FunctionExpression {
public:
	FunctionSUM(const string&,const expression::Expressions&);
	FunctionSUM(const FunctionSUM&);
	~FunctionSUM(); // Change to virtual if base class

	SQLExpression* clone() const;

private:
// No copy allowed
	FunctionSUM& operator=(const FunctionSUM&);

	unsigned long long count_;
	double value_;

// -- Overridden methods
	virtual void prepare(SQLSelect&);
	virtual void cleanup(SQLSelect&);
	virtual void partialResult();
	virtual double eval(bool& missing) const;
	bool isAggregate() const { return true; }
	bool resultNULL_;

// -- Friends
	//friend ostream& operator<<(ostream& s,const FunctionSUM& p)
	//	{ p.print(s); return s; }
};

} // namespace function
} // namespace expression 
} // namespace sql
} // namespace odb 

#endif
