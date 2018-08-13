/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/filesystem/PathName.h"
#include "eckit/log/Log.h"
#include "eckit/parser/StringTools.h"
#include "eckit/parser/Tokenizer.h"
#include "odb_api/Header.h"
#include "odb_api/MetaData.h"
#include "odb_api/MetaDataReader.h"
#include "odb_api/MetaDataReaderIterator.h"
#include "odb_api/ODBAPISettings.h"
#include "eckit/sql/SQLTypedefs.h"
#include "odb_api/tools/MDSetTool.h"

using namespace eckit;
using namespace std;
typedef eckit::StringTools S;

namespace odb {
namespace tool {

void MDSetTool::help(std::ostream &o) {
    o << "Creates a new file resetting types or values (constants only) of columns.";
}


void MDSetTool::usage(const std::string& name, std::ostream &o) {
    o << name << " <update-list> <input.odb> <output.odb>" << endl << endl

      << "\t<update-list> is a comma separated list of expressions of the form:" << endl
      << "\t  <column-name> : <type> = <value>" << endl << endl
      << "\t<type> can be one of: integer, real, double, string. If ommited, the existing type of the column will not be changed." << endl
      << "\tBoth type and value are optional; at least one of the two should be present. For example:" << endl
      << "\t  odb mdset \"expver='    0008'\" input.odb patched.odb " << endl;
}

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

    PathName inFile = parameters(2), outFile = parameters(3);
    std::unique_ptr<DataHandle> outHandle(ODBAPISettings::instance().writeToFile(outFile));

    std::vector<std::string> columns, types, values;
    std::vector<eckit::sql::BitfieldDef> bitfieldDefs;
    parseUpdateList(parameters(1), columns, types, values, bitfieldDefs);

    typedef odb::MetaDataReader<odb::MetaDataReaderIterator> R;
    R reader(inFile, false);

    for (R::iterator it = reader.begin(), end = reader.end();
        it != end;
        ++it)
    {
        ASSERT(it->isNewDataset());
        const MetaData& md (it->columns());
        for (size_t i = 0; i < columns.size(); ++i)
        {
            Column& c (*md[md.columnIndex(columns[i])]);
            Log::info() << "" << columns[i]  << ": " << c << endl;

            if (types[i].size() && types[i] != "NONE") c.type(Column::type(types[i]));
            if (bitfieldDefs[i].first.size()) c.bitfieldDef(bitfieldDefs[i]);
            if (values[i].size() && values[i] != "NONE")
            {
                odb::codec::Codec& codec (c.coder());
                if (codec.name().find("constant") == std::string::npos)
                {
                    stringstream ss;
                    ss << "Column '" << columns[i] << "' is not constant (codec: " << codec.name() << ")" << endl;
                    throw UserError(ss.str());
                }
                double v (StringTool::translate(values[i]));
                c.min(v);
                c.max(v);
            }
        }

		size_t sizeOfEncodedData = (**it).sizeOfEncodedData(); 
	    // See if the file was created on a different order architecture
		if ((**it).byteOrder() == BYTE_ORDER_INDICATOR)
		{
			Log::info() << "MDSetTool::run: SAME ORDER " << sizeOfEncodedData << std::endl;

            serializeHeader<SameByteOrder,DataHandle>(*outHandle, sizeOfEncodedData, md.rowsNumber(), it->properties(), md);
			DataStream<SameByteOrder,DataHandle>(*outHandle).writeBytes((**it).encodedData(), sizeOfEncodedData);	
		}
		else
		{
			Log::info() << "MDSetTool::run: OTHER ORDER " << sizeOfEncodedData << std::endl;
			
            serializeHeader<OtherByteOrder,DataHandle>(*outHandle, sizeOfEncodedData, md.rowsNumber(), it->properties(), md);
			DataStream<OtherByteOrder,DataHandle>(*outHandle).writeBytes((**it).encodedData(), sizeOfEncodedData);	
		}
	}
}

// 
//static std::vector<std::string> split(const std::string& delim, const std::string& text);

void MDSetTool::parseUpdateList(const std::string& s,
                                std::vector<std::string>& columns,
                                std::vector<std::string>& types,
                                std::vector<std::string>& values,
                                std::vector<eckit::sql::BitfieldDef>& bitfieldDefs)
{
    std::vector<std::string> assignments(S::split(",", s));
	for (size_t i = 0; i < assignments.size(); ++i)
	{
		vector<string> assignment(S::split("=", assignments[i]));
        string value (assignment.size() == 2 ? assignment[1] : "NONE");
        vector<string> columnNameAndType(S::split(":", assignment[0]));
        string type (columnNameAndType.size() == 2 ? columnNameAndType[1] : "NONE");
        string column (assignment[0]);
	
        eckit::sql::BitfieldDef bf;
        if (type.size() && type[0] == '[' && type[type.size() - 1] == ']')
        {
            std::vector<std::string> parts(StringTools::split(";", type.substr(1, type.size() - 2)));
            for (size_t p = 0; p < parts.size(); ++p)
            {   
                std::vector<std::string> field (S::split(":", parts[p]));
                bf.first.push_back(field[0]);
                bf.second.push_back(atoi(field[1].c_str()));
            }   
        }

		Log::info() << "MDSetTool::parseUpdateList: " << column << " : " << type << " = '" << value << "'" << std::endl;

		columns.push_back(column);
		types.push_back(type);
		values.push_back(value);
        bitfieldDefs.push_back(bf);
	}
    ASSERT(columns.size() == types.size());
    ASSERT(columns.size() == values.size());
    ASSERT(columns.size() == bitfieldDefs.size());
}

} // namespace tool 
} // namespace odb 

