// File FunctionVAR.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef FunctionVAR_H
#define FunctionVAR_H

#include "FunctionExpression.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

class FunctionVAR : public FunctionExpression {
public:
	FunctionVAR(const string&,const expression::Expressions&);
	FunctionVAR(const FunctionVAR&);
	~FunctionVAR();

	SQLExpression* clone() const;
protected:

// -- Overridden methods
	virtual double eval(bool& missing) const;

private:
// No copy allowed
	FunctionVAR& operator=(const FunctionVAR&);

	unsigned long long count_;
	double value_;
	double squares_;

// -- Overridden methods
	virtual void prepare(SQLSelect&);
	virtual void cleanup(SQLSelect&);
	virtual void partialResult();

	bool isAggregate() const { return true; }

// -- Friends
	//friend ostream& operator<<(ostream& s,const FunctionVAR& p)
	//	{ p.print(s); return s; }
};

} // namespace function
} // namespace expression 
} // namespace sql
} // namespace odb 

#endif
