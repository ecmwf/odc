/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <sstream>

#include "CreateIndexHandler.h"

#include "odb_api/Comparator.h"
#include "eckit/io/MultiHandle.h"

#include "eckit/types/Types.h"
#include "experimental/eckit/ecml/parser/Request.h"
#include "experimental/eckit/ecml/parser/RequestParser.h"
#include "experimental/eckit/ecml/core//ExecutionContext.h"
#include "experimental/eckit/ecml/core/Environment.h"
#include "experimental/eckit/ecml/data/DataHandleFactory.h"
#include "odb_api/Indexer.h"

using namespace std;
using namespace eckit;
using namespace odb;

namespace odb {

CreateIndexHandler::CreateIndexHandler(const string& name) : RequestHandler(name) {}

Values CreateIndexHandler::handle(ExecutionContext& context)
{
    vector<string> fs (context.environment().lookupList("files", context));

    vector<PathName> files;
    for (size_t i(0); i < fs.size(); ++i)
        files.push_back(fs[i]);

    vector<PathName> indices (odb::Indexer::createIndex(files));

    List l;
    for (size_t i(0); i < indices.size(); ++i)
        l.append(indices[i]);

    return l;
}

} // namespace odb 

