/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef ODBIterator_H
#define ODBIterator_H

#include "eckit/filesystem/PathName.h"
#include "odb_api/RowsIterator.h"
#include "odb_api/SQLInteractiveSession.h"

namespace eckit { class PathName; }

namespace odb { namespace sql { class SchemaAnalyzer; class SQLInteractiveSession; } }

namespace odb {

class MetaData;

namespace tool {

class ODBIterator : public odb::RowsReaderIterator {
public:

	//ODBIterator(const eckit::PathName& db, const std::string& sql); 
	ODBIterator(const std::string& db, const std::string& sql); 
	~ODBIterator ();

	void destroy();

	const ODBIterator& end() { return *reinterpret_cast<ODBIterator*>(0); }

	bool operator!=(const ODBIterator& o) { ASSERT(&o == 0); return hasNext_; }

	ODBIterator& operator++() { next(); return *this; }

	odb::MetaData& columns();

	virtual bool isNewDataset();
	virtual double* data();

	static eckit::PathName schemaFile(const eckit::PathName db);

	virtual bool next();

protected:
	int setColumn(unsigned long index, std::string& name, odb::ColumnType type, double missingValue);
	//virtual bool next();

private:
	void createColumns();

	eckit::PathName db_;

	void *odbHandle_;
	int noOfColumns_;
	//colinfo_t *ci_;
	void *ci_;
	odb::MetaData *columns_;
	int newDataset_;
	double* data_;
	unsigned long long count_;
	bool hasNext_;

	std::string defaultSQL(const eckit::PathName db);
	const odb::sql::SchemaAnalyzer& getSchema(const eckit::PathName db);
	bool schemaParsed_;
	odb::sql::SQLInteractiveSession session_;

	friend class FakeODBIterator;
	//friend class ReptypeGenIterator;
public:
	int refCount_;
	bool noMore_;
};

} // namespace tool 
} // namespace odb 

#endif
