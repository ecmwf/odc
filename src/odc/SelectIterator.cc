/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/sql/SQLParser.h"
#include "eckit/sql/SQLSelectFactory.h"
#include "eckit/sql/SQLSelect.h"

#include "odc/DataStream.h"
#include "odc/MetaData.h"
#include "odc/Select.h"
#include "odc/SelectIterator.h"
#include "odc/sql/SQLSelectOutput.h"


using namespace eckit;

namespace odc {

//----------------------------------------------------------------------------------------------------------------------

SelectIterator::SelectIterator(const std::string& select, eckit::sql::SQLSession& s, sql::SQLSelectOutput& output) :
    select_(select),
    output_(output),
    session_(s),
    refCount_(0),
    noMore_(false) {

    parse();
}

SelectIterator::~SelectIterator() {}


void SelectIterator::parse() {

    eckit::sql::SQLParser p;
    p.parseString(session_, select_);
    eckit::sql::SQLStatement& stmt (session_.statement());

    // n.b. non-owning
    selectStmt_ = dynamic_cast<eckit::sql::SQLSelect*>(&stmt);
    if (! selectStmt_)
        throw UserError(std::string("Expected SELECT, got: ") + select_);

    selectStmt_->prepareExecute();
}


bool SelectIterator::next() {
    bool ret;
    if (noMore_) return false;
    if (!(ret = selectStmt_->processOneRow())) {
        selectStmt_->postExecute();
        noMore_ = true;
    }
    return ret;
}


void SelectIterator::setOutputRowBuffer(double* data, size_t count) {
    output_.resetBuffer(data, count);
}


#if 0
SelectIterator::~SelectIterator()
{
    if (ownData_) delete [] data_;
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
#endif

//----------------------------------------------------------------------------------------------------------------------

} // namespace odc 
