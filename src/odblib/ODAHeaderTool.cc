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
#include "ODAHeaderTool.h"
#include "MetaDataReaderIterator.h"
#include "MetaDataReader.h"

namespace odb {
namespace tool {

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

