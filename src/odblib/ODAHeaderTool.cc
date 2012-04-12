#include "oda.h"

#include "Tool.h"
#include "ToolFactory.h"
#include "ODAHeaderTool.h"
#include "MetaDataReaderIterator.h"
#include "MetaDataReader.h"

namespace odb {
namespace tool {

ToolFactory<HeaderTool> odaHeader("header");

HeaderTool::HeaderTool (int argc, char *argv[]) : Tool(argc, argv) { }

void HeaderTool::run()
{
	if (parameters().size() < 2)
	{
		Log::error() << "Usage: ";
		usage(parameters(0), Log::error());
		Log::error() << endl;
		return;
	}

	string db = parameters(1);

	ostream& o = cout;

	typedef odb::MetaDataReader<odb::MetaDataReaderIterator> R;
	R oda(db);
	R::iterator r(oda.begin());
	R::iterator end(oda.end());

	odb::MetaData metaData(0);

	size_t headerCount = 0;
	// NOTE: Currently it prints columns' meta data only.
	// TODO: print out the whole header.

	metaData = r->columns();
	//o << endl << "Header " << ++headerCount << ":";
	//o << endl << r->columns();

	int nrows = 0;
	for(; r != end; ++r, ++nrows)
	{
		ASSERT (r->isNewDataset());

		//if (! (metaData == r->columns()))
		//{
			o << endl << "Header " << ++headerCount << ". "
				<< "Begin offset: " << (**r).blockStartOffset() << ", end offset: " << (**r).blockEndOffset()
				<< ", number of rows in block: " << r->columns().rowsNumber() << endl
				<< r->columns();
		//}

		metaData = r->columns();
	}
}

} // namespace tool 
} // namespace odb 

