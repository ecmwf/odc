#include "eclib/Tokenizer.h"

#include "odblib/oda.h"
#include "odblib/Tool.h"
#include "odblib/ToolFactory.h"
#include "odblib/MetaDataReader.h"
#include "odblib/MDSetTool.h"
#include "odblib/DataStream.h"

namespace odb {
namespace tool {

ToolFactory<MDSetTool> mdset("mdset");

MDSetTool::MDSetTool (int argc, char *parameters[]) : Tool(argc, parameters) { }

void MDSetTool::run()
{
	if (parameters().size() != 4)
	{
		Log::error() << "Usage: ";
		usage(parameters(0), Log::error());
		Log::error() << endl;
		return;
	}

	vector<string> columns;
	vector<string> types;

	PathName inFile = parameters(2);

	PathName outFile = parameters(3);
	FileHandle outHandle(outFile, true);
	outHandle.openForWrite(outFile.exists() ? outHandle.estimate() : Length(0) );

	parseUpdateList(parameters(1), columns, types);
    ASSERT(columns.size() == types.size());

	vector<BitfieldDef> bitfieldDefs;
    for (size_t i = 0; i < types.size(); ++i)
    {   
        Log::info() << columns[i] << " : " << types[i] << endl;

        // Only bitfoelds now:
        // [active:1;passive:1;rejected:1;blacklisted:1;use_emiskf_only:1;monthly:1;constant:1;experimental:1;whitelist:]
        ASSERT(types[i].size());
        ASSERT(types[i][0] == '[');
        ASSERT(types[i][types[i].size() - 1] == ']');

        BitfieldDef bf; 
        vector<string> parts(StringTool::split(";", types[i].substr(1, types[i].size() - 2)));
        for (size_t p = 0; p < parts.size(); ++p)
        {   
            vector<string> field = StringTool::split(":", parts[p]);
            bf.first.push_back(field[0]);
            bf.second.push_back(atoi(field[1].c_str()));
        }   
        bitfieldDefs.push_back(bf);
        Log::info() << "" << i << ": " << columns[i] << " - " << bf.first << endl; // "[" << bf.second << "]" << endl;
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
			Log::info() << "MDSetTool::run: SAME ORDER " << sizeOfEncodedData << endl;

			serializeHeader<SameByteOrder,DataHandle>(outHandle, sizeOfEncodedData, md.rowsNumber(), props, md);
			DataStream<SameByteOrder,DataHandle>(outHandle).writeBytes((**it).encodedData(), sizeOfEncodedData);	
		}
		else
		{
			Log::info() << "MDSetTool::run: OTHER ORDER " << sizeOfEncodedData << endl;
			
			serializeHeader<OtherByteOrder,DataHandle>(outHandle, sizeOfEncodedData, md.rowsNumber(), props, md);
			DataStream<OtherByteOrder,DataHandle>(outHandle).writeBytes((**it).encodedData(), sizeOfEncodedData);	
		}
	}

}

void MDSetTool::parseUpdateList(string s, vector<string>& columns, vector<string>& values)
{
    Tokenizer splitAssignments(",");
    vector<string> assignments;
    splitAssignments(s, assignments);
	
    Tokenizer splitEq("=");

	for (size_t i = 0; i < assignments.size(); ++i)
	{
		vector<string> assignment;
		splitEq(assignments[i], assignment);
		ASSERT(assignment.size() == 2);

		string colName = assignment[0];
		string value = assignment[1];
		
		Log::info() << "MDSetTool::parseUpdateList: " << colName << "='" << value << "'" << endl;

		columns.push_back(colName);
		values.push_back(value);
	}
}

} // namespace tool 
} // namespace odb 

