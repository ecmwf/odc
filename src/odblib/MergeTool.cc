#include "odblib/oda.h"
#include "odblib/Tool.h"
#include "odblib/ToolFactory.h"
#include "Comparator.h"
#include "MergeTool.h"



namespace odb {
namespace tool {

ToolFactory<MergeTool> mergeTool("merge");

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
	struct R : public vector<odb::Reader*> {
		~R() { for (size_t i = 0; i < size(); ++i) delete at(i); }
	} readers;

	vector<pair<odb::Reader::iterator, odb::Reader::iterator> > iterators;

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
				throw UserError(string("Column '") + columns[i]->name() + "' occurs in more than one input file of merge.");

		out->columns() += columns;
	}
	out->writeHeader();

	odb::Reader::iterator it = iterators[0].first,
						  end = iterators[0].second;
	for (; it != end; ++out)
	{
		double* buff = out->data();
		size_t i = 0;

		for (size_t ii = 0; ii < iterators.size(); ++ii)
		{
			for (size_t cn = 0; cn < iterators[ii].first->columns().size(); ++cn)
				buff[i++] = (*iterators[ii].first)[cn];
			++iterators[ii].first;
		}
	}
}

} // namespace tool 
} // namespace odb 

