// File FunctionCOUNT.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef FunctionCOUNT_H
#define FunctionCOUNT_H

#include "FunctionExpression.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

class FunctionCOUNT : public FunctionExpression {
public:
	FunctionCOUNT(const string&,const expression::Expressions&);
	FunctionCOUNT(const FunctionCOUNT&);
	~FunctionCOUNT();

	SQLExpression* clone() const;

private:
// No copy allowed
	FunctionCOUNT& operator=(const FunctionCOUNT&);

	unsigned long long count_;

// -- Overridden methods
	virtual void prepare(SQLSelect&);
	virtual void cleanup(SQLSelect&);
	virtual void partialResult();
	virtual double eval(bool& missing) const;

	bool isAggregate() const { return true; }

// -- Friends
	//friend ostream& operator<<(ostream& s,const FunctionCOUNT& p)
	//	{ p.print(s); return s; }
};

} // namespace function
} // namespace expression 
} // namespace sql
} // namespace odb

#endif
