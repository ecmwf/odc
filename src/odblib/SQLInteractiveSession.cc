/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eclib/Exceptions.h"
#include "eclib/Log.h"

#include "odblib/SQLAST.h"
#include "odblib/SQLBitfield.h"
#include "odblib/SchemaAnalyzer.h"
#include "odblib/SQLInteractiveSession.h"
#include "odblib/SQLSimpleOutput.h"
#include "odblib/SQLDatabase.h"
#include "odblib/SQLStatement.h"

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
