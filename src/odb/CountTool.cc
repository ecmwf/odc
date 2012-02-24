#include "oda.h"

#include "Tool.h"
#include "ToolFactory.h"
#include "CountTool.h"
#include "MetaDataReaderIterator.h"
#include "MetaDataReader.h"

namespace odb {
namespace tool {

ToolFactory<CountTool> countTool("count");

CountTool::CountTool (int argc, char *argv[]) : Tool(argc, argv) { }

unsigned long long CountTool::fastRowCount(const PathName &db)
{
	unsigned long long n = 0;

	MetaDataReader mdReader(db);
	MetaDataReader::iterator it = mdReader.begin();
	MetaDataReader::iterator end = mdReader.end();
	for (; it != end; ++it)
	{
		MetaData &md = it->columns();
		n += md.rowsNumber();
	}
	return n;
}

unsigned long long CountTool::rowCount(const PathName &db)
{
	odb::Reader oda(db);
	odb::Reader::iterator i = oda.begin();
	odb::Reader::iterator end = oda.end();

	unsigned long long n = 0;
	for ( ; i != end; ++i)
		++n;
	return n;
}

void CountTool::run()
{
	if (parameters().size() < 2)
	{
		Log::error() << "Usage: ";
		usage(parameters(0), Log::error());
		Log::error() << endl;
		return;
	}

	//unsigned long long n = rowCount(parameters(1));
	unsigned long long n = fastRowCount(parameters(1));
	
	cout << n << endl;
}

} // namespace tool 
} // namespace odb 

