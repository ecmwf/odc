// File FunctionNORM.h
// ECMWF July 2010

#ifndef FunctionNORM_H
#define FunctionNORM_H

#include "FunctionExpression.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

class FunctionNORM : public FunctionExpression {
public:
	FunctionNORM(const string&,const expression::Expressions&);
	FunctionNORM(const FunctionNORM&);
	~FunctionNORM();

	SQLExpression* clone() const;
private:
// No copy allowed
	FunctionNORM& operator=(const FunctionNORM&);

	double value_;

// -- Overridden methods
	virtual void prepare(SQLSelect&);
	virtual void cleanup(SQLSelect&);
	virtual void partialResult();
	virtual double eval(bool& missing) const;

	bool isAggregate() const { return true; }
	bool resultNULL_;
// -- Friends
	//friend ostream& operator<<(ostream& s,const FunctionNORM& p)
	//	{ p.print(s); return s; }
};

} // namespace function
} // namespace expression 
} // namespace sql
} // namespace odb

#endif
