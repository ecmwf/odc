/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File VariablesTable.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef VariablesTable_H
#define VariablesTable_H

//#include "eckit/filesystem/PathName.h"

//#include "odblib/SQLBitfield.h"
#include "odblib/SQLTable.h"

namespace odb {
namespace sql {

//class SQLFile;
class SQLPool;
class SQLColumn;
class SQLDatabase;
namespace expression {
class SQLExpression;
}

class VariablesTableIterator : public SQLTableIterator {
    typedef std::map<std::string,expression::SQLExpression*> Variables;
public:
    VariablesTableIterator(std::map<std::string,expression::SQLExpression*>&);
	~VariablesTableIterator();
	void rewind();
	bool next();
private:
	Variables& variables_;
	Variables::iterator it_;
    double data_[2];
    bool missing_[2];
};

typedef std::vector<std::string> ColumnNames;

class VariablesTable : public SQLTable {
public:
	VariablesTable(SQLDatabase&,const std::string&);
	~VariablesTable(); 

    SQLColumn* createSQLColumn(const type::SQLType& type, const std::string& name, int index, bool hasMissingValue, double missingValue, const BitfieldDef&);
    SQLColumn* createSQLColumn(const type::SQLType& type, const std::string& name, int index, bool hasMissingValue, double missingValue);

	SQLTableIterator* iterator(const std::vector<SQLColumn*>&) const;

private:
    
	friend std::ostream& operator<<(std::ostream& s,const VariablesTable& p)
		{ p.print(s); return s; }
};

} // namespace sql
} // namespace odb

#endif
