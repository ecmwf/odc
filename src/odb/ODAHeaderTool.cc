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

#include "odblib/MetaDataReaderIterator.h"
#include "odblib/MetaDataReader.h"

#include "odb/ODAHeaderTool.h"
#include "odb/Tool.h"
#include "odb/ToolFactory.h"

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

	typedef odb::MetaDataReader R;
	R oda(db);
	R::iterator r(oda.begin());
	R::iterator end(oda.end());

	odb::MetaData metaData(0);

	size_t headerCount = 0;
	// NOTE: Currently it prints columns' meta data only.
	// TODO: print out the whole header.

	metaData = r->columns();
	o << endl << "Header " << ++headerCount << ":";
	o << endl << r->columns();

	int nrows = 0;
	for(; r != end; ++r, ++nrows)
	{
		ASSERT (r->isNewDataset());

		Log::debug() << "Block: <" << (**r).blockStartOffset() << "," << (**r).blockEndOffset() << ">" << endl;
		if (! (metaData == r->columns()))
		{
			o << endl << "Header " << ++headerCount << ":" << endl << r->columns();
		}

		metaData = r->columns();
	}
}

} // namespace tool 
} // namespace odb 

