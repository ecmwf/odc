/// \file FunctionRGG_BOXLAT.h
/// ECMWF July 2010

#ifndef FunctionRGG_BOXLAT_H
#define FunctionRGG_BOXLAT_H

#include "FunctionExpression.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

class FunctionRGG_BOXLAT : public FunctionExpression {
public:
	FunctionRGG_BOXLAT(const string&, const expression::Expressions&);
	FunctionRGG_BOXLAT(const FunctionRGG_BOXLAT&);
	~FunctionRGG_BOXLAT(); // Change to virtual if base class

	SQLExpression* clone() const;
private:
// No copy allowed
	FunctionRGG_BOXLAT& operator=(const FunctionRGG_BOXLAT&);

// -- Overridden methods
	virtual double eval(bool& missing) const;

// -- Friends
	//friend ostream& operator<<(ostream& s,const FunctionRGG_BOXLAT& p)
	//	{ p.print(s); return s; }

};

} // namespace function
} // namespace expression 
} // namespace sql
} // namespace odb 

#endif
