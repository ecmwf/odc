/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/config/Resource.h"

#include "odb_api/odbcapi.h"
#include "odb_api/SQLBitfield.h"
#include "odb_api/DataStream.h"
#include "odb_api/HashTable.h"
#include "odb_api/Codec.h"
#include "odb_api/Column.h"
#include "odb_api/MetaData.h"
#include "odb_api/IteratorProxy.h"
#include "odb_api/FastODA2Request.h"
#include "odb_api/CommandLineParser.h"
#include "odb_api/StringTool.h"

#include "odb_api/FileMapper.h"
#include "Odb2Hub.h"

using namespace eckit;
using namespace odb;
using namespace odb::tool;

PathName Odb2Hub::getPath(const string& schema, const string& pathName, const string& keywordToColumnMapping)
{
    ostream& L(Log::info());
	FileMapper mapper(schema);
	
	vector<string> keywords(mapper.keywords());
	L << "Odb2Hub::getPath: schema = " << schema << endl;
	L << "Odb2Hub::getPath: pathName = " << pathName << endl;
	L << "Odb2Hub::getPath: keywordToColumnMapping = " << keywordToColumnMapping << endl;

	FastODA2Request<ODA2RequestServerTraits> o2r;
	//o.parseConfig(StringTool::readFile(cfgFile));
	o2r.parseConfig(keywordToColumnMapping);

	OffsetList offsets;
	LengthList lengths;
	vector<ODAHandle*> handles;
	bool rc = o2r.scanFile(pathName, offsets, lengths, handles);
	ASSERT(rc == true);

	// This will throw an exception if values of interest are not constant.
	map<string, double> uniqueValues(o2r.getUniqueValues());
	map<string, string> values;
	for (map<string, double>::iterator it(uniqueValues.begin()); it != uniqueValues.end(); ++it)
	{
		const string keyword(it->first);
		const set<string>& vs(o2r.getValues(keyword));
		ASSERT (vs.size() == 1);
        string value (StringTools::trim(*vs.begin()));

		L << "getPath: '" << keyword << "' = '" << value << "'" << endl;

		values[keyword] = value;
	}
	
	return mapper.encodeRelative(values);
}
