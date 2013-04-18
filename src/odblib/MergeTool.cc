/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odblib/odb_api.h"
#include "odblib/Tool.h"
#include "odblib/ToolFactory.h"
#include "Comparator.h"
#include "MergeTool.h"

using namespace eclib;

namespace odb {
namespace tool {

MergeTool::MergeTool (int ac, char *av[])
: Tool(ac, av) 
{
	registerOptionWithArgument("-o");
	if (parameters().size() < 3)
	{
		Log::error() << "Usage:";
		usage(parameters(0), Log::error());
		Log::error() << endl;
		return;
	}
	string o = optionArgument("-o", string("<no-default>"));
	if (o == "<no-default>")
		UserError("Output file is obligatory (option -o)");
	outputFile_ = o;

	for (size_t i = 1; i < parameters().size(); ++i) 
		inputFiles_.push_back(PathName(parameters()[i]));
}


void MergeTool::run()
{
	stringstream s;
	for (size_t i = 0; i < inputFiles_.size(); ++i)
		s << inputFiles_[i] << ",";
	Timer t(string("Merging files '") + s.str() + "' into '" + outputFile_ + "'");
	merge(inputFiles_, outputFile_);
}

void MergeTool::merge(const vector<PathName>& inputFiles, const PathName& outputFile)
{
	typedef odb::Reader::iterator I;
	struct R : public vector<odb::Reader*> {
		~R() { for (size_t i = 0; i < size(); ++i) delete at(i); }
	} readers;

	vector<pair<I, I> > iterators;

	for (size_t i = 0; i < inputFiles.size(); ++i)
	{
		readers.push_back(new odb::Reader(inputFiles[i]));
		iterators.push_back(make_pair(readers[i]->begin(), readers[i]->end()));
	}

	odb::Writer<> writer(outputFile);
	odb::Writer<>::iterator out(writer.begin());

	for (size_t i = 0; i < iterators.size(); ++i)
	{
		MetaData& columns(iterators[i].first->columns());

		for (size_t i = 0; i < columns.size(); ++i)
			if (out->columns().hasColumn(columns[i]->name()))
				throw eclib::UserError(string("Column '") + columns[i]->name()
					+ "' occurs in more than one input file of merge.");
		out->columns() += columns;
	}

	out->writeHeader();
	Log::info() << "MergeTool::merge: output metadata: " << out->columns() << endl;

	for(;;)
	{
		for (size_t i = 0, ii = 0; ii < iterators.size(); ++ii)
		{
			I& in(iterators[ii].first);
			I& inEnd(iterators[ii].second);
			if(! (in != inEnd))
				return (void) (Log::info() << "Input file number " << ii << " ended." << endl);

			for (size_t cn = 0; cn < in->columns().size(); ++cn)
			{
				ASSERT(i < out->columns().size());
				out->data(i++) = (*in)[cn];
			}
			++in;
		}

		++out;
	}
}

} // namespace tool 
} // namespace odb 

