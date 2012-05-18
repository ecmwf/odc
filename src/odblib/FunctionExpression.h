/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File FunctionExpression.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef FunctionExpression_H
#define FunctionExpression_H

#include "eclib/ThreadSingleton.h"

#include "odblib/SQLExpression.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

class FunctionExpression : public SQLExpression {
public:
	FunctionExpression(const string&,const expression::Expressions&);
	~FunctionExpression(); // Change to virtual if base class

protected:
	string name_;
	expression::Expressions args_;
	// void print(ostream&) const; // Change to virtual if base class	

// -- Overridden methods
	virtual void print(ostream& s) const;
	virtual void prepare(SQLSelect& sql);
	virtual void cleanup(SQLSelect& sql);
	virtual bool isConstant() const;
	virtual SQLExpression* simplify(bool&);
	//virtual double eval() const;
	bool isAggregate() const;
	void partialResult();

	void tables(set<SQLTable*>&);
private:
// No copy allowed
	FunctionExpression(const FunctionExpression&);
	FunctionExpression& operator=(const FunctionExpression&);
};

class FunctionFactory {
protected:
	int    arity_;
	string name_;
	virtual FunctionExpression* make(const string&,const expression::Expressions&) { NOTIMP; return 0; } // = 0;

public:
	static FunctionFactory& instance();
	FunctionFactory() : name_("FunctionFactory"), arity_(-1) {}
	FunctionFactory(const string& name, int arity = -1);
	~FunctionFactory();
	vector<pair<string, int> >& functionsInfo();
	FunctionExpression* build(const string&,SQLExpression*);
	FunctionExpression* build(const string&,SQLExpression*,SQLExpression*);
	FunctionExpression* build(const string&,SQLExpression*,SQLExpression*,SQLExpression*);
	FunctionExpression* build(const string&,const expression::Expressions&);
};

template<class T>
class FunctionMaker : public FunctionFactory {
	virtual FunctionExpression* make(const string& name,const expression::Expressions& args)
	{ return new T(name,args); }
public:
	FunctionMaker(const string& name, int arity = -1) : FunctionFactory(name, arity) {}
};

} // namespace function
} // namespace expression 
} // namespace sql
} // namespace odb

#endif
