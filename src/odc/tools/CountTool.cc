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
#include "eckit/log/Log.h"

#include "odc/core/MetaData.h"
#include "odc/core/TablesReader.h"
#include "odc/LibOdc.h"
#include "odc/Reader.h"
#include "odc/tools/CountTool.h"

using namespace eckit;

namespace odc {
namespace tool {

CountTool::CountTool (int argc, char *argv[]) : Tool(argc, argv) { }

size_t CountTool::rowCount(const PathName &db)
{
    odc::core::TablesReader reader(db);
    auto it = reader.begin();
    auto end = reader.end();

    size_t n = 0;

    for (; it != end; ++it) {
        n += it->rowCount();
    }

	return n;
}

void CountTool::run()
{
	if (parameters().size() < 2)
	{
		Log::error() << "Usage: ";
		usage(parameters(0), Log::error());
		Log::error() << std::endl;
        std::stringstream ss;
        ss << "Expected at least 2 command line parameters";
        throw UserError(ss.str());
	}

	unsigned long long n (0);
    for (size_t i (1); i < parameters().size(); ++i)
    {
        const std::string fileName (parameters(i));

        LOG_DEBUG_LIB(LibOdc) << "CountTool: counting " << fileName << std::endl;

        n += rowCount(fileName);
    }
	
	std::cout << n << std::endl;
}

} // namespace tool 
} // namespace odc 

