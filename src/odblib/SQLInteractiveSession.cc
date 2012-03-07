/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "Exceptions.h"
#include "Log.h"
#include "SQLAST.h"
#include "SQLBitfield.h"
#include "SchemaAnalyzer.h"
#include "SQLInteractiveSession.h"
#include "SQLSimpleOutput.h"
#include "SQLDatabase.h"
#include "SQLStatement.h"

namespace odb {
namespace sql {

SQLInteractiveSession::SQLInteractiveSession(ostream &out)
: out_(out)
{}

SQLInteractiveSession::~SQLInteractiveSession()
{}

SQLOutput* SQLInteractiveSession::defaultOutput()
{
	return new SQLSimpleOutput(out_);
}

void SQLInteractiveSession::statement(SQLStatement *sql)
{
	ASSERT(sql);	
	execute(*sql);
	delete sql;
}

} // namespace sql
} // namespace odb
