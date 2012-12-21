/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File FunctionFactory.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef FunctionFactory_H
#define FunctionFactory_H

#include "eclib/ThreadSingleton.h"

#include "odblib/SQLExpression.h"
#include "odblib/FunctionExpression.h"

namespace odb {
namespace sql {
namespace expression {
namespace function {

class FunctionFactoryBase {
protected:
	int    arity_;
	string name_;
	string help_;
	virtual FunctionExpression* make(const string&,const expression::Expressions&) = 0;

public:
	//FunctionFactoryBase() : name_("FunctionFactory"), arity_(-1) {}
	FunctionFactoryBase(const string& name, int arity, const string& help);
	~FunctionFactoryBase();

	FunctionExpression* build(const string&, SQLExpression*);
	FunctionExpression* build(const string&, SQLExpression*, SQLExpression*);
	FunctionExpression* build(const string&, SQLExpression*, SQLExpression*, SQLExpression*);
	FunctionExpression* build(const string&, const expression::Expressions&);
};

class FunctionFactory : public FunctionFactoryBase {
public:
	static FunctionFactory& instance();
	FunctionFactory(); // : FunctionFactoryBase("FunctionFactory", -1) {}

	typedef vector<pair<pair<string, int>, string> > FunctionInfo;

	FunctionInfo& functionsInfo();

private:
	FunctionExpression* make(const string&,const expression::Expressions&) { NOTIMP; return 0; }


	map<pair<string,int>, FunctionFactoryBase*> map_;
	FunctionInfo functionInfo_;
};

template<class T>
class FunctionMaker : public FunctionFactoryBase {
	FunctionExpression* make(const string& name, const expression::Expressions& args)
	{ return new T(name, args); }
public:
	FunctionMaker(const string& name, int arity, const string& help) : FunctionFactoryBase(name, arity, help) {}
};

} // namespace function
} // namespace expression 
} // namespace sql
} // namespace odb

#endif
