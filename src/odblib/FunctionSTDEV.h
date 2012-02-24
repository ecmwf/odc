// File FunctionSTDEV.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef FunctionSTDEV_H
#define FunctionSTDEV_H

#include "FunctionVAR.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

class FunctionSTDEV : public FunctionVAR {
public:
	FunctionSTDEV(const string&, const expression::Expressions&);
	FunctionSTDEV(const FunctionSTDEV&);
	~FunctionSTDEV(); // Change to virtual if base class

	SQLExpression* clone() const;

private:
// No copy allowed
	FunctionSTDEV& operator=(const FunctionSTDEV&);

	virtual double eval(bool& missing) const;

// -- Friends
	//friend ostream& operator<<(ostream& s,const FunctionSTDEV& p)
	//	{ p.print(s); return s; }
};

} // namespace function
} // namespace expression 
} // namespace sql
} // namespace odb

#endif
