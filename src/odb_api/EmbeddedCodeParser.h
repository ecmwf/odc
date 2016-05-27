/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef EmbeddedCodeParser_H
#define EmbeddedCodeParser_H

#include <vector>
#include <string>

#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"

#include "odb_api/SQLSession.h"

#include "SQLTable.h"

namespace ecml { class ExecutionContext; }

namespace odb {

namespace sql { class SQLTable; }

class EmbeddedCodeParser {
public:
    static std::vector<odb::sql::SQLTable*> getFromTables(const std::string&, const std::string&, odb::sql::SQLSession&, ecml::ExecutionContext* context);
};

} // namespace odb 

#endif 
