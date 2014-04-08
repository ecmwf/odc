/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

///
/// \file SelectIterator.h
///
/// @author Piotr Kuchta, Feb 2009

#ifndef _SelectIterator_H
#define _SelectIterator_H

#include "odblib/Expressions.h"
#include "odblib/RowsIterator.h"
#include "odblib/SQLIteratorSession.h"

extern "C" {
	typedef void oda_select_iterator;
	int odb_select_iterator_get_next_row(oda_select_iterator*, int, double*, int*);
}

namespace odb {

class Select;
class MetaData;

template <typename I, typename O, typename D> class IteratorProxy;

namespace sql {
	template <typename T> class SQLIteratorSession;
	template <typename T> class SQLIteratorOutput;
	class SQLSelect;
}

class SelectIterator { //: public RowsReaderIterator {
public:
    SelectIterator (Select &owner, std::string select);
	~SelectIterator();

	virtual bool isNewDataset();
	virtual const double* data();

	const MetaData& columns();
	const MetaData& columns(const MetaData&) { NOTIMP; }
    void setNumberOfColumns(size_t) { NOTIMP; }

	int close() { NOTIMP; }
    int setColumn(size_t index, std::string name, ColumnType type) { NOTIMP; }
	void writeHeader() { NOTIMP; }
    int setBitfieldColumn(size_t index, std::string name, ColumnType type, BitfieldDef b) { NOTIMP; }
	void missingValue(size_t, double) { NOTIMP; }
	
	bool isCachingRows() { return isCachingRows_; }
	void cacheRow(const Expressions& results);
	double& data(size_t i);

protected:
	virtual bool next();

private:
// No copy allowed.
	SelectIterator(const SelectIterator&);
	SelectIterator& operator=(const SelectIterator&);

	template <typename DATASTREAM> void populateMetaData();
	template <typename DATASTREAM> void parse(typename DATASTREAM::DataHandleType *);
	void parse(std::istream *);

	Select& owner_;
    std::string select_;
	sql::SQLIteratorSession<SelectIterator> session_;
	sql::SQLSelect *selectStmt_;
	MetaData *metaData_;

	double* data_;
	bool newDataset_;

	bool noMore_;
	bool aggregateResultRead_;
	bool isCachingRows_;
	std::list<std::vector<double> > rowCache_;

protected:
	SelectIterator (Select &owner);

	int refCount_;

	friend int ::odb_select_iterator_get_next_row(::oda_select_iterator*, int, double*, int*);
	friend class odb::Select;
	friend class odb::sql::SQLIteratorOutput<SelectIterator>;
	friend class odb::IteratorProxy<odb::SelectIterator, odb::Select, const double>;
};

} // namespace odb 

#endif
