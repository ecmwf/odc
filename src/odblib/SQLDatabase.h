/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File SQLDatabase.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef SQLDatabase_H
#define SQLDatabase_H

#include "eckit/filesystem/PathName.h"

#include "odblib/SQLTable.h"
#include "odblib/SchemaAnalyzer.h"

namespace eckit { class DataHandle; }

namespace odb {

class DataTable;

namespace sql {
// Forward declarations

class SQLStatement;
namespace expression { class SQLExpression; }
using namespace expression;

typedef map<string, set<string> > Links;
typedef map<string, SQLExpression*> Variables;

class SQLDatabase {
public:
	typedef enum { ODA, CSV } DataFormat;

	SQLDatabase(const eckit::PathName&,const string&);
	SQLDatabase(const string& = "default");
	virtual ~SQLDatabase(); 

// -- Methods
	virtual void open();
	virtual void close();

	virtual SQLTable* table(const string&);
	virtual SQLTable* openDataHandle(eckit::DataHandle&, DataFormat = ODA) = 0; 
	virtual SQLTable* openDataStream(istream&, const string& delimiter, DataFormat = CSV) = 0; 
	virtual void addTable(SQLTable *table) { tablesByName_[table->name()] = table; }

	void setLinks(const Links&);
	void setLinks() { setLinks(links_); }

	void addLinks(const Links& ls) { links_.insert(ls.begin(), ls.end()); }
	Links& links() { return links_; }

	virtual const string& name() const { return name_; }

	SQLExpression* getVariable(const string&) const;
	void   setVariable(const string&, SQLExpression*);
	Variables& variables() { return variables_; }

	virtual bool sameAs(const SQLDatabase& other) const;
	SchemaAnalyzer& schemaAnalyzer() { return schemaAnalyzer_; }

        void setIncludePath(const std::string& includePath);
        const std::vector<eclib::PathName>& includePath() const { return includePath_; }

    SQLTable* dualTable();

protected:
	Links links_;
	map<string,SQLTable*> tablesByName_;

    eckit::PathName path_;
    std::vector<eckit::PathName> includePath_;

	Variables variables_;
	string name_;
	SchemaAnalyzer schemaAnalyzer_;

    DataTable* dualTable_;
    SQLTable* dual_;

private:
// No copy allowed
	SQLDatabase(const SQLDatabase&);
	SQLDatabase& operator=(const SQLDatabase&);

	void loadIOMAP();
	void loadDD();
	void loadFLAGS();

	void setUpVariablesTable();

// -- Friends
	//friend ostream& operator<<(ostream& s,const SQLDatabase& p)
	//	{ p.print(s); return s; }
};

} // namespace sql 
} // namespace odb 

#endif
