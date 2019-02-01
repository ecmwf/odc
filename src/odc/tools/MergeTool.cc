/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/FileHandle.h"
#include "eckit/log/Timer.h"
#include "odc/Reader.h"
#include "odc/Select.h"
#include "odc/Writer.h"
#include "odc/tools/MergeTool.h"

using namespace eckit;
using namespace odc::core;

namespace odc {
namespace tool {

void MergeTool::help(std::ostream &o) {
    o << "Merges rows from files";
}

void MergeTool::usage(const std::string& name, std::ostream &o)
{
    o << name << " -o <output-file.odb> <input1.odb> <input2.odb> ..." << std::endl
              << ""                                                    << std::endl
              << "\t or "                                                << std::endl
              << ""                                                    << std::endl
      << name << "\t -S -o <output-file.odb> <input1.odb> <sql-select1> <input2.odb> <sql-select2> ..." << std::endl;
}

MergeTool::MergeTool (int ac, char *av[])
: Tool(ac, av),
  inputFiles_(),
  sql_(),
  outputFile_(),
  sqlFiltering_(false)
{
	registerOptionWithArgument("-o");
	if (parameters().size() < 3)
	{
		Log::error() << "Usage:";
		usage(parameters(0), Log::error());
		Log::error() << std::endl;
		return;
	}
    sqlFiltering_ = optionIsSet("-S");
	std::string o(optionArgument("-o", std::string("<no-default>")));
	if (o == "<no-default>")
		UserError("Output file is obligatory (option -o)");
	outputFile_ = o;

	for (size_t i = 1; i < parameters().size(); ++i) 
    {
		inputFiles_.push_back(PathName(parameters()[i]));
        if (sqlFiltering_) {
            std::string s(parameters()[++i]);
            sql_.push_back(StringTool::isSelectStatement(s) ? s : StringTool::readFile(s));
        }
    }
}


void MergeTool::run()
{
    if (inputFiles_.size() == 0)
        return;
    std::stringstream s;
	for (size_t i = 0; i < inputFiles_.size(); ++i)
		s << inputFiles_[i] << ",";
	Timer t(std::string("Merging files '") + s.str() + "' into '" + outputFile_ + "'");
    if(! sqlFiltering_)
        merge(inputFiles_, outputFile_);
    else
        merge(inputFiles_, sql_, outputFile_);
}

template <typename T, typename I>
void doMerge(std::vector<std::pair<I, I> >& iterators, const PathName& outputFile)
{
	odc::Writer<> writer(outputFile);
	odc::Writer<>::iterator out(writer.begin());

	for (size_t i = 0; i < iterators.size(); ++i)
	{
		MetaData columns (iterators[i].first->columns());

		for (size_t i = 0; i < columns.size(); ++i)
			if (out->columns().hasColumn(columns[i]->name()))
				throw eckit::UserError(std::string("Column '") + columns[i]->name()
					+ "' occurs in more than one input file of merge.");
        MetaData md (out->columns());
        md += columns;
        out->columns(md);
		//out->columns() += columns;
	}

	out->writeHeader();
	Log::info() << "MergeTool::merge: output metadata: " << out->columns() << std::endl;

	for(;;)
	{
		for (size_t i = 0, ii = 0; ii < iterators.size(); ++ii)
		{
			I& in(iterators[ii].first);
			I& inEnd(iterators[ii].second);
			if(! (in != inEnd))
                return (void) (Log::info() << "Input file number " << ii << " ended." << std::endl);

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

template <typename T>
struct AutoR : public std::vector<T*> { ~AutoR() { for (size_t i = 0; i < this->size(); ++i) delete this->at(i); } }; 

void MergeTool::merge(const std::vector<PathName>& inputFiles, const PathName& outputFile)
{
	typedef odc::Reader R;
	typedef R::iterator I;

    AutoR<R>  readers;
    std::vector<std::pair<I, I> > iterators;

	for (size_t i = 0; i < inputFiles.size(); ++i)
	{
		readers.push_back(new odc::Reader(inputFiles[i]));
        iterators.push_back(std::make_pair(readers[i]->begin(), readers[i]->end()));
	}
    doMerge<R, I>(iterators, outputFile);
}

void MergeTool::merge(const std::vector<PathName>& inputFiles, const std::vector<std::string>& sqls, const PathName& outputFile)
{
    typedef odc::Select S;
    AutoR<S> readers;
    AutoR<eckit::FileHandle> fhs;
    std::vector<std::pair<S::iterator, S::iterator> > iterators;
	for (size_t i = 0; i < inputFiles.size(); ++i)
	{
        FileHandle* fh = new FileHandle(inputFiles[i]);
        fh->openForRead();
        fhs.push_back(fh);
		readers.push_back(new S(sqls[i], *fhs[i]));
        iterators.push_back(std::make_pair(readers[i]->begin(), readers[i]->end()));
	}
    doMerge<S, S::iterator>(iterators, outputFile);
}

} // namespace tool 
} // namespace odc 

