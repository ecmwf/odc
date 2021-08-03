/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/log/Log.h"
#include "odc/Comparator.h"
#include "odc/Reader.h"
#include "odc/Writer.h"
#include "odc/tools/CompactTool.h"

using namespace std;
using namespace eckit;

namespace odc {
namespace tool {

CompactTool::CompactTool (int argc, char *argv[]) : Tool(argc, argv) { }

void CompactTool::run()
{
	if (parameters().size() != 3)
	{
		Log::error() << "Usage: ";
		usage(parameters(0), Log::error());
		Log::error() << std::endl;
        std::stringstream ss;
        ss << "Expected exactly 3 command line parameters";
        throw UserError(ss.str());
	}

	PathName inFile = parameters(1);
	PathName outFile = parameters(2);

	odc::Reader in(inFile);
	odc::Writer<> out(outFile);

	odc::Reader::iterator it(in.begin());
	odc::Reader::iterator end(in.end());

	odc::Writer<>::iterator writer(out.begin());
	writer->pass1(it, end);
	
	odc::Reader outReader(outFile);
	Log::info() << "Verifying." << std::endl;	
	odc::Reader::iterator it1 = in.begin();
	odc::Reader::iterator end1 = in.end();

	odc::Reader::iterator it2 = outReader.begin();
	odc::Reader::iterator end2 = outReader.end();

	odc::Comparator comparator;
	comparator.compare(it1, end1, it2, end2, inFile, outFile);
}

} // namespace tool 
} // namespace odc 

