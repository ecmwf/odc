/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odblib/oda.h"

#include "odblib/Tool.h"
#include "odblib/ToolFactory.h"
#include "odblib/MetaDataReader.h"
#include "odblib/Comparator.h"
#include "odblib/SplitTool.h"
#include "odblib/TemplateParameters.h"

#include "eclib/PartFileHandle.h"

namespace odb {
namespace tool {

SplitTool::SplitTool (int argc, char *argv[])
: Tool(argc, argv),
  sort_(true),
  maxOpenFiles_(1)
{
	registerOptionWithArgument("-maxopenfiles");
}

void SplitTool::run()
{
	if (parameters().size() != 3)
	{
		Log::error() << "Usage: ";
		usage(parameters(0), Log::error());
		Log::error() << endl;
		return;
	}

	if (optionIsSet("-nosort")) sort_ = false;
	maxOpenFiles_ = optionArgument("-maxopenfiles", maxOpenFiles_);

	PathName inFile = parameters(1);
	string outFile = parameters(2);

	if (sort_)
		presortAndSplit(inFile, outFile);
	else
		split(inFile, outFile, maxOpenFiles_);
}

void SplitTool::presortAndSplit(const PathName& inFile, const string& outFile)
{
	typedef odb::MetaDataReader<odb::MetaDataReaderIterator> MDReader;
    MDReader mdr(inFile);
    MDReader::iterator it(mdr.begin()), end(mdr.end());

    odb::MetaData metaData(it->columns());
	odb::DispatchingWriter out(outFile, 1); //, append);
	odb::DispatchingWriter::iterator outIt = out.begin();

    TemplateParameters templateParameters;
    TemplateParameters::parse(outFile, templateParameters, it->columns());
	stringstream ss;
	ss << "select * order by ";
	for (size_t i = 0; i < templateParameters.size(); ++i)
	{
		Log::info() << "SplitTool::presortAndSplit: " << templateParameters[i]->name << endl;
		if (i) ss << ",";
		ss << templateParameters[i]->name;
	}
	string sql = ss.str();
	Log::info() << "SplitTool::presortAndSplit: sql: '" << sql << "'" << endl;

	Log::info() << "sql: " << sql << endl;
    for(; it != end; ++it)
    {   
        ASSERT (it->isNewDataset());
        metaData = it->columns();

        Offset offset((**it).blockStartOffset());
        Length length((**it).blockEndOffset() - (**it).blockStartOffset());
		PartFileHandle h(inFile, offset, length);
		h.openForRead();
		odb::Select in(sql, h);
		outIt->pass1(in.begin(), in.end());
    } 
}

void SplitTool::split(const PathName& inFile, const string& outFile, size_t maxOpenFiles)
{
	odb::Reader in(inFile);
	odb::DispatchingWriter out(outFile, maxOpenFiles);

	odb::DispatchingWriter::iterator outIt = out.begin();
	outIt->pass1(in.begin(), in.end());

	odb::Reader input(inFile);
	odb::Reader::iterator begin(input.begin());
	odb::Reader::iterator end(input.end());
	(**outIt).verify(begin, end);
}

} // namespace tool 
} // namespace odb 

