/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <fstream>
#include <memory>
#include <ostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/FileDescHandle.h"
#include "eckit/io/FileHandle.h"
#include "eckit/io/Length.h"
#include "eckit/io/PartFileHandle.h"
#include "eckit/utils/StringTools.h"

#include "eckit/sql/SQLParser.h"
#include "eckit/sql/SQLSelectFactory.h"
#include "eckit/sql/SQLSession.h"
#include "eckit/sql/SQLStatement.h"
#include "eckit/types/Types.h"

#include "odc/sql/SQLOutputConfig.h"
#include "odc/sql/TODATable.h"
#include "odc/tools/SQLTool.h"

using namespace std;
using namespace eckit;

namespace odc {
namespace tool {

//----------------------------------------------------------------------------------------------------------------------

SQLTool::SQLTool(int argc, char** argv) : Tool(argc, argv) {

    registerOptionWithArgument("-o");
    registerOptionWithArgument("-i");
    registerOptionWithArgument("-I");
    registerOptionWithArgument("-delimiter");
    registerOptionWithArgument("-f");  // output format
    registerOptionWithArgument("-offset");
    registerOptionWithArgument("-length");

    if ((inputFile_ = optionArgument("-i", std::string(""))) == "-")
        inputFile_ = "/dev/stdin";

    offset_ = optionArgument("-offset", (long)0);  // FIXME@ optionArgument should accept unsigned long etc
    length_ = optionArgument("-length", (long)0);

    // Configure the output

    bool noColumnNames         = optionIsSet("-T");
    bool noNULL                = optionIsSet("-N");
    std::string fieldDelimiter = optionArgument("-delimiter", std::string("\t"));
    std::string outputFormat   = optionArgument("-f", std::string(eckit::sql::SQLOutputConfig::defaultOutputFormat));
    bool bitfieldsBinary       = optionIsSet("--bin") || optionIsSet("--binary");
    //    bool bitfieldsHex = optionIsSet("--hex") || optionIsSet("--hexadecimal");
    bool noColumnAlignment = optionIsSet("--no_alignment");
    bool fullPrecision     = optionIsSet("--full_precision") || optionIsSet("--full-precision");


    sqlOutputConfig_.reset(new odc::sql::SQLOutputConfig(noColumnNames, noNULL, fieldDelimiter, outputFormat,
                                                         bitfieldsBinary, noColumnAlignment, fullPrecision));

    // Configure the output file

    std::string outputFile = optionArgument("-o", std::string(""));
    if (outputFile == "-")
        outputFile = "/dev/stdout";

    if (!outputFile.empty()) {
        sqlOutputConfig_->setOutputFile(outputFile);
    }
}

SQLTool::~SQLTool() {}

void SQLTool::run() {
    if (parameters().size() < 2) {
        Log::error() << "Usage: ";
        usage(parameters(0), Log::error());
        Log::error() << std::endl;
        std::stringstream ss;
        ss << "Expected at least 2 command line parameters";
        throw UserError(ss.str());
    }

    if (sqlOutputConfig_->outputFormat() == "odb" && !optionIsSet("-o")) {
        throw UserError("Output file is required (option -o) for binary output format (option -f odb)");
    }

    std::vector<std::string> params(parameters());
    params.erase(params.begin());

    std::string sql(StringTool::isSelectStatement(params[0])
                        ? StringTools::join(" ", params) + ";"
                        // FIXME:
                        : StringTool::readFile(params[0] == "-" ? "/dev/tty" : params[0]) + ";");


    std::unique_ptr<std::ofstream> outStream;
    if (optionIsSet("-o") && sqlOutputConfig_->outputFormat() != "odb") {
        outStream.reset(new std::ofstream(optionArgument("-o", std::string("")).c_str()));
        sqlOutputConfig_->setOutputStream(*outStream);
    }

    // Configure the session to include any specified ODB file

    eckit::sql::SQLSession session(std::move(sqlOutputConfig_));  // n.b. invalidates sqlOutputConfig_
    std::unique_ptr<eckit::DataHandle> implicitTableDH;
    std::unique_ptr<AutoClose> implicitCloser;

    if (!inputFile_.empty()) {
        if (inputFile_ == "/dev/stdin" || inputFile_ == "stdin") {
            Log::info() << "Reading table from standard input" << std::endl;
            implicitTableDH.reset(new FileDescHandle(0));
            NOTIMP;  // Is this working?
            /// parser.parseString(session, sql, &fh, config);
        }
        else if (offset_ == eckit::Offset(0)) {
            implicitTableDH.reset(new FileHandle(inputFile_));
        }
        else {
            implicitTableDH.reset(new PartFileHandle(inputFile_, offset_, length_));
        }

        implicitTableDH->openForRead();
        implicitCloser.reset(new AutoClose(*implicitTableDH));

        eckit::sql::SQLDatabase& db(session.currentDatabase());
        db.addImplicitTable(new odc::sql::ODATable(db, *implicitTableDH));
    }

    // And actually do the SQL!

    eckit::sql::SQLParser parser;
    parser.parseString(session, sql);
    session.statement().execute();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace tool
}  // namespace odc
