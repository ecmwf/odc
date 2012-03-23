/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// zfile TODATable.h
/// Piotr Kuchta - ECMWF Oct 2010

#ifndef TODATable_H
#define TODATable_H

#include "odblib/SQLTable.h"
#include "odblib/TODATableIterator.h"

namespace odb {
namespace sql {

template <typename T>
class TODATable : public SQLTable {
public:

	typedef T TReader;
	typedef TODATableIterator<TODATable> TableIterator;

	TODATable(SQLDatabase&, const PathName&, const string&);
	TODATable(SQLDatabase&, DataHandle&);
	TODATable(SQLDatabase&, istream&);

	~TODATable(); 

	double value(long);

// -- Overridden methods
	bool hasColumn(const string&, string* fullName = 0);
	SQLColumn* column(const string&);

protected:
	// void print(ostream&) const;
	SQLColumn* createSQLColumn(const type::SQLType& type, const string& name, int index, bool hasMissingValue, double missingValue, bool isBitfield, const BitfieldDef&);

private:

// No copy allowed
	TODATable(const TODATable&);
	TODATable& operator=(const TODATable&);

// -- Members

public:

	double* data_;
	typename TODATable<T>::TReader oda_;
	typename TODATable<T>::TReader::iterator reader_;
	typename TODATable<T>::TReader::iterator end_;

// -- Methods

	void populateMetaData();
	void updateMetaData(const vector<SQLColumn*>&);

	virtual SQLTableIterator* iterator(const vector<SQLColumn*>&) const;
};

} // namespace sql 
} // namespace odb 

#include "TODATable.cc"

#endif
