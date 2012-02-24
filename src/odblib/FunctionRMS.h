// File FunctionRMS.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef FunctionRMS_H
#define FunctionRMS_H

#include "FunctionExpression.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

class FunctionRMS : public FunctionExpression {
public:
	FunctionRMS(const string&,const expression::Expressions&);
	FunctionRMS(const FunctionRMS&);
	~FunctionRMS(); // Change to virtual if base class

// -- Overridden methods
	virtual double eval(bool& missing) const;

	SQLExpression* clone() const;

private:
// No copy allowed
	FunctionRMS& operator=(const FunctionRMS&);

	unsigned long long count_;
	double squares_;

// -- Overridden methods
	virtual void prepare(SQLSelect&);
	virtual void cleanup(SQLSelect&);
	virtual void partialResult();

	bool isAggregate() const { return true; }

// -- Friends
	//friend ostream& operator<<(ostream& s,const FunctionRMS& p)
	//	{ p.print(s); return s; }
};

} // namespace function
} // namespace expression 
} // namespace sql
} // namespace odb 

#endif
