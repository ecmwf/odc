/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File ConstantExpression.h
// Piotr Kuchta - ECMWF Oct 11

#ifndef ConstantExpression_H
#define ConstantExpression_H

#include "eckit/eckit.h"

#include "eckit/exception/Exceptions.h"

#include "odblib/SQLType.h"
#include "odblib/SQLExpression.h"
#include "odblib/SQLBitfield.h"

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
	virtual ~ConstantExpression(); 

	virtual void prepare(SQLSelect&) { NOTIMP; }
	virtual void cleanup(SQLSelect&) { NOTIMP; }

	// -- For WHERE
	virtual double eval(bool& missing) const { missing = missing_; return value_; }

	virtual bool andSplit(expression::Expressions&) { return false; }
	virtual void tables(std::set<SQLTable*>&) {}

	virtual bool isConstant() const { return true; }
	virtual bool isNumber() const { NOTIMP; return false; }

	//virtual SQLExpression* simplify(bool&);
	//virtual void title(const std::string&);
	//virtual std::string title() const;

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
	virtual void print(std::ostream&) const { NOTIMP; }; 

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
