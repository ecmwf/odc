// File StringExpression.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef StringExpression_H
#define StringExpression_H

#include "SQLExpression.h"

namespace odb {
namespace sql {
namespace expression {

class StringExpression : public SQLExpression {
public:
	StringExpression(const string&);
	StringExpression(const StringExpression&);
	~StringExpression(); // Change to virtual if base class

	SQLExpression* clone() const;

private:
// No copy allowed
	StringExpression& operator=(const StringExpression&);

	string name_;
	double value_;

// -- Overridden methods
	virtual void print(ostream& s) const;
	virtual void expandStars(const std::vector<SQLTable*>&, expression::Expressions&);
	virtual void prepare(SQLSelect& sql);
	virtual void cleanup(SQLSelect& sql);

	virtual double eval(bool& missing) const;
	virtual bool isConstant() const { return true; }
	virtual bool isNumber() const { return true; }
	virtual void output(ostream& s) const;
};

} // namespace expression
} // namespace sql
} // namespace odb 

#endif
