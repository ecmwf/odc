#include "oda.h"
#include "PathName.h"
#include "BigNum.h"

#include "Tool.h"
#include "ToolFactory.h"
#include "Comparator.h"
#include "SQLDatabase.h"
#include "SchemaAnalyzer.h"
#include "SQLInteractiveSession.h"

#include "ImportODBTool.h"
#include "CountTool.h"
#include "ODA2RequestTool.h"
#include "ReptypeGenIterator.h"

#define SRC __FILE__, __LINE__

#include <iostream>
#include <fstream>

extern "C" {
#include "odbdump.h"
}

#include "ODBIterator.h"
#include "FakeODBIterator.h"

#include "ksh.h"

namespace odb {
namespace tool {

typedef std::string str;   // string is a typedef in an ODB header.

template <typename IN>
ImportODBTool<IN>::ImportODBTool (int argc, char *argv[])
: Tool(argc, argv)
{}

template <typename IN>
ImportODBTool<IN>::ImportODBTool (const Application *app)
: Tool(app)
{}

template <typename IN>
ImportODBTool<IN>::ImportODBTool (const CommandLineParser& clp)
: Tool(clp)
{}

//ToolFactory<ImportODBTool> importODB("importodb");

template <typename IN>
pair<unsigned long long, const vector<PathName> > ImportODBTool<IN>::importDispatching(PathName db, const std::string& sql, const std::string& dumpFile)
{
	Timer importingAndDipatching("Importing and dipatching");

	Log::info() << "Importing data from '" << db
		<< "', query is '" << sql << "', into '"
		<< dumpFile << "' template." << endl;

	odb::DispatchingWriter writer(dumpFile);
	odb::DispatchingWriter::iterator w = writer.begin();

	unsigned long long inRowsNumber = saveData<>(w, db, sql);

	vector<PathName> files = (**w).getFiles();
	return make_pair(inRowsNumber, files);
}

template <typename IN>
void ImportODBTool<IN>::validate(PathName db, const std::string& sql, const PathName& file)
{
	Timer verification("Validating dispatched output");

	odb::Reader odaReader(file);
	odb::Reader::iterator r(odaReader.begin());

	IN reader(db, sql);
	typename IN::iterator begin = reader.begin();
	const typename IN::iterator end = reader.end();

	 // don't check missing value as the old ODB is not giving correct info for that some times (e.g.  sortbox)
	odb::Comparator(false)
		.compare(begin, end, r, odaReader.end(),
			std::string("ODB input ") + db, std::string("converted output ") + file);
}


template <typename IN>
void ImportODBTool<IN>::validateRowsNumber(unsigned long long inRowsNumber, const vector<PathName>& files)
{
	Timer verification("Validating dispatched output");

	Log::info() << "ImportODBTool::validateRowsNumber: Validating output. " << endl;
	Log::info() << "ImportODBTool::validateRowsNumber: input rows number: " << BigNum(inRowsNumber) << endl;

	unsigned long long outRowsNumber = 0;
	unsigned long long outFilesSize = 0;
	for (size_t i = 0; i < files.size(); ++i)
	{
		const PathName &fn = files[i];
		unsigned long long n = CountTool::rowCount(fn);
		unsigned long long fileSize = fn.size();

		outRowsNumber += n;
		outFilesSize += fileSize;

		Log::info() << "ImportODBTool::validateRowsNumber: " << fn << ": " << BigNum(n) << " rows, file size: " << BigNum(fileSize) << "." << endl;
	}
	Log::info() << "ImportODBTool::validateRowsNumber: sum of output rows number: " << BigNum(outRowsNumber) << ", sum of file sizes: " << BigNum(outFilesSize) << endl;
	ASSERT(inRowsNumber == outRowsNumber);
}


template <typename IN>
void ImportODBTool<IN>::archiveFiles(const vector<PathName>& files)
{
	for (size_t i = 0; i < files.size(); ++i)
	{
		PathName fn = files[i];
		ODA2RequestTool oda2request;
		str request = oda2request.generateMarsRequest(fn);
		Log::info() << "ImportODBTool::archiveFiles: request to archive '" << fn << "': " << endl << request << endl;
		
		PathName requestFile = fn + ".archive.request";
		FileHandle rf(requestFile);
		rf.openForWrite(request.size());
		rf.write(request.c_str(), request.size());
		rf.close();

		str cmd = "mars -t ";
		cmd += requestFile;
		ksh(cmd, SRC, false);
	}
}

template <typename IN>
void ImportODBTool<IN>::run()
{
	ASSERT("Wrong number of parameters. odb2oda.cc:main should check this."
		&& !(parameters().size() < 2 || parameters().size() > 4));

	str db = parameters(1);
	if (isECFSPathName(db))
	{
		// TODO: Check if it is already in cache (local disk).
		db = readFromECFS(db);
	}
	
	//const SchemaAnalyzer &schema = getSchema(db);

	//If the second param is '.' then generate select using ODB's schema
	str sql = parameters().size() > 2 && parameters(2) != "." ? readFile(parameters(2)) : "";//defaultSQL(db);
	str dumpFile = (parameters().size() > 3) ? parameters(3) : (db + ".odb");

	if (dumpFile.substr(dumpFile.size() - 4) != ".odb")
		dumpFile.append(".odb");

	Log::info() << "Importing data from '" << db
		<< "', query is '" << sql << "', into '"
		<< dumpFile << "'." << endl;

	odb::TemplateParameters params;
	if (odb::TemplateParameters::parse(dumpFile, params).size() != 0)
	{
		DispatchResult r = importDispatching(db, sql, dumpFile);

		unsigned long long importedRowsNumber = r.first;
		const vector<PathName>& outFiles = r.second;

		Timer verification("Verification");
		validateRowsNumber(importedRowsNumber, outFiles);

		if(0) archiveFiles(outFiles);
	}
	else
	{

		odb::Writer<> writer(dumpFile);
		odb::Writer<>::iterator w = writer.begin();

		unsigned long long importedRowsNumber = saveData<>(w, db, sql); //, schema);
		Log::info() << "Imported " << BigNum(importedRowsNumber) << " row(s)." << endl;

		Timer verification("Verification");
		Log::info() << "Verifying." << endl;
		Log::info() << "Comparing data from: 1) ODB, and 2) ODA" << endl;

		validate(db, sql, dumpFile);
	}
	Log::info() << "ImportODBTool: Finished OK" << endl;
}

template <typename IN>
template <typename OUT_ITERATOR>
unsigned long long ImportODBTool<IN>::saveData(OUT_ITERATOR w, PathName odb, str sql) //, const SchemaAnalyzer &schema)
{
	unsigned long long n = 0;
	try {
		(**w).property("ODB_DATABASE", odb);
		//w->property("ODB_SCHEMA", readFile(schemaFile(odb), false));
		//w->property("SELECTED_DATA", sql);
		IN reader (odb, sql);
		typename IN::iterator begin = reader.begin();
		const typename IN::iterator end = reader.end();
		
		// Unfortunately we don't get any info about bitfields definitions from ODB directly, so:
		//w->columns() = it.columns();
		//schema.updateBitfieldsDefs(w->columns());

		n = w->pass1(begin, end);
		//w->close();
	} catch (...) {
		ksh("cat odbdump.stderr && cp odbdump.stderr " + odb + ".odb.log", SRC);
		throw;
	}
	return n;
}

template <typename IN>
bool ImportODBTool<IN>::isECFSPathName(const PathName fileName)
{
	return str(fileName).find("era40:") == 0;
}

/// Reads a file from ec:/ERAS/era40/EXPVER1/$yyyy/$yyyymmddhh
/// where
///  yyyy: <1957,2002>
///  hh:   00,06,12,18
///
/// @param fileName  e.g. "era40:2002080100"
///
template <typename IN>
PathName ImportODBTool<IN>::readFromECFS(const PathName fileName)
{
	ASSERT(isECFSPathName(fileName));

	str fn = fileName;

	str datetime = fn.substr(6);
	Log::info() << "datetime: " << datetime << endl;

	ASSERT("Time format must be: yyyymmddhh" && datetime.size() == 10);
	
	str year = datetime.substr(0, 4);
	Log::info() << "year: " << year << endl;

	str ecfsPath = str("ec:/ERAS/era40/EXPVER1/") + year + "/" + datetime + "/ECMA.tar";
	PathName localPath = datetime + ".ECMA.tar";

	if (localPath.exist() && localPath.isDir())
		Log::info() << "Directory " << localPath
			<< " already exists. Skipping copying from ECFS." << endl;
	else
	{
		str cmd = "$ECFS_SYS_PATH/ecp.p " + ecfsPath + " " + localPath;
		ksh(cmd, SRC);
	}

	PathName unpackDir(datetime);
	if (unpackDir.exist() && unpackDir.isDir())
		Log::info() << "Directory " << unpackDir << " exist, skipping unpacking." << endl;
	else
	{
		Log::info() << "Creating directory '" << unpackDir << "'" << endl;
		str cmd = "mkdir " + unpackDir;
		ksh(cmd, SRC, false);

		Log::info() << "Unpacking '" << localPath << "' to '" << unpackDir << "'" << endl;
		cmd = "cd " + datetime + " && tar xvf ../" + localPath;
		ksh(cmd, SRC);
	}

	str localODB = unpackDir + "/ECMA";

	return localODB;
}

} // namespace tool 
} //namespace odb 


