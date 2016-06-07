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

#include "eckit/io/MultiHandle.h"
#include "eckit/types/Types.h"
#include "eckit/parser/StringTools.h"

#include "ecml/parser/Request.h"
#include "ecml/parser/RequestParser.h"
#include "ecml/core//ExecutionContext.h"
#include "ecml/core/Environment.h"
#include "ecml/data/DataHandleFactory.h"

#include "CreatePartitionsHandler.h"
#include "odb_api/Partitioner.h"

using namespace std;
using namespace eckit;
using namespace odb;

namespace odb {

CreatePartitionsHandler::CreatePartitionsHandler(const string& name) : RequestHandler(name) {}

/// verb create_partitions
/// parameters:
///             files             ODB files
///             n                 desired number of partitions
///             target            file with partitions information; default: partitions_info.txt 
///             write_files       if set to true create one data file for each partition; default: false
///             partition_prefix  prefix of file names if writing data; default: "part.odb" 
///
/// return value: descriptors of partitions

ecml::Values CreatePartitionsHandler::handle(ecml::ExecutionContext& context)
{
    string target (context.environment().lookup("target", "partitions_info.txt", context));

    Log::info() << "create_partitions: saving partitions info to '" << target << "'" << endl;

    const string numberOfPartitions (context.environment().lookup("n", "", context));
    size_t n (atol(numberOfPartitions.c_str()));

    if (n <= 0)
        throw UserError("create_partitions: parameter 'n' must be a positive integer");

    string writeFiles (context.environment().lookup("write_files", "0", context));
    if (StringTools::lower(writeFiles) == "true")
        writeFiles = "1";
    bool doWriteFiles (atoi(writeFiles.c_str()));

    const string prefix (context.environment().lookup("partition_prefix", "part.odb", context));

    vector<string> fs (context.environment().lookupList("files", context));
    vector<PathName> files;
    for (size_t i(0); i < fs.size(); ++i)
        files.push_back(fs[i]);

    Partitions partitions (odb::Partitioner::createPartitions(files, n));
    partitions.save(target);

    ecml::List l;

    if (! doWriteFiles)
        l.append(target);
    else
    {
        vector<PathName> ps (partitions.write(prefix));
        for (size_t i(0); i < ps.size(); ++i)
            l.append(ps[i]);
    }

    return l;
}

} // namespace odb 

