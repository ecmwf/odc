// File ConstantExpression.h
// Piotr Kuchta - ECMWF Oct 11

#ifndef ConstantExpression_H
#define ConstantExpression_H

#include <machine.h>

#include "SQLType.h"
#include "SQLExpression.h"
#include "SQLBitfield.h"
#include "Exceptions.h"

namespace odb {
namespace sql {
// Forward declarations

class SQLSelect;
class SQLTable;
class SQLIndex;
class SQLOutput;

namespace expression {

class ConstantExpression : public SQLExpression {
public:
	ConstantExpression(double, bool, const odb::sql::type::SQLType*);
	virtual ~ConstantExpression(); // Change to virtual if base class

	virtual void prepare(SQLSelect&) { NOTIMP; }
	virtual void cleanup(SQLSelect&) { NOTIMP; }

	// -- For WHERE
	virtual double eval(bool& missing) const { missing = missing_; return value_; }

	virtual bool andSplit(expression::Expressions&) { return false; }
	virtual void tables(set<SQLTable*>&) {}

	virtual bool isConstant() const { return true; }
	virtual bool isNumber() const { NOTIMP; return false; }

	//virtual SQLExpression* simplify(bool&);
	//virtual void title(const string&);
	//virtual string title() const;

	virtual const odb::sql::type::SQLType* type() const;
	// ----

	virtual SQLExpression* clone() const { NOTIMP; return 0; }
	
	virtual bool isAggregate() const { return false; }
	// For select expression

	virtual void output(SQLOutput& s) const;
	virtual void partialResult() { NOTIMP; }
	virtual void expandStars(const std::vector<SQLTable*>&,expression::Expressions&) { NOTIMP; }

	virtual bool isBitfield() const { return isBitfield_; }
	BitfieldDef bitfieldDef() const { return bitfieldDef_; }
	virtual bool hasMissingValue() const { return hasMissingValue_; }
	double missingValue() const { return missingValue_; }

	virtual bool indexed()  { return false; }
	virtual bool useIndex() { return false; }
	virtual SQLIndex* getIndex(double* = 0) { return 0; }

protected:
	virtual void print(ostream&) const { NOTIMP; }; 

	bool isBitfield_;
	BitfieldDef bitfieldDef_;
	bool hasMissingValue_;
	double missingValue_;

private:
	ConstantExpression(const ConstantExpression&);
	ConstantExpression& operator=(const ConstantExpression&);

	double value_;
	bool missing_;
	const odb::sql::type::SQLType& type_;
};

} // namespace expression
} // namespace sql
} // namespace odb

using namespace odb::sql::expression;

#endif
