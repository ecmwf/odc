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
#include "odb_api/SQLTable.h"
#include "odb_api/SchemaAnalyzer.h"

namespace eckit { class DataHandle; }
namespace odb { class DataTable; }

namespace odb {
namespace sql {
// Forward declarations

class SQLStatement;
namespace expression { class SQLExpression; }


typedef std::map<std::string, std::set<std::string> > Links;
typedef std::map<std::string, expression::SQLExpression*> Variables;

class SQLDatabase {
public:
	typedef enum { ODA, CSV } DataFormat;

	SQLDatabase(const eckit::PathName&,const std::string&);
	SQLDatabase(const std::string& = "default");
	virtual ~SQLDatabase(); 

// -- Methods
	virtual void open();
	virtual void close();

    //virtual SQLTable* table(const std::string&);
    virtual SQLTable* table(const Table&);
    virtual SQLTable* defaultTable();
	virtual SQLTable* openDataHandle(eckit::DataHandle&, DataFormat = ODA) = 0; 
    virtual SQLTable* openDataStream(std::istream&, const std::string& delimiter, DataFormat = CSV) = 0;
	virtual void addTable(SQLTable *table) { tablesByName_[table->name()] = table; }

	void setLinks(const Links&);
	void setLinks() { setLinks(links_); }

	void addLinks(const Links& ls) { links_.insert(ls.begin(), ls.end()); }
	Links& links() { return links_; }

	virtual const std::string& name() const { return name_; }

    expression::SQLExpression* getVariable(const std::string&) const;
    void   setVariable(const std::string&, expression::SQLExpression*);
	Variables& variables() { return variables_; }

	virtual bool sameAs(const SQLDatabase& other) const;
	SchemaAnalyzer& schemaAnalyzer() { return schemaAnalyzer_; }

    void setIncludePath(const std::string& includePath);
    const std::vector<eckit::PathName>& includePath() const { return includePath_; }

    SQLTable* dualTable();

protected:
	Links links_;
    std::map<std::string,SQLTable*> tablesByName_;

    eckit::PathName path_;
    std::vector<eckit::PathName> includePath_;

	Variables variables_;
	std::string name_;
	SchemaAnalyzer schemaAnalyzer_;

    odb::DataTable* dualTable_;
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
	//friend std::ostream& operator<<(std::ostream& s,const SQLDatabase& p)
	//	{ p.print(s); return s; }
};

} // namespace sql 
} // namespace odb 

#endif
