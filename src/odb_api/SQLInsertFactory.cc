/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/types/Types.h"


#include "odb_api/SQLDatabase.h"
#include "odb_api/SQLInsertFactory.h"
#include "odb_api/SQLSession.h"
#include "odb_api/Writer.h"
#include "odb_api/SQLAST.h"
#include "odb_api/SQLInsert.h"

using namespace eckit;
using namespace std;

namespace odb {
namespace sql {

SQLInsertFactory::SQLInsertFactory()
{}

SQLInsert* SQLInsertFactory::create (SQLSession& session, const InsertAST& a)
{
    return new SQLInsert(a);
}

} // namespace sql
} // namespace odb
