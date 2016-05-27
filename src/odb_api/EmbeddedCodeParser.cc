/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/config/Resource.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"

#include "ecml/core/ExecutionContext.h"
#include "ecml/data/DataHandleFactory.h"

#include "odb_api/EmbeddedCodeParser.h"
#include "odb_api/Reader.h"
#include "odb_api/StringTool.h"
#include "odb_api/Tracer.h"
#include "odb_api/SQLTable.h"
#include <string.h>

using namespace std;
using namespace eckit;

namespace odb {

std::vector<odb::sql::SQLTable*> EmbeddedCodeParser::getFromTables(const string& text, const string& database, odb::sql::SQLSession& session, ecml::ExecutionContext* context)
{
    ASSERT(context);

    ecml::Values values (context->execute(text));

    std::vector<odb::sql::SQLTable*> tables;

    for (ecml::Request e (values); e; e = e->rest())
    {
        string descriptor (e->value()->text());

        if (e->value()->rest()) 
            throw UserError("element on the list returned by embedded code is not a single value");

        DataHandle* dh (ecml::DataHandleFactory::openForRead(descriptor));
        odb::sql::SQLTable* t (session.openDataHandle(*dh));
        tables.push_back(t);
    }
    return tables;
}

} // namespace odb

