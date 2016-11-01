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
#include "odb_api/SQLOutputConfig.h"

namespace odb {
namespace sql {

SQLNonInteractiveSession::SQLNonInteractiveSession(const odb::sql::SQLOutputConfig& config, const std::string& csvDelimiter)
: SQLSession(config, csvDelimiter),
  statement_(0)
{}

SQLNonInteractiveSession::~SQLNonInteractiveSession()
{}

SQLOutput* SQLNonInteractiveSession::defaultOutput()
{
    return new SQLSimpleOutput(std::cout);
}

void SQLNonInteractiveSession::statement(SQLStatement *sql)
{
    delete statement_;
    statement_ = sql;
}

SQLStatement * SQLNonInteractiveSession::statement()
{
    typedef odb::sql::SQLStatement* P;
    if (gotSelectAST())
    {
        gotSelectAST(false);
        statement_ = P(selectFactory().create(*this, selectAST()));
    }
    return statement_;
}

} // namespace sql
} // namespace odb
