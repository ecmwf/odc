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
#include "odb_api/migrator/ImportODBTool.h"
#include "odb_api/migrator/ReptypeGenIterator.h"
#include "odb_api/Comparator.h"
#include "odb_api/DispatchingWriter.h"
#include "odb_api/SQLDatabase.h"
#include "odb_api/SQLInteractiveSession.h"
#include "odb_api/SchemaAnalyzer.h"
#include "odb_api/SchemaAnalyzer.h"
#include "odb_api/odb_api.h"
#include "odb_api/RowsCounter.h"
#include "odb_api/tools/ODA2RequestTool.h"
#include "odb_api/tools/Tool.h"
#include "odb_api/tools/ToolFactory.h"

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
: Tool(argc, argv),
  noVerification_ (optionIsSet("-no_verification"))
{}

template <typename IN>
ImportODBTool<IN>::ImportODBTool (const CommandLineParser& clp)
: Tool(clp),
  noVerification_ (optionIsSet("-no_verification"))
{}

template <typename IN>
std::pair<unsigned long long, const std::vector<eckit::PathName> > ImportODBTool<IN>::importDispatching(const eckit::PathName& db, const std::string& sql, const std::string& dumpFile)
{    
    using namespace eckit;

    Timer importingAndDipatching("Importing and dipatching");

    Log::info() << "Importing data from '" << db
        << "', query is '" << sql << "', into '"
        << dumpFile << "' template." << std::endl;

    odb::DispatchingWriter writer(dumpFile);
    odb::DispatchingWriter::iterator w = writer.begin();

    unsigned long long inRowsNumber = saveData<>(w, db, sql);

    std::vector<eckit::PathName> files = (**w).outputFiles();
    return std::make_pair(inRowsNumber, files);
}

template <typename IN>
void ImportODBTool<IN>::validate(const eckit::PathName &db, const std::string& sql, const eckit::PathName& file)
{
    eckit::Timer verification("Validating dispatched output");

    odb::Reader odaReader(file);
    odb::Reader::iterator r(odaReader.begin());

    IN reader(db, sql);
    typename IN::iterator begin (reader.begin());
    const typename IN::iterator end (reader.end());

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

	unsigned long long outRowsNumber (0);
	unsigned long long outFilesSize (0);
	for (size_t i = 0; i < files.size(); ++i)
	{
		const PathName &fn (files[i]);
		unsigned long long n (RowsCounter::rowCount(fn));
		unsigned long long fileSize (fn.size());

		outRowsNumber += n;
		outFilesSize += fileSize;

		Log::info() << "ImportODBTool::validateRowsNumber: " << fn << ": " << BigNum(n) << " rows, file size: " << BigNum(fileSize) << "." << std::endl;
	}
	Log::info() << "ImportODBTool::validateRowsNumber: sum of output rows number: " << BigNum(outRowsNumber) << ", sum of file sizes: " << BigNum(outFilesSize) << std::endl;
	ASSERT(inRowsNumber == outRowsNumber);
}


template <typename IN>
void ImportODBTool<IN>::run()
{
    using namespace eckit;
    
    ASSERT("Wrong number of parameters. odb2oda.cc:main should check this."
        && !(parameters().size() < 2 || parameters().size() > 4));

    str db (parameters(1));
    str sql (parameters().size() > 2 && parameters(2) != "." ? readFile(parameters(2)) : "");

    Log::info() << "ImportODBTool::run: sql='" << sql << "'" << std::endl;

    str dumpFile ((parameters().size() > 3) ? parameters(3) : (db + ".odb"));
    if (dumpFile.substr(dumpFile.size() - 4) != ".odb")
        dumpFile.append(".odb");

    Log::info() << "Importing data from '" << db << "', query is '" << sql << "', into '" << dumpFile << "'." << std::endl;

    odb::TemplateParameters params;
    if (odb::TemplateParameters::parse(dumpFile, params).size() != 0)
    {
        DispatchResult r (importDispatching(db, sql, dumpFile));

        unsigned long long importedRowsNumber (r.first);
        const std::vector<eckit::PathName>& outFiles (r.second);

        if (! noVerification_ )
        {
            Timer verification("Verification");
            validateRowsNumber(importedRowsNumber, outFiles);
        }
    }
    else
    {
        odb::Writer<> writer(dumpFile);
        odb::Writer<>::iterator w (writer.begin());

        unsigned long long importedRowsNumber (saveData<>(w, db, sql)); 
        Log::info() << "Imported " << BigNum(importedRowsNumber) << " row(s)." << std::endl;

        Timer verification("Verification");
        Log::info() << "Verifying." << std::endl;
        Log::info() << "Comparing data from: 1) ODB, and 2) ODA" << std::endl;

        if (importedRowsNumber && ! noVerification_) 
            validate(db, sql, dumpFile);
    }
    Log::info() << "ImportODBTool: Finished OK" << std::endl;
}

template <typename IN>
template <typename OUT_ITERATOR>
unsigned long long ImportODBTool<IN>::saveData(OUT_ITERATOR w, eckit::PathName odb, str sql) //, const SchemaAnalyzer &schema)
{
    using namespace eckit;
    
    Log::info() << "ImportODBTool<IN>::saveData: odb='" << odb << "', sql='" << sql << "'" << std::endl;
    unsigned long long n (0);
	try {
        (**w).property("ODB_DATABASE", odb);
        IN reader (odb, sql);
        typename IN::iterator begin (reader.begin());
        const typename IN::iterator end (reader.end());
		
        if (begin->columns().empty())
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


} // namespace tool 
} // namespace odb 


