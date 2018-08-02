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
/// \file SelectIterator.cc
///
/// @author Piotr Kuchta, Feb 2009

#include "odb_api/DataStream.h"
#include "odb_api/MetaData.h"
#include "odb_api/Select.h"
#include "odb_api/SelectIterator.h"
#include "eckit/sql/SQLParser.h"
#include "eckit/sql/SQLSelectFactory.h"
#include "eckit/sql/SQLSelect.h"

using namespace eckit;

namespace odb {

SelectIterator::SelectIterator(odb::Select &owner, eckit::sql::SQLSession& s)
: owner_(owner),
  select_(),
  selectStmt_(0),
  metaData_(0),
  data_(0),
  columnOffsets_(0),
  rowDataSizeDoubles_(0),
  newDataset_(true),
  noMore_(false),
  aggregateResultRead_(false),
  isCachingRows_(false),
  refCount_(0),
  session_(s)
{}

SelectIterator::SelectIterator(odb::Select& owner, const std::string& select, eckit::sql::SQLSession& s)
: owner_(owner),
  select_(select),
  selectStmt_(0),
  metaData_(0),
  data_(0),
  columnOffsets_(0),
  rowDataSizeDoubles_(0),
  newDataset_(true),
  noMore_(false),
  aggregateResultRead_(false),
  isCachingRows_(false),
  refCount_(0),
  session_(s)
{
    if (owner.dataIStream())
        parse(session_, owner.dataIStream());
    else
        parse<DataStream<SameByteOrder, DataHandle> >(session_, owner.dataHandle());
}

template <typename DATASTREAM> 
void SelectIterator::parse(eckit::sql::SQLSession& session, typename DATASTREAM::DataHandleType *dh)
{
    sql::SQLParser p;
    NOTIMP;
    // TODO: Add implicit table
//    p.parseString(session, select_, dh, session.selectFactory().config());
    p.parseString(session, select_);
    sql::SQLStatement& stmt (session_.statement());

    selectStmt_ = dynamic_cast<sql::SQLSelect*>(&stmt);
    if (! selectStmt_)
        throw UserError(std::string("Expected SELECT, got: ") + select_);

    selectStmt_->prepareExecute();
	
    populateMetaData<DATASTREAM>();

    NOTIMP;
//    selectStmt_->env.pushFrame(selectStmt_->sortedTables_.begin());
}

void SelectIterator::parse(eckit::sql::SQLSession& session, std::istream *is)
{
	sql::SQLParser p;
//    eckit::sql::SQLSelectFactory& factory(session.selectFactory());
//	p.parseString(session, select_, is, factory.config(), factory.csvDelimiter());
    // TODO: Add implicit table
    NOTIMP;
    p.parseString(session, select_);
    sql::SQLStatement& stmt (session_.statement());
    selectStmt_ = dynamic_cast<sql::SQLSelect*>(&stmt);
	ASSERT(selectStmt_);
	selectStmt_->prepareExecute();
	
	populateMetaData<DataStream<SameByteOrder, DataHandle> >();

    NOTIMP;
//	selectStmt_->env.pushFrame(selectStmt_->sortedTables_.begin());
}

SelectIterator::~SelectIterator()
{
    delete [] data_;
    delete [] columnOffsets_;
    delete selectStmt_;
    delete metaData_ ;
}

void SelectIterator::cacheRow(const Expressions& results)
{
    size_t n = results.size();
    std::vector<double> v(n);
    bool missing = false;
    for(size_t i = 0; i < n; i++)
        v[i] = results[i]->eval(missing = false);
    rowCache_.push_back(v);
}

bool SelectIterator::next()
{
	newDataset_ = false;
	if (noMore_) return false;

	if (isCachingRows_)
	{
		if (rowCache_.size() == 0)
		{
			noMore_ = true;
			return false;
		}
		else
		{
			noMore_ = false;
			std::vector<double>& r(rowCache_.front());
			//for (size_t i = 0; i < r.size(); ++i) data_[i] = r[i];
			copy(r.begin(), r.end(), data_);
	
			rowCache_.pop_front();
			return true;
		}
	}

	if (aggregateResultRead_) return noMore_ = true;

    bool rc = selectStmt_->processOneRow();

	if (!rc)
	{
		if (selectStmt_->isAggregate())
			aggregateResultRead_ = true;
		else
			noMore_ = true;

		isCachingRows_ = true;
        selectStmt_->postExecute();
		//isCachingRows_ = false; // this would be needed if we reuse the same iterator for several queries. 
		
		if (rowCache_.size())
		{
			noMore_ = false;
			std::vector<double>& r(rowCache_.front());
			//for (size_t i = 0; i < r.size(); ++i) data_[i] = r[i];
			copy(r.begin(), r.end(), data_);
			rowCache_.pop_front();
			return true;
		}
	}
	
	return rc && !aggregateResultRead_;
}

bool SelectIterator::isNewDataset() { return newDataset_; }

double& SelectIterator::data(size_t i)
{
    ASSERT(data_ && columnOffsets_);
	ASSERT(i >= 0 && i < columns().size());
    return data_[columnOffsets_[i]];
}

const MetaData& SelectIterator::columns() const
{
	ASSERT(metaData_);
	return *metaData_;
}

template <typename DATASTREAM>
void SelectIterator::populateMetaData()
{
    Expressions results = selectStmt_->output();
	delete metaData_;
    metaData_ = new MetaData(results.size());
    for (size_t i = 0; i < results.size(); i++)
	{
		Column* col = new Column(*metaData_);
		(*metaData_)[i] = col;
        std::shared_ptr<sql::expression::SQLExpression> exp = results[i];
		std::string title = exp->title();
		col->name(title);

		const sql::type::SQLType* sqlType = exp->type();
		int kind = sqlType->getKind();
		switch (kind) {
			case sql::type::SQLType::realType:
				col->type<DATASTREAM>(REAL, false); //FIXME
				break;
			case sql::type::SQLType::doubleType:
				col->type<DATASTREAM>(DOUBLE, false); //FIXME
				break;
			case sql::type::SQLType::integerType:
				col->type<DATASTREAM>(INTEGER, false); //FIXME
				break;
			case sql::type::SQLType::stringType:
				col->type<DATASTREAM>(STRING, false); // FIXME
				break;
			case sql::type::SQLType::bitmapType:
				col->type<DATASTREAM>(BITFIELD, false);
				break;
			case sql::type::SQLType::blobType:
				NOTIMP;
				break;
			default:
				Log::error() << "Unknown type: " << *sqlType << ", kind: " << kind << std::endl;
				ASSERT(!"UnknownType");
				break;
		}

		col->hasMissing(exp->hasMissingValue());
		col->missingValue(exp->missingValue());
		col->bitfieldDef(exp->bitfieldDef());
	}

    rowDataSizeDoubles_ = rowDataSizeDoublesInternal();

	delete [] data_;
    data_ = new double[rowDataSizeDoubles()];

    delete [] columnOffsets_;
    columnOffsets_ = new size_t[metaData_->size()];

    size_t offset = 0;
    for (size_t i = 0; i < metaData_->size(); ++i) {
        data_[offset] = (*metaData_)[i]->missingValue();
        columnOffsets_[i] = offset;
        offset += (*metaData_)[i]->dataSizeDoubles();
    }
	newDataset_ = true;
}

size_t SelectIterator::rowDataSizeDoublesInternal() const {

    size_t total = 0;
    for (const auto& column : columns()) {
        total += column->dataSizeDoubles();
    }
    return total;
}



} // namespace odb 
