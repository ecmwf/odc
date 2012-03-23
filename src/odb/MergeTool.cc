/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odblib/oda.h"
#include "Tool.h"
#include "ToolFactory.h"
#include "odblib/Comparator.h"
#include "MergeTool.h"

#define SRC __FILE__, __LINE__

namespace odb {
namespace tool {

ToolFactory<MergeTool> mergeTool("merge");

MergeTool::MergeTool (int argc, char *argv[])
: Tool(argc, argv) 
{
	if (argc != 4)
	{
		Log::error() << "Usage:";
		usage(parameters(0), Log::error());
		Log::error() << endl;
		return;
	}
	

	file1_ = PathName(argv[1]);
	file2_ = PathName(argv[2]);

    ASSERT("First file does not exist!" && file1_.exists());
    ASSERT("Second file does not exist!" && file2_.exists());

	outputFile_ = PathName(argv[3]);
}


void MergeTool::run()
{
	Timer t(string("Merging files '") + file1_ + "' and '" + file2_ + "' into '" + + "'");
	merge(file1_, file2_, outputFile_);
}

void MergeTool::merge(const PathName& file1, const PathName& file2, const PathName& outputFile)
{
	odb::Reader f1(file1);
	odb::Reader f2(file2);

	odb::Reader::iterator it1(f1.begin()), end1(f1.end());
	odb::Reader::iterator it2(f2.begin()), end2(f2.end());

	odb::Writer<> writer(outputFile);
	odb::Writer<>::iterator out(writer.begin());

	out->columns() = it1->columns() + it2->columns();

	size_t n = out->columns().size();
	size_t n1 = it1->columns().size();
	size_t n2 = it2->columns().size();
	ASSERT(n == it1->columns().size() + it2->columns().size());

	out->writeHeader();

	//double buff[n]; 
	for (; it1 != end1; ++it1, ++it2)
	{
		double* buff = out->data();
		size_t i = 0;
		for (size_t i1 = 0; i1 < n1; ++i1)
			buff[i++] = (*it1)[i1];
		for (size_t i2 = 0; i2 < n2; ++i2)
			buff[i++] = (*it2)[i2];
		++out;
	}
}

} // namespace tool 
} // namespace odb 

