/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <iostream>
#include <fstream>

#include "eckit/filesystem/LocalPathName.h"
#include "eckit/io/FileHandle.h"
#include "eckit/log/BigNum.h"
#include "eckit/log/Timer.h"
#include "migrator/ImportODBTool.h"
#include "migrator/ReptypeGenIterator.h"
#include "odb_api/Comparator.h"
#include "odb_api/DispatchingWriter.h"
#include "odb_api/SQLDatabase.h"
#include "odb_api/SQLInteractiveSession.h"
#include "odb_api/SchemaAnalyzer.h"
#include "odb_api/SchemaAnalyzer.h"
#include "odb_api/odb_api.h"
#include "tools/CountTool.h"
#include "tools/ODA2RequestTool.h"
#include "tools/Tool.h"
#include "tools/ToolFactory.h"


extern "C" {
#include "odbdump.h"
}

#include "ODBIterator.h"
#include "FakeODBIterator.h"

namespace odb {
namespace tool {

typedef std::string str;   // string is a typedef in an ODB header.

template <typename IN>
ImportODBTool<IN>::ImportODBTool (int argc, char *argv[])
: Tool(argc, argv)
{}

template <typename IN>
ImportODBTool<IN>::ImportODBTool (const CommandLineParser& clp)
: Tool(clp)
{}

//ToolFactory<ImportODBTool> importODB("importodb");

template <typename IN>
std::pair<unsigned long long, const std::vector<eckit::PathName> > ImportODBTool<IN>::importDispatching(eckit::PathName db, const std::string& sql, const std::string& dumpFile)
{    
    using namespace eckit;

    Timer importingAndDipatching("Importing and dipatching");

	Log::info() << "Importing data from '" << db
		<< "', query is '" << sql << "', into '"
		<< dumpFile << "' template." << std::endl;

	odb::DispatchingWriter writer(dumpFile);
	odb::DispatchingWriter::iterator w = writer.begin();

	unsigned long long inRowsNumber = saveData<>(w, db, sql);

    std::vector<eckit::PathName> files = (**w).getFiles();
    return std::make_pair(inRowsNumber, files);
}

template <typename IN>
void ImportODBTool<IN>::validate(eckit::PathName db, const std::string& sql, const eckit::PathName& file)
{
	eckit::Timer verification("Validating dispatched output");

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
void ImportODBTool<IN>::validateRowsNumber(unsigned long long inRowsNumber, const std::vector<eckit::PathName>& files)
{
    using namespace eckit;
    
	Timer verification("Validating dispatched output");

	Log::info() << "ImportODBTool::validateRowsNumber: Validating output. " << std::endl;
	Log::info() << "ImportODBTool::validateRowsNumber: input rows number: " << BigNum(inRowsNumber) << std::endl;

	unsigned long long outRowsNumber = 0;
	unsigned long long outFilesSize = 0;
	for (size_t i = 0; i < files.size(); ++i)
	{
		const PathName &fn = files[i];
		unsigned long long n = CountTool::rowCount(fn);
		unsigned long long fileSize = fn.size();

		outRowsNumber += n;
		outFilesSize += fileSize;

		Log::info() << "ImportODBTool::validateRowsNumber: " << fn << ": " << BigNum(n) << " rows, file size: " << BigNum(fileSize) << "." << std::endl;
	}
	Log::info() << "ImportODBTool::validateRowsNumber: sum of output rows number: " << BigNum(outRowsNumber) << ", sum of file sizes: " << BigNum(outFilesSize) << std::endl;
	ASSERT(inRowsNumber == outRowsNumber);
}


template <typename IN>
void ImportODBTool<IN>::archiveFiles(const std::vector<eckit::PathName>& files)
{
    using namespace eckit;
    
	for (size_t i = 0; i < files.size(); ++i)
	{
		PathName fn = files[i];
		ODA2RequestTool oda2request;
		str request = oda2request.generateMarsRequest(fn);
        Log::info() << "ImportODBTool::archiveFiles: request to archive '" << fn << "': " << std::endl << request << std::endl;
		
		PathName requestFile = fn + ".archive.request";
		FileHandle rf(requestFile);
		rf.openForWrite(request.size());
		rf.write(request.c_str(), request.size());
		rf.close();

		str cmd = "mars -t ";
		cmd += requestFile;
		shell(cmd, Here(), false);
	}
}

template <typename IN>
void ImportODBTool<IN>::run()
{
    using namespace eckit;
    
	ASSERT("Wrong number of parameters. odb2oda.cc:main should check this."
		&& !(parameters().size() < 2 || parameters().size() > 4));

	str db = parameters(1);
	if (isECFSPathName(db))
	{
		// TODO: Check if it is already in cache (local disk).
		db = readFromECFS(db);
	}
	
	str sql = parameters().size() > 2 && parameters(2) != "." ? readFile(parameters(2)) : "";

	Log::info() << "ImportODBTool::run: sql='" << sql << "'" << std::endl;

	str dumpFile = (parameters().size() > 3) ? parameters(3) : (db + ".odb");

	if (dumpFile.substr(dumpFile.size() - 4) != ".odb")
		dumpFile.append(".odb");

	Log::info() << "Importing data from '" << db
		<< "', query is '" << sql << "', into '"
		<< dumpFile << "'." << std::endl;

	odb::TemplateParameters params;
	if (odb::TemplateParameters::parse(dumpFile, params).size() != 0)
	{
		DispatchResult r = importDispatching(db, sql, dumpFile);

		unsigned long long importedRowsNumber = r.first;
        const std::vector<eckit::PathName>& outFiles = r.second;

		Timer verification("Verification");
		validateRowsNumber(importedRowsNumber, outFiles);

		if(0) archiveFiles(outFiles);
	}
	else
	{

		odb::Writer<> writer(dumpFile);
		odb::Writer<>::iterator w = writer.begin();

		unsigned long long importedRowsNumber = saveData<>(w, db, sql); //, schema);
		Log::info() << "Imported " << BigNum(importedRowsNumber) << " row(s)." << std::endl;

		Timer verification("Verification");
		Log::info() << "Verifying." << std::endl;
		Log::info() << "Comparing data from: 1) ODB, and 2) ODA" << std::endl;

		if (importedRowsNumber) validate(db, sql, dumpFile);
	}
	Log::info() << "ImportODBTool: Finished OK" << std::endl;
}

template <typename IN>
template <typename OUT_ITERATOR>
unsigned long long ImportODBTool<IN>::saveData(OUT_ITERATOR w, eckit::PathName odb, str sql) //, const SchemaAnalyzer &schema)
{
    using namespace eckit;
    
	Log::info() << "ImportODBTool<IN>::saveData: odb='" << odb << "', sql='" << sql << "'" << std::endl;
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
		if ( &(begin->columns()) == 0 )
		{
			Log::warning() << "ImportODBTool<IN>::saveData: empty input data set." << std::endl;
			return 0;
		}

		n = w->pass1(begin, end);
		//w->close();
	} catch (...) {
		shell("[ -f odbdump.stderr ] && cat odbdump.stderr && cp odbdump.stderr " + odb + ".odb.log || echo odbdump.stderr not found", Here()); 
		throw;
	}
	return n;
}

template <typename IN>
bool ImportODBTool<IN>::isECFSPathName(const eckit::PathName fileName)
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
eckit::PathName ImportODBTool<IN>::readFromECFS(const eckit::PathName fileName)
{
    using namespace eckit;

    ASSERT(isECFSPathName(fileName));

	str fn = fileName;

	str datetime = fn.substr(6);
	Log::info() << "datetime: " << datetime << std::endl;

	ASSERT("Time format must be: yyyymmddhh" && datetime.size() == 10);
	
	str year = datetime.substr(0, 4);
	Log::info() << "year: " << year << std::endl;

	str ecfsPath = str("ec:/ERAS/era40/EXPVER1/") + year + "/" + datetime + "/ECMA.tar";
	LocalPathName localPath = datetime + ".ECMA.tar";

	if (localPath.exists() && localPath.isDir())
		Log::info() << "Directory " << localPath
			<< " already exists. Skipping copying from ECFS." << std::endl;
	else
	{
		str cmd = "$ECFS_SYS_PATH/ecp.p " + ecfsPath + " " + localPath;
		shell(cmd, Here());
	}

	LocalPathName unpackDir(datetime);
	if (unpackDir.exists() && unpackDir.isDir())
		Log::info() << "Directory " << unpackDir << " exist, skipping unpacking." << std::endl;
	else
	{
		Log::info() << "Creating directory '" << unpackDir << "'" << std::endl;
		PathName(unpackDir).mkdir();

		Log::info() << "Unpacking '" << localPath << "' to '" << unpackDir << "'" << std::endl;
		str cmd = "cd " + datetime + " && tar xvf ../" + localPath;
		shell(cmd, Here());
	}

	str localODB = unpackDir + "/ECMA";

	return localODB;
}

} // namespace tool 
} // namespace odb 


