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
#include "odc/MetaData.h"
#include "odc/Reader.h"
#include "odc/Select.h"
#include "odc/Indexer.h"
#include "odc/tools/IndexTool.h"
#include "odc/tools/CountTool.h"
#include "eckit/io/PartFileHandle.h"
#include "eckit/io/Offset.h"
#include "eckit/io/Length.h"

using namespace eckit;

namespace odc {
namespace tool {

IndexTool::IndexTool (int argc, char *argv[]) : Tool(argc, argv) { }

void IndexTool::help(std::ostream &o) {
    o << "Creates index of reports for a given file";
}


void IndexTool::usage(const std::string& name, std::ostream &o) {
    o << name
      << " <file.odb> [<file.odb.idx>] " << std::endl
      << std::endl
      << "\tSpecifically the index file is an ODB file with (INTEGER) columns: block_begin, block_length, seqno, n_rows"
      << std::endl
      << "\tOne entry is made for each unique seqno - block pair within the source ODB file." << std::endl;
}


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
} // namespace odc 

