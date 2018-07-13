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

#ifndef odb_api_SelectIterator_H
#define odb_api_SelectIterator_H

#include "odb_api/Expressions.h"
#include "odb_api/ColumnType.h"
#include "odb_api/sql/SQLIteratorSession.h"

extern "C" {
	typedef void oda_select_iterator;
	int odb_select_iterator_get_next_row(oda_select_iterator*, int, double*, int*);
}

namespace odb { class Select; }
namespace odb { class MetaData; }
namespace odb { template <typename I, typename O, typename D> class IteratorProxy; } 
namespace odb { namespace sql { class SQLSession; } }
namespace odb { namespace sql { class SQLNonInteractiveSession; }}
namespace odb { namespace sql { class SQLIteratorSession; }}
namespace odb { namespace sql { class SQLSelect; } }
namespace odb { namespace sql { template <typename T> class SQLIteratorOutput; }}

namespace odb {

class SelectIterator { 
public:
	
    SelectIterator (Select &owner, const std::string&, odb::sql::SQLNonInteractiveSession&);
	~SelectIterator();

	bool isNewDataset();
    const double* data() const { return data_; }
    double* data() { return data_; }
    double& data(size_t i);

    const MetaData& columns() const;
	const MetaData& columns(const MetaData&) { NOTIMP; }
    void setNumberOfColumns(size_t) { NOTIMP; }

    const std::map<std::string, std::string>& properties() const { NOTIMP; }

    int close() { NOTIMP; }
    int setColumn(size_t index, std::string name, ColumnType type) { NOTIMP; }
	void writeHeader() { NOTIMP; }
    int setBitfieldColumn(size_t index, std::string name, ColumnType type, BitfieldDef b) { NOTIMP; }
	void missingValue(size_t, double) { NOTIMP; }
	
	bool isCachingRows() { return isCachingRows_; }
	void cacheRow(const Expressions& results);

    /// The offset of a given column in the doubles[] data array
    size_t dataOffset(size_t i) const { ASSERT(columnOffsets_); return columnOffsets_[i]; }

    // Get the number of doubles per row.
    size_t rowDataSizeDoubles() const { return rowDataSizeDoubles_; }


protected:
    bool next();
    size_t rowDataSizeDoublesInternal() const;

private:
// No copy allowed.
	SelectIterator(const SelectIterator&);
	SelectIterator& operator=(const SelectIterator&);

	template <typename DATASTREAM> void populateMetaData();
	template <typename DATASTREAM> void parse(odb::sql::SQLSession&, typename DATASTREAM::DataHandleType *);
	void parse(odb::sql::SQLSession&, std::istream *);

	Select& owner_;
    std::string select_;
	odb::sql::SQLSelect *selectStmt_;
	MetaData *metaData_;

	double* data_;
    size_t* columnOffsets_; // in doubles
    size_t rowDataSizeDoubles_;
    bool newDataset_;

	bool noMore_;
	bool aggregateResultRead_;
	bool isCachingRows_;
	std::list<std::vector<double> > rowCache_;

protected:
	SelectIterator (Select &owner, odb::sql::SQLNonInteractiveSession&);

	int refCount_;
	odb::sql::SQLIteratorSession session_;

	friend int ::odb_select_iterator_get_next_row(::oda_select_iterator*, int, double*, int*);
	friend class odb::Select;
	friend class odb::sql::SQLIteratorOutput<SelectIterator>;
	friend class odb::IteratorProxy<odb::SelectIterator, odb::Select, const double>;
};

} // namespace odb 

#endif
