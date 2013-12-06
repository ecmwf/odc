/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef _SQLTool_H
#define _SQLTool_H

#include "odblib/Tool.h"

namespace odb {

namespace sql { class SQLSession; class SQLParser; class SQLOutputConfig; }

namespace tool {

class SQLTool : public Tool {

public:
	SQLTool(int argc, char **argv);
	~SQLTool();
	virtual void run();
    static void execute(const std::string&, std::ostream& = std::cout);
	static void help(std::ostream &o) { o << "Executes SQL statement"; }

	static void usage(const std::string& name, std::ostream &o)
	{
		o << name << " <select-statement> | <script-filename>" << std::endl;
        o << "             [-T]                  Disables printing of column names" << std::endl;
        o << "             [-offset <offset>]    Start processing file at a given offset" << std::endl;
        o << "             [-length <length>]    Process only given bytes of data" << std::endl;
        o << "             [-N]                  Do not write NULLs, but proper missing data values" << std::endl;
        o << "             [-i <inputfile>]      ODB input file" << std::endl;
        o << "             [-o <outputfile>]     ODB output file" << std::endl;
        o << "             [-f default|wide|odb] ODB output format (default is ascii; odb is binary ODB, wide is ascii with"
												<< " bitfields definitions in header)" << std::endl;
        o << "             [-delimiter <delim>]  Changes the default values' delimiter (TAB by default)" << std::endl; 
        o << "                                   delim can be any character or std::string" << std::endl;
	}

private:
	static void runSQL(const std::string&,
                const eckit::PathName&,
                odb::sql::SQLSession&,
                odb::sql::SQLParser&,
                const odb::sql::SQLOutputConfig&,
                const eckit::Offset& offset = eckit::Offset(0),
                const eckit::Length& length = eckit::Length(0));

	bool doNotWriteColumnNames_; // -T
	bool doNotWriteNULL_;        // -N
	std::string delimiter_;           // -delimiter
	std::string inputFile_;           // -i
	std::string outputFile_;          // -o
	std::string outputFormat_;        // default is ascii
	eckit::Offset offset_;       // -offset
	eckit::Length length_;       // -length
};

} // namespace tool 
} // namespace odb 

#endif

