/// \file FunctionEQ_BOXLAT.h
/// ECMWF July 2010

#ifndef FunctionEQ_BOXLAT_H
#define FunctionEQ_BOXLAT_H

#include "FunctionExpression.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

class FunctionEQ_BOXLAT : public FunctionExpression {
public:
	FunctionEQ_BOXLAT(const string&, const expression::Expressions&);
	FunctionEQ_BOXLAT(const FunctionEQ_BOXLAT&);
	~FunctionEQ_BOXLAT();

	SQLExpression* clone() const;

private:
// No copy allowed
	FunctionEQ_BOXLAT& operator=(const FunctionEQ_BOXLAT&);

// -- Overridden methods
	virtual double eval(bool& missing) const;

// -- Friends
	//friend ostream& operator<<(ostream& s,const FunctionEQ_BOXLAT& p)
	//	{ p.print(s); return s; }

};

} // namespace function
} // namespace expression 
} // namespace sql
} // namespace odb 

#endif
