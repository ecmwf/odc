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

namespace odb {

namespace sql { class SQLSession; class SQLParser; class SQLOutputConfig; }

namespace tool {

class SQLTool : public Tool {

public:
	SQLTool(int argc, char **argv);
	~SQLTool();
	virtual void run();
    static void execute(const string&, ostream& = std::cout);
	static void help(ostream &o) { o << "Executes SQL statement"; }

	static void usage(const string& name, ostream &o)
	{
		o << name << " <select-statement> | <script-filename>" << endl;
        o << "             [-T]                  Disables printing of column names" << endl;
        o << "             [-offset <offset>]    Start processing file at a given offset" << endl;
        o << "             [-length <length>]    Process only given bytes of data" << endl;
        o << "             [-N]                  Do not write NULLs, but proper missing data values" << endl;
        o << "             [-i <inputfile>]      ODB input file" << endl;
        o << "             [-o <outputfile>]     ODB output file" << endl;
        o << "             [-f default|wide|odb] ODB output format (default is ascii; odb is binary ODB, wide is ascii with"
												<< " bitfields definitions in header)" << endl;
        o << "             [-delimiter <delim>]  Changes the default values' delimiter (TAB by default)" << endl; 
        o << "                                   delim can be any character or string" << endl;
        o << "             [--binary]            Print bitfields in binary notation" << endl;
        o << "             [--no_alignment]      Do not align columns" << endl;
	}

private:
	static void runSQL(const string&,
                const eclib::PathName&,
                odb::sql::SQLSession&,
                odb::sql::SQLParser&,
                const odb::sql::SQLOutputConfig&,
                const eclib::Offset& offset = eclib::Offset(0),
                const eclib::Length& length = eclib::Length(0));

	bool doNotWriteColumnNames_; // -T
	bool doNotWriteNULL_;        // -N
	string delimiter_;           // -delimiter
	string inputFile_;           // -i
	string outputFile_;          // -o
	string outputFormat_;        // default is ascii
	eclib::Offset offset_;       // -offset
	eclib::Length length_;       // -length
};

} // namespace tool 
} // namespace odb 

#endif

