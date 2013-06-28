/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File SQLTable.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef SQLTable_H
#define SQLTable_H

#include "eclib/PathName.h"

#include "odblib/SQLBitfield.h"
#include "odblib/SQLType.h"

namespace odb {
namespace sql {

//class SQLFile;
class SQLPool;
class SQLColumn;
class SQLDatabase;

class SQLTableIterator {
public:
	virtual ~SQLTableIterator() {}
	virtual void rewind() = 0;
	virtual bool next()   = 0;
};

typedef vector<string> ColumnNames;

class SQLTable {
public:
	SQLTable(SQLDatabase&,const eclib::PathName&,const string&);
	virtual ~SQLTable(); // Change to virtual if base class

	void loadIOMAP(istream&);
	void addColumn(const string&, int, const type::SQLType&, bool, double, bool, const BitfieldDef&);

	void addLinkFrom(const SQLTable*);
	bool hasLinkFrom(const SQLTable&) const;

	void addLinkTo(const SQLTable*);
	bool hasLinkTo(const SQLTable&) const;

	bool isParentOf(const SQLTable&) const;

	virtual SQLColumn* column(const string&);
	SQLPool*   pool(int);

	SQLTable* master() const;
	void master(SQLTable* master);

	//virtual bool hasColumn(const string&, string* fullName = 0, bool *hasMissingValue=0, double *missingValue=0, BitfieldDef*=0);
	virtual bool hasColumn(const string&, string* fullName = 0);//, string* fullName = 0, bool *hasMissingValue=0, double *missingValue=0, BitfieldDef*=0);

	unsigned long long noRows() const;

	ColumnNames columnNames() const;
	FieldNames bitColumnNames(const string&) const;

	const string& name()  const { return name_; }

	string fullName() const;

	SQLDatabase& owner() const { return owner_; }

	bool sameAs(const SQLTable& other) const;
	bool sameDatabase(const SQLTable& other) const;

    const eclib::PathName& path() const { return path_; }

	virtual void print(ostream& s) const;

	virtual SQLTableIterator* iterator(const vector<SQLColumn*>&) const = 0;

protected:
    eclib::PathName path_;
	string   name_;

	//map<int,SQLFile*> files_;
	map<int, SQLPool*> pools_;

	map<string, FieldNames > bitColumnNames_;
	map<string, SQLColumn*>  columnsByName_;
	map<int, SQLColumn*>     columnsByIndex_;

	set<const SQLTable*> linksFrom_;
	set<const SQLTable*> linksTo_;

// -- Methods
	void clearColumns();
	
	// void print(ostream&) const; // Change to virtual if base class	
	void addColumn(SQLColumn*, const string&, int);

	virtual SQLColumn* createSQLColumn(const type::SQLType& type, const string& name, int index, bool hasMissingValue, double
missingValue, const BitfieldDef&) = 0;
	virtual SQLColumn* createSQLColumn(const type::SQLType& type, const string& name, int index, bool hasMissingValue, double
missingValue) = 0;
private:
// No copy allowed
	SQLTable(const SQLTable&);
	SQLTable& operator=(const SQLTable&);

	SQLDatabase& owner_;
	SQLTable* master_;

	friend ostream& operator<<(ostream& s,const SQLTable& p)
		{ p.print(s); return s; }

};

} // namespace sql
} // namespace odb

#endif
