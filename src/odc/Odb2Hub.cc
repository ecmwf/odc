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

#include "odc/odccapi.h"
#include "eckit/sql/type/SQLBitfield.h"
#include "odc/DataStream.h"
#include "odc/Codec.h"
#include "odc/Column.h"
#include "odc/MetaData.h"
#include "odc/IteratorProxy.h"
#include "odc/FastODA2Request.h"
#include "odc/CommandLineParser.h"
#include "odc/StringTool.h"

#include "odc/FileMapper.h"
#include "Odb2Hub.h"

using namespace eckit;
using namespace odc;
using namespace odc::tool;

PathName Odb2Hub::getPath(const std::string& schema, const std::string& pathName, const std::string& keywordToColumnMapping)
{
    std::ostream& L(Log::info());
	FileMapper mapper(schema);
	
    std::vector<std::string> keywords(mapper.keywords());
    L << "Odb2Hub::getPath: schema = " << schema << std::endl;
    L << "Odb2Hub::getPath: pathName = " << pathName << std::endl;
    L << "Odb2Hub::getPath: keywordToColumnMapping = " << keywordToColumnMapping << std::endl;

	FastODA2Request<ODA2RequestServerTraits> o2r;
	//o.parseConfig(StringTool::readFile(cfgFile));
	o2r.parseConfig(keywordToColumnMapping);

	OffsetList offsets;
	LengthList lengths;
    std::vector<ODAHandle*> handles;
	bool rc = o2r.scanFile(pathName, offsets, lengths, handles);
	ASSERT(rc == true);

	// This will throw an exception if values of interest are not constant.
    std::map<std::string, double> uniqueValues(o2r.getUniqueValues());
    std::map<std::string, std::string> values;
    for (std::map<std::string, double>::iterator it(uniqueValues.begin()); it != uniqueValues.end(); ++it)
	{
        const std::string keyword(it->first);
        const std::set<std::string>& vs(o2r.getValues(keyword));
		ASSERT (vs.size() == 1);
        std::string value (StringTools::trim(*vs.begin()));

        L << "getPath: '" << keyword << "' = '" << value << "'" << std::endl;

		values[keyword] = value;
	}
	
	return mapper.encodeRelative(values);
}
