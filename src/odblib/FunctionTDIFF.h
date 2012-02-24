// File FunctionTDIFF.h
// ECMWF July 2010

#ifndef FunctionTDIFF_H
#define FunctionTDIFF_H

#include "FunctionExpression.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

class FunctionTDIFF : public FunctionExpression {
public:
	FunctionTDIFF(const string&,const expression::Expressions&);
	FunctionTDIFF(const FunctionTDIFF&);
	~FunctionTDIFF(); // Change to virtual if base class

// -- Overridden methods
	virtual const odb::sql::type::SQLType* type() const;
	virtual void output(ostream& s) const;

	SQLExpression* clone() const;

private:
// No copy allowed
	FunctionTDIFF& operator=(const FunctionTDIFF&);

// -- Overridden methods
	virtual double eval(bool& missing) const;

// -- Friends
	//friend ostream& operator<<(ostream& s,const FunctionTDIFF& p)
	//	{ p.print(s); return s; }
};

} // namespace function
} // namespace expression 
} // namespace sql
} // namespace odb

#endif
