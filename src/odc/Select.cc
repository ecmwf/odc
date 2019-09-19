/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Piotr Kuchta
/// @author Simon Smart
/// @date April 2010

#include "eckit/io/DataHandle.h"

#include "odc/data/DataHandleFactory.h"
#include "odc/Select.h"
#include "odc/SelectIterator.h"
#include "odc/sql/SQLOutputConfig.h"
#include "odc/sql/SQLSelectOutput.h"
#include "odc/sql/TODATable.h"

using namespace eckit;

namespace odc {

//----------------------------------------------------------------------------------------------------------------------

// TODO: Select should BE a SelectIterator, not posess one.

Select::Select(const std::string& selectStatement, bool manageOwnBuffer) :
    selectStatement_(selectStatement),
    session_(std::unique_ptr<eckit::sql::SQLOutput>(new sql::SQLSelectOutput(manageOwnBuffer)),
             std::unique_ptr<eckit::sql::SQLOutputConfig>(new odc::sql::SQLOutputConfig)),
    initted_(false),
    it_(nullptr) {}


Select::Select(const std::string& selectStatement, DataHandle& dh, bool /* manageOwnBuffer */) :
    Select(selectStatement, true) {

    dh.openForRead();
    eckit::sql::SQLDatabase& db(session_.currentDatabase());
    db.addImplicitTable(new odc::sql::ODATable(db, dh));
}


Select::Select(const std::string& selectStatement, const std::string& path, bool /* manageOwnBuffer */) :
    Select(selectStatement) {

    ownDH_.reset(DataHandleFactory::openForRead(path));
    ownDH_->openForRead();
    eckit::sql::SQLDatabase& db(session_.currentDatabase());
    db.addImplicitTable(new odc::sql::ODATable(db, *ownDH_));
}

Select::Select(const std::string &selectStatement, const char *path, bool manageOwnBuffer) :
    Select(selectStatement, std::string(path), manageOwnBuffer) {}


Select::~Select() {}

eckit::sql::SQLDatabase& Select::database() {
    return session_.currentDatabase();
}


SelectIterator* Select::createSelectIterator(const std::string& sql) {

    sql::SQLSelectOutput* output = dynamic_cast<sql::SQLSelectOutput*>(&session_.output());
    ASSERT(output);

    return new SelectIterator(sql, session_, *output);
}

const Select::iterator Select::end() { return iterator(nullptr); }

// This is horrible, but the TextReader, and any stream based iteraton, can only
// iterate once, so we MUST NOT create two iterators if begin() is called twice.
Select::iterator Select::begin()
{
    if (!initted_) {
        SelectIterator* it = createSelectIterator(selectStatement_);
        ASSERT(it);
        it->next();
        it_ = iterator(it);
        initted_ = true;
    }
    return it_;
}


#ifdef SWIGPYTHON
template odc::IteratorProxy< odc::SelectIterator,odc::Select,double const >; 
#endif

//----------------------------------------------------------------------------------------------------------------------

} // namespace odc
