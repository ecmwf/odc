/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/filesystem/PathName.h"
#include "eckit/io/PartFileHandle.h"
#include "odb_api/DispatchingWriter.h"
#include "odb_api/MetaDataReader.h"
#include "odb_api/MetaDataReaderIterator.h"
#include "odb_api/Reader.h"
#include "odb_api/Select.h"
#include "odb_api/Stack.h"
#include "odb_api/TemplateParameters.h"
#include "SplitTool.h"

using namespace eckit;
using namespace std;

namespace odb {
namespace tool {

typedef odb::MetaDataReader<odb::MetaDataReaderIterator> MDReader;

SplitTool::SplitTool (int argc, char *argv[])
: Tool(argc, argv),
  sort_(false),
  maxOpenFiles_(200)
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

	if (optionIsSet("-sort")) sort_ = true;
   
	maxOpenFiles_ = optionArgument("-maxopenfiles", maxOpenFiles_);
	Log::debug() << "SplitTool: maxOpenFiles_ = " << maxOpenFiles_ << endl;

	PathName inFile (parameters(1));
	string outFileTemplate (parameters(2));

	if (sort_)
		presortAndSplit(inFile, outFileTemplate);
	else
		split(inFile, outFileTemplate, maxOpenFiles_, !optionIsSet("-no_verification"));
}

/**
 * @param maxExpandedSize maximum size of the data in chunks after decoding
*/
vector<pair<Offset,Length> > SplitTool::getChunks(const PathName& inFile, size_t maxExpandedSize)
{
    ostream &L(Log::debug());
	L << "SplitTool::getChunks: " << endl;

    vector<pair<Offset,Length> > r;

    MDReader mdr(inFile);
    MDReader::iterator it(mdr.begin()), end(mdr.end());

	Offset currentOffset(0);
	Length currentLength(0);
	size_t currentSize (0);

    for(; it != end; ++it)
    {   
        Offset offset((**it).blockStartOffset());
        Length length((**it).blockEndOffset() - offset);
		size_t numberOfRows (it->columns().rowsNumber());
		size_t numberOfColumns (it->columns().size());

		L << "SplitTool::getChunks: " << offset << " " << length << endl;

		size_t size (numberOfRows * numberOfColumns * sizeof(double));
		if (currentSize + size > maxExpandedSize)
		{
			L << "SplitTool::getChunks: collect " << currentOffset << " " << currentLength << endl;
            r.push_back(make_pair(currentOffset, currentLength));
			currentOffset = offset;
			currentLength = length;
		} else {
			currentLength += length;
			currentSize += numberOfRows * numberOfColumns * sizeof(double);
		}
    } 
	if (r.size() == 0 || r.back().first != currentOffset)
        r.push_back(make_pair(currentOffset, currentLength));
	return r;
}

std::string SplitTool::genOrderBySelect(const std::string& inFile, const std::string& outFileTemplate)
{
    MDReader mdr(inFile);
    MDReader::iterator it(mdr.begin());
    TemplateParameters templateParameters;
    TemplateParameters::parse(outFileTemplate, templateParameters, it->columns());
    std::stringstream ss;
	ss << "select * order by ";
	for (size_t i = 0; i < templateParameters.size(); ++i)
	{
		if (i) ss << ",";
		ss << templateParameters[i]->name;
	}
	std::string sql (ss.str());
	Log::info() << "SplitTool::genOrderBySelect: sql: '" << sql << "'" << endl;
	return sql;
}

void SplitTool::presortAndSplit(const PathName& inFile, const std::string& outFileTemplate)
{
	odb::DispatchingWriter out(outFileTemplate, 1); 
	odb::DispatchingWriter::iterator outIt (out.begin());

	string sql(genOrderBySelect(inFile, outFileTemplate));
	
    vector<std::pair<Offset,Length> > chunks(getChunks(inFile));
    for(size_t i=0; i < chunks.size(); ++i)
    {   
		PartFileHandle h(inFile, chunks[i].first, chunks[i].second);
		h.openForRead();
		odb::Select in(sql, h);
		outIt->pass1(in.begin(), in.end());
    } 
}

void SplitTool::split(const PathName& inFile, const std::string& outFileTemplate, size_t maxOpenFiles, bool verify)
{
	odb::Reader in(inFile);
	odb::DispatchingWriter out(outFileTemplate, maxOpenFiles);

	odb::DispatchingWriter::iterator outIt (out.begin());
	outIt->pass1(in.begin(), in.end());

	odb::Reader input(inFile);
	odb::Reader::iterator begin(input.begin());
	odb::Reader::iterator end(input.end());
	outIt->close();
    if (verify) (**outIt).verify(begin, end);
}

} // namespace tool 
} // namespace odb 

