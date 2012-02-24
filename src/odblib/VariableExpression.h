// File VariableExpression.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef VariableExpression_H
#define VariableExpression_H

#include "SQLExpression.h"

namespace odb {
namespace sql {
namespace expression {

class VariableExpression : public SQLExpression {
public:
	VariableExpression(const string&);
	VariableExpression(const VariableExpression&);
	~VariableExpression(); // Change to virtual if base class

	SQLExpression* clone() const;

private:
// No copy allowed
	VariableExpression& operator=(const VariableExpression&);

	SQLExpression* value_;
	string name_;

// -- Overridden methods
	virtual void print(ostream& s) const;
	virtual void prepare(SQLSelect& sql);
	virtual void cleanup(SQLSelect& sql);

	virtual double eval(bool& missing) const;

	virtual bool isConstant() const;
	virtual bool isVector() const;
};

} // namespace expression
} // namespace sql
} // namespace odb

#endif
