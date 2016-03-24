/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <sstream>

#include "ChunkHandler.h"

#include "odb_api/Comparator.h"

#include "eckit/types/Types.h"
#include "eckit/ecml/parser/Request.h"
#include "eckit/ecml/parser/RequestParser.h"
#include "eckit/ecml/core/ExecutionContext.h"
#include "eckit/ecml/core/Environment.h"
#include "eckit/ecml/data/DataHandleFactory.h"

#include "odb_api/MetaDataReader.h"
#include "odb_api/MetaDataReaderIterator.h"

using namespace std;
using namespace eckit;
using namespace odb;

namespace odb {

ChunkHandler::ChunkHandler(const string& name) : RequestHandler(name) {}

Values ChunkHandler::handle(ExecutionContext& context)
{
    vector<string> source (context.environment().lookupList("source", context));

    typedef odb::MetaDataReader<odb::MetaDataReaderIterator> MDReader;

    string fileName (source[0]); // TODO: process rest of files

	MDReader oda (fileName);
	MDReader::iterator r(oda.begin());
	MDReader::iterator end(oda.end());

	odb::MetaData metaData(r->columns());
    List l;
	for(; r != end; ++r)
	{
		ASSERT (r->isNewDataset());
		Offset offset ((**r).blockStartOffset());
		Length length ((**r).blockEndOffset() - (**r).blockStartOffset());
        stringstream ss;
        ss << "partfile://" << fileName << ":" << offset << "," << length;
        l.append(ss.str());
	}
    return l;
}

} // namespace odb 

