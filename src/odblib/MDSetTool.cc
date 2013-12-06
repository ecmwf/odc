/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/utils/Tokenizer.h"

//#include "odblib/odb_api.h"
//#include "odblib/Tool.h"
//#include "odblib/ToolFactory.h"
#include "odblib/MetaDataReader.h"
#include "odblib/MDSetTool.h"
//#include "odblib/DataStream.h"
#include "eckit/log/Log.h"
#include "eckit/filesystem/PathName.h"
#include "odblib/ODBAPISettings.h"
#include "odblib/Types.h"
#include "eckit/utils/StringTools.h"
#include "odblib/MetaData.h"
#include "odblib/MetaDataReaderIterator.h"
#include "odblib/Header.h"

using namespace eckit;

namespace odb {
namespace tool {

MDSetTool::MDSetTool (int argc, char *parameters[]) : Tool(argc, parameters) { }

void MDSetTool::run()
{
	if (parameters().size() != 4)
	{
		Log::error() << "Usage: ";
		usage(parameters(0), Log::error());
		Log::error() << std::endl;
		return;
	}

	std::vector<std::string> columns;
	std::vector<std::string> types;

	PathName inFile = parameters(2);

	PathName outFile = parameters(3);
	auto_ptr<DataHandle> outHandle(ODBAPISettings::instance().writeToFile(outFile));

	parseUpdateList(parameters(1), columns, types);
    ASSERT(columns.size() == types.size());

	std::vector<BitfieldDef> bitfieldDefs;
    for (size_t i = 0; i < types.size(); ++i)
    {   
        Log::info() << columns[i] << " : " << types[i] << std::endl;

        // Only bitfoelds now:
        // [active:1;passive:1;rejected:1;blacklisted:1;use_emiskf_only:1;monthly:1;constant:1;experimental:1;whitelist:]
        ASSERT(types[i].size());
        ASSERT(types[i][0] == '[');
        ASSERT(types[i][types[i].size() - 1] == ']');

        BitfieldDef bf; 
        std::vector<std::string> parts(StringTools::split(";", types[i].substr(1, types[i].size() - 2)));
        for (size_t p = 0; p < parts.size(); ++p)
        {   
            std::vector<std::string> field = StringTools::split(":", parts[p]);
            bf.first.push_back(field[0]);
            bf.second.push_back(atoi(field[1].c_str()));
        }   
        bitfieldDefs.push_back(bf);
        Log::info() << "" << i << ": " << columns[i] << " - " << bf.first << std::endl; // "[" << bf.second << "]" << std::endl;
    } 


	typedef odb::MetaDataReader<odb::MetaDataReaderIterator> R;
	R reader(inFile, false);
	const R::iterator end = reader.end();
	R::iterator it = reader.begin();

	
	// Se if the file was created on a different order architecture


	for (; it != end; ++it)
	{
		ASSERT(it->isNewDataset());
		MetaData& md = it->columns();
		for (size_t i = 0; i < columns.size(); ++i)
			md[md.columnIndex(columns[i])]->bitfieldDef(bitfieldDefs[i]);

		size_t sizeOfEncodedData =(**it).sizeOfEncodedData(); 
		Properties props;
		if ((**it).byteOrder() == BYTE_ORDER_INDICATOR)
		{
			Log::info() << "MDSetTool::run: SAME ORDER " << sizeOfEncodedData << std::endl;

			serializeHeader<SameByteOrder,DataHandle>(*outHandle, sizeOfEncodedData, md.rowsNumber(), props, md);
			DataStream<SameByteOrder,DataHandle>(*outHandle).writeBytes((**it).encodedData(), sizeOfEncodedData);	
		}
		else
		{
			Log::info() << "MDSetTool::run: OTHER ORDER " << sizeOfEncodedData << std::endl;
			
			serializeHeader<OtherByteOrder,DataHandle>(*outHandle, sizeOfEncodedData, md.rowsNumber(), props, md);
			DataStream<OtherByteOrder,DataHandle>(*outHandle).writeBytes((**it).encodedData(), sizeOfEncodedData);	
		}
	}

}

void MDSetTool::parseUpdateList(std::string s, std::vector<std::string>& columns, std::vector<std::string>& values)
{
    Tokenizer splitAssignments(",");
    std::vector<std::string> assignments;
    splitAssignments(s, assignments);
	
    Tokenizer splitEq("=");

	for (size_t i = 0; i < assignments.size(); ++i)
	{
		std::vector<std::string> assignment;
		splitEq(assignments[i], assignment);
		ASSERT(assignment.size() == 2);

		std::string colName = assignment[0];
		std::string value = assignment[1];
		
		Log::info() << "MDSetTool::parseUpdateList: " << colName << "='" << value << "'" << std::endl;

		columns.push_back(colName);
		values.push_back(value);
	}
}

} // namespace tool 
} // namespace odb 

