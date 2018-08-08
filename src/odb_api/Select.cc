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

#include "odb_api/data/DataHandleFactory.h"
#include "odb_api/Select.h"
#include "odb_api/sql/SQLSelectOutput.h"
#include "odb_api/TODATable.h"

using namespace eckit;

namespace odb {

//----------------------------------------------------------------------------------------------------------------------

Select::Select(const std::string& selectStatement, bool manageOwnBuffer) :
    selectStatement_(selectStatement),
    session_(std::unique_ptr<eckit::sql::SQLOutput>(new sql::SQLSelectOutput(manageOwnBuffer))) {}


Select::Select(const std::string& selectStatement, DataHandle& dh, bool manageOwnBuffer) :
    Select(selectStatement, &dh) {

    dh.openForRead();
    eckit::sql::SQLDatabase& db(session_.currentDatabase());
    db.addImplicitTable(new odb::sql::TODATable(db, dh));
}


Select::Select(const std::string& selectStatement, const std::string& path, bool manageOwnBuffer) :
    Select(selectStatement) {

    ownDH_.reset(DataHandleFactory::openForRead(path));
    ownDH_->openForRead();
    eckit::sql::SQLDatabase& db(session_.currentDatabase());
    db.addImplicitTable(new odb::sql::TODATable(db, *ownDH_));
}


Select::~Select() {}


SelectIterator* Select::createSelectIterator(const std::string& sql) {

    sql::SQLSelectOutput* output = dynamic_cast<sql::SQLSelectOutput*>(&session_.output());
    ASSERT(output);

    return new SelectIterator(sql, session_, *output);
}

const Select::iterator Select::end() { return iterator(0); }

Select::iterator Select::begin()
{
    SelectIterator* it = createSelectIterator(selectStatement_);
    ASSERT(it);
    it->next();
    return iterator(it);
}


#ifdef SWIGPYTHON
template odb::IteratorProxy< odb::SelectIterator,odb::Select,double const >; 
#endif

//----------------------------------------------------------------------------------------------------------------------

} // namespace odb
