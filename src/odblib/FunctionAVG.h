// File FunctionAVG.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef FunctionAVG_H
#define FunctionAVG_H

#include "FunctionExpression.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

class FunctionAVG : public FunctionExpression {
public:
	FunctionAVG(const string&,const expression::Expressions&);
	FunctionAVG(const FunctionAVG&);
	~FunctionAVG(); // Change to virtual if base class

	SQLExpression* clone() const;
private:
// No copy allowed
	FunctionAVG& operator=(const FunctionAVG&);

// -- Members
	unsigned long long count_;
	double value_;

// -- Overridden methods
	virtual void prepare(SQLSelect&);
	virtual void cleanup(SQLSelect&);
	virtual void partialResult();
	virtual double eval(bool& missing) const;

	bool isAggregate() const { return true; }

// -- Friends
	//friend ostream& operator<<(ostream& s,const FunctionAVG& p)
	//	{ p.print(s); return s; }

};

} // namespace function
} // namespace expression 
} // namespace sql
} // namespace odb

#endif
