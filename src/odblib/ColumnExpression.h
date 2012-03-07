/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File ColumnExpression.h
// Baudouin Raoult - ECMWF Dec 03
#ifndef ColumnExpression_H
#define ColumnExpression_H

#include "SQLExpression.h"
#include "SQLType.h"

namespace odb {
namespace sql {

class SQLOutput;

namespace expression {

class ColumnExpression : public SQLExpression {
public:
	ColumnExpression(const string&, SQLTable*, int begin = -1, int end = -1);
	ColumnExpression(const string&, const string& tableReference, int begin = -1, int end = -1);
	ColumnExpression(const ColumnExpression&);

	~ColumnExpression(); // Change to virtual if base class

	SQLTable* table() { return table_; }

	double* current() { return &(value_->first); }

	SQLExpression* clone() const;

// -- Overridden methods
	// None

protected:
	const type::SQLType*   type_;
	pair<double,bool>*     value_;
	string                 columnName_;
	SQLTable*              table_;
	string                 tableReference_;
	int                    beginIndex_;
	int                    endIndex_;

// -- Methods
	
// -- Overridden methods

	virtual void print(ostream& s) const;
	virtual void prepare(SQLSelect& sql);
	virtual void cleanup(SQLSelect& sql);
	virtual double eval(bool& missing) const;
	virtual bool isConstant() const { return false; }
	virtual void output(SQLOutput& s) const;

private:
	ColumnExpression& operator=(const ColumnExpression&);

// -- Overridden methods
	virtual const type::SQLType* type() const { return type_; }
	virtual void expandStars(const std::vector<SQLTable*>&, Expressions&);
	virtual void tables(set<SQLTable*>&);
	virtual bool indexed();
	virtual SQLIndex* getIndex(double*);

	//friend ostream& operator<<(ostream& s,const ColumnExpression& p)
	//	{ p.print(s); return s; }
};

} // namespace expression 
} // namespace sql
} // namespace odb 

#endif
