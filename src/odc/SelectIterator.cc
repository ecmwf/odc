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

#include "odc/core/MetaData.h"
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
    noMore_(false),
    refCount_(0) {

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


//----------------------------------------------------------------------------------------------------------------------

} // namespace odc 
