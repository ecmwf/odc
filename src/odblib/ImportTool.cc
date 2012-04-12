#include "Types.h"

#include "oda.h"

#include "Tool.h"
#include "ToolFactory.h"
#include "Tokenizer.h"
#include "ImportTool.h"
#define SRC __FILE__,__LINE__

using namespace std;

namespace odb {
namespace tool {

ToolFactory<ImportTool> import("import");

string ImportTool::defaultDelimiter = ",";

ImportTool::ImportTool(int argc, char *parameters[])
: Tool(argc, parameters)
{
	registerOptionWithArgument("-d"); // Delimiter
}

void ImportTool::run()
{
	Log::info() << "parameters().size(): " << parameters().size() << endl;
	Log::info() << "ImportTool::run: parameters(): " << *static_cast<CommandLineParser*>(this) << endl;

	if (parameters().size() != 3)
	{
		Log::error() << "Usage: ";
		usage(parameters(0), Log::error());
		Log::error() << endl;
		return;
	}

	PathName inFile = parameters(1);
	PathName outFile = parameters(2);

	Log::info() << "ImportTool::run: inFile: " << inFile << ", outFile: " << outFile << endl;

	importFile(inFile, outFile, optionArgument("-d", defaultDelimiter));
}

void ImportTool::importFile(const PathName& in, const PathName& out, const string& delimiter)
{
	ifstream fs(in.c_str());
	odb::Select input("select *", fs);

	odb::Writer<> writer(out);
	odb::Writer<>::iterator output(writer.begin());

	odb::Select::iterator it = input.begin();
	odb::Select::iterator end = input.end();

	unsigned long long n = output->pass1(it, end);

	Log::info(SRC) << "ImportTool::importFile: = copied " << n << " rows." << endl;
}

void ImportTool::importText(const string& s, const PathName& out, const string& delimiter)
{
	stringstream fs(s);
	odb::Select input("select *", fs);

	odb::Writer<> writer(out);
	odb::Writer<>::iterator output(writer.begin());

	odb::Select::iterator it = input.begin();
	odb::Select::iterator end = input.end();

	unsigned long long n = output->pass1(it, end);

	Log::info(SRC) << "ImportTool::importText: = copied " << n << " rows." << endl;
}

} // namespace tool 
} // namespace odb 

