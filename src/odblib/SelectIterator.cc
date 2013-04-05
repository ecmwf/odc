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


#include "odblib/oda.h"
#include "odblib/SQLSelectFactory.h"

using namespace eclib;

namespace odb {

SelectIterator::SelectIterator(Select &owner)
: owner_(owner),
  select_(),
  session_(*this),
  selectStmt_(0),
  metaData_(0),
  data_(0),
  newDataset_(true),
  noMore_(false),
  aggregateResultRead_(false),
  isCachingRows_(false),
  refCount_(0)
{
}

SelectIterator::SelectIterator(Select &owner, std::string select)
: owner_(owner),
  select_(select),
  session_(*this),
  selectStmt_(0),
  metaData_(0),
  data_(0),
  newDataset_(true),
  noMore_(false),
  aggregateResultRead_(false),
  isCachingRows_(false),
  refCount_(0)
{
	if (owner.dataIStream())
		parse(owner.dataIStream());
	else
		// TODO: SelectIterator should be templatizec with DATASTREAM and the parameter used below
		parse<DataStream<SameByteOrder, DataHandle> >(owner.dataHandle());
}

template <typename DATASTREAM> 
void SelectIterator::parse(typename DATASTREAM::DataHandleType *dh)
{
	sql::SQLParser p;
	p.parseString(select_, dh, odb::sql::SQLSelectFactory::instance().config());
	sql::SQLStatement *stmt = session_.statement();
	selectStmt_ = dynamic_cast<sql::SQLSelect*>(stmt);
	ASSERT(selectStmt_);

	//Log::info() << "SelectIterator::parse: " << *selectStmt_ << endl;

	selectStmt_->prepareExecute();
	
	populateMetaData<DATASTREAM>();

	selectStmt_->env.pushFrame(selectStmt_->sortedTables_.begin());
}

void SelectIterator::parse(std::istream *is)
{
	sql::SQLParser p;
	odb::sql::SQLSelectFactory& factory(odb::sql::SQLSelectFactory::instance());
	p.parseString(select_, is, factory.config(), factory.csvDelimiter());
	sql::SQLStatement *stmt = session_.statement();
	selectStmt_ = dynamic_cast<sql::SQLSelect*>(stmt);
	ASSERT(selectStmt_);
	selectStmt_->prepareExecute();
	
	populateMetaData<DataStream<SameByteOrder, DataHandle> >();

	selectStmt_->env.pushFrame(selectStmt_->sortedTables_.begin());
}

SelectIterator::~SelectIterator()
{
	delete [] data_;
	delete selectStmt_;
	delete metaData_ ;
}

void SelectIterator::cacheRow(const Expressions& results)
{
	size_t n = results.size();
	vector<double> v(n);
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
			vector<double>& r(rowCache_.front());
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
			vector<double>& r(rowCache_.front());
			//for (size_t i = 0; i < r.size(); ++i) data_[i] = r[i];
			copy(r.begin(), r.end(), data_);
			rowCache_.pop_front();
			return true;
		}
	}
	
	return rc && !aggregateResultRead_;
}

bool SelectIterator::isNewDataset() { return newDataset_; }

const double* SelectIterator::data() { return data_; }

MetaData& SelectIterator::columns()
{
	ASSERT(metaData_);
	return *metaData_;
}

template <typename DATASTREAM>
void SelectIterator::populateMetaData()
{
	newDataset_ = true;
	Expressions &results_ = selectStmt_->results_;
	delete metaData_;
	metaData_ = new MetaData(results_.size());
	for (size_t i = 0; i < results_.size(); i++)
	{
		Column* col = new Column(*metaData_);
		(*metaData_)[i] = col;
		sql::expression::SQLExpression *exp = results_[i];
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
				Log::error() << "Unknown type: " << *sqlType << ", kind: " << kind << endl;
				ASSERT(!"UnknownType");
				break;
		}

		col->hasMissing(exp->hasMissingValue());
		col->missingValue(exp->missingValue());
		col->bitfieldDef(exp->bitfieldDef());
	}
	delete [] data_;
	data_ = new double[metaData_->size()];
	for (size_t i = 0; i < metaData_->size(); ++i)
		data_[i] = (*metaData_)[i]->missingValue();
}

} // namespace odb 
