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

#include "odb_api/Comparator.h"

#include "eckit/types/Types.h"
#include "experimental/eckit/ecml/parser/Request.h"
#include "experimental/eckit/ecml/parser/RequestParser.h"
#include "experimental/eckit/ecml/core/ExecutionContext.h"
#include "experimental/eckit/ecml/core/Environment.h"
#include "experimental/eckit/ecml/data/DataHandleFactory.h"
#include "odb_api/odb2netcdf/ecml_verbs/Odb2NetcdfHandler.h"
#include "odb_api/odb2netcdf/Odb2NetCDF.h"

#include "odb_api/MetaDataReader.h"
#include "odb_api/MetaDataReaderIterator.h"

using namespace std;
using namespace eckit;
using namespace odb;

Odb2NetcdfHandler::Odb2NetcdfHandler(const string& name) : RequestHandler(name) {}

Values Odb2NetcdfHandler::handle(ExecutionContext& context)
{
    vector<string> source (context.environment().lookupList("source", context));
    vector<string> target (context.environment().lookupList("target", context));
    string format (context.environment().lookup("format", "1d", context));

    if (! (format == "1d" || format == "2d"))
        throw UserError ("odb2netcdf: FORMAT must be one of 1d or 2d");

    if (source.size() != target.size())
        throw UserError ("odb2netcdf: number of elements of SOURCE and TARGET must be the same");

    List l;
	for(size_t i(0); i < source.size(); ++i)
	{
        const string& input (source[i]), output (target[i]);

        if (format == "2d")
        {
            Odb2NetCDF_2D converter (input, output);
            converter.convert();
        }
        else
        {
            Odb2NetCDF_1D converter (input, output);
            converter.convert();
        }
        l.append(output);
	}
    return l;
}
