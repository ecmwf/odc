// File ParameterExpression.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef ParameterExpression_H
#define ParameterExpression_H

#include "SQLExpression.h"

namespace odb {
namespace sql {
namespace expression {

class ParameterExpression : public SQLExpression {
public:
	ParameterExpression(int);
	ParameterExpression(const ParameterExpression&);
	~ParameterExpression(); // Change to virtual if base class

	SQLExpression* clone() const;

private:
// No copy allowed
	ParameterExpression& operator=(const ParameterExpression&);

// -- Members
	// None
	double value_;
	int    which_;

	virtual void print(ostream& s) const;
	virtual void prepare(SQLSelect& sql);
	virtual void cleanup(SQLSelect& sql);

	virtual double eval(bool& missing) const;
	virtual bool isConstant() const;
};

} // namespace expression 
} // namespace sql 
} // namespace odb 

#endif
