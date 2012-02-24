// File NumberExpression.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef NumberExpression_H
#define NumberExpression_H

#include "SQLExpression.h"

namespace odb {
namespace sql {
namespace expression {

class NumberExpression : public SQLExpression {
public:
	NumberExpression(double value);
	NumberExpression(const NumberExpression&);
	~NumberExpression();

	SQLExpression* clone() const;

	void value(double v) { value_ = v; }

private:
// No copy allowed
	NumberExpression& operator=(const NumberExpression&);

	double value_;

// -- Overridden methods
	virtual void print(ostream& s) const;
	virtual void prepare(SQLSelect& sql);
	virtual void cleanup(SQLSelect& sql);

	virtual double eval(bool& missing) const;
	virtual bool isConstant() const { return true; }
	virtual bool isNumber() const { return true; }
};

} // namespace expression 
} // namespace sql
} // namespace odb 

#endif
