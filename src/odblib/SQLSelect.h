/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File SQLSelect.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef SQLSelect_H
#define SQLSelect_H

#include <stack>

#include "eckit/eckit.h"

#include "odblib/Environment.h"
#include "odblib/FunctionROWNUMBER.h"
#include "odblib/FunctionTHIN.h"
#include "odblib/SQLColumn.h"
#include "odblib/SQLExpression.h"
#include "odblib/SQLOutput.h"
#include "odblib/SQLOutputConfig.h"
#include "odblib/SQLOutputConfig.h"
#include "odblib/SQLStatement.h"
#include "odblib/SelectOneTable.h"
#include "odblib/Stack.h"

namespace odb {

	class SelectIterator;

namespace sql {

class SQLTableIterator;

class SQLSelect : public SQLStatement {
	friend class odb::SelectIterator;

public:
	SQLSelect(const Expressions&, const vector<SQLTable*>&, odb::sql::expression::SQLExpression*, SQLOutput*, SQLOutputConfig);
	~SQLSelect(); 

// -- Methods
	void prepareExecute(); //SQLExpression*& where);
	unsigned long long process(odb::sql::expression::SQLExpression*,SortedTables::iterator);
	bool processOneRow();
	void postExecute();

	void pushFirstFrame();

	bool isAggregate() { return aggregate_; }

	pair<double,bool>* column(const string& name, SQLTable*);
	const type::SQLType* typeOf(const string& name, SQLTable*) const;
	// FIXME: do we really need all these optional parameters?
	SQLTable* findTable(const string& name, string *fullName = 0, bool *hasMissingValue=0, double *missingValue=0, bool* isBitfield=0, BitfieldDef* =0) const;

	virtual Expressions output() const; 

	Expressions& results() { return results_; }

// -- Overridden methods
	virtual unsigned long long execute();

protected:
	virtual void print(ostream&) const; 	

private:
// No copy allowed
	SQLSelect(const SQLSelect&);
	SQLSelect& operator=(const SQLSelect&);

// -- Members
	Expressions select_;
	vector<SQLTable*> tables_;
	SortedTables sortedTables_;

	auto_ptr<odb::sql::expression::SQLExpression> where_;
	odb::sql::expression::SQLExpression* simplifiedWhere_;

	Stack env;

	auto_ptr<SQLOutput>     output_;
	Expressions  results_;

	typedef map<vector<pair<double,bool> >, expression::Expressions*> AggregatedResults;
	AggregatedResults aggregatedResults_;

	map<string,pair<double,bool> > values_;
	set<SQLTable*>     allTables_;

	typedef map<SQLTable*,SelectOneTable> TableMap;
	TableMap tablesToFetch_;

	unsigned long long count_;
	unsigned long long total_;
	unsigned long long skips_;

	bool aggregate_;
	bool mixedAggregatedAndScalar_;
	Expressions aggregated_;
	Expressions nonAggregated_;
	vector<bool> mixedResultColumnIsAggregated_;
	SQLOutputConfig outputConfig_;
// -- Methods

	void reset();
	bool resultsOut();
	bool output(odb::sql::expression::SQLExpression*);
    SQLExpression* findAliasedExpression(const string& alias);

	friend class odb::sql::expression::function::FunctionROWNUMBER; // needs access to count_
	friend class odb::sql::expression::function::FunctionTHIN; // needs access to count_

	friend ostream& operator<<(ostream& s,const SQLSelect& p)
		{ p.print(s); return s; }
};

} // namespace sql
} // namespace odb

#endif
