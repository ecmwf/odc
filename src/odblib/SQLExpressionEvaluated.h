/// \file SQLExpressionEvaluated.h
/// Piotr Kuchta - ECMWF Nov 11

#ifndef SQLExpressionEvaluated_H
#define SQLExpressionEvaluated_H

#include <machine.h>

#include "SQLType.h"
#include "SQLExpression.h"
#include "Exceptions.h"

namespace odb {
namespace sql {
namespace expression {

class SQLExpressionEvaluated : public SQLExpression {
public:
	SQLExpressionEvaluated(SQLExpression&);
	~SQLExpressionEvaluated(); // Change to virtual if base class

	// Overriden

	virtual void prepare(SQLSelect&) { NOTIMP; }
	virtual void cleanup(SQLSelect&) { NOTIMP; }
	virtual double eval(bool& missing) const { if (missing_) missing = true; return value_; }
	virtual bool isConstant() const { NOTIMP; /*?*/ return true; }
	virtual bool isNumber() const { NOTIMP; /**/ return false; }
	virtual SQLExpression* simplify(bool&) { NOTIMP; return 0; }
	virtual SQLExpression* clone() const { NOTIMP; return 0; }
	virtual bool isAggregate() const { NOTIMP; return false; }

	virtual const odb::sql::type::SQLType* type() const { return type_; }

	virtual void output(SQLOutput& o) const { type_->output(o, value_, missing_); }

protected:
	virtual void print(ostream&) const;

private:
	SQLExpressionEvaluated(const SQLExpressionEvaluated&);
	SQLExpressionEvaluated& operator=(const SQLExpressionEvaluated&);

	friend ostream& operator<<(ostream& s, const SQLExpressionEvaluated& p) { p.print(s); return s; }

	const odb::sql::type::SQLType* type_;
	bool missing_;
	double value_;
	double missingValue_;
};

} // namespace expression
} // namespace sql
} // namespace odb

using namespace odb::sql::expression;

#endif
