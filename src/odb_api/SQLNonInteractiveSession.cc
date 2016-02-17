/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/eckit.h"
#include "eckit/exception/Exceptions.h"
#include "odb_api/SQLNonInteractiveSession.h"
#include "odb_api/SQLSimpleOutput.h"
#include "odb_api/SQLStatement.h"

namespace odb {
namespace sql {

SQLNonInteractiveSession::SQLNonInteractiveSession()
: statement_(0)
{}

SQLNonInteractiveSession::~SQLNonInteractiveSession()
{}

SQLOutput* SQLNonInteractiveSession::defaultOutput()
{
    NOTIMP;
}

void SQLNonInteractiveSession::statement(SQLStatement *sql)
{
    delete statement_;
    statement_ = sql;
}

SQLStatement * SQLNonInteractiveSession::statement()
{
    return statement_;
}

} // namespace sql
} // namespace odb
