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
#include "odb_api/MetaData.h"
#include "odb_api/MetaDataReader.h"
#include "odb_api/MetaDataReaderIterator.h"
#include "odb_api/Reader.h"
#include "odb_api/Select.h"
#include "odb_api/Indexer.h"
#include "odb_api/tools/IndexTool.h"
#include "odb_api/tools/CountTool.h"
#include "eckit/io/PartFileHandle.h"
#include "eckit/io/Offset.h"
#include "eckit/io/Length.h"

using namespace eckit;

namespace odb {
namespace tool {

IndexTool::IndexTool (int argc, char *argv[]) : Tool(argc, argv) { }

void IndexTool::run()
{
	if (! (parameters().size() == 2 || parameters().size() == 3))
	{
		Log::error() << "Usage: ";
		usage(parameters(0), Log::error());
		Log::error() << std::endl;
		return;
	}

    PathName dataFile (parameters(1));
    PathName indexFile (parameters().size() == 3 
                        ? parameters(2) 
                        : parameters(1) + ".idx");

	Indexer::createIndex(dataFile, indexFile);
}

} // namespace tool 
} // namespace odb 

