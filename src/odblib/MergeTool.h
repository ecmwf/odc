/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef MergeTool_H
#define MergeTool_H

//#include "eckit/filesystem/PathName.h"

namespace odb {
namespace tool {

class MergeTool : public Tool {
public:
	MergeTool (int argc, char *argv[]); 

	void run(); 

	static void help(std::ostream &o) { o << "Merges rows from files"; }
	static void usage(const std::string& name, std::ostream &o)
	{
		o << endl
          << name << " -o <output-file.odb> <input1.odb> <input2.odb> ..." << endl
                  << ""                                                    << endl
                  << " or "                                                << endl
                  << ""                                                    << endl
          << name << " -S -o <output-file.odb> <input1.odb> <sql-select1> <input2.odb> <sql-select2> ..." << endl
                  ;
	}

	static void merge(const std::vector<eckit::PathName>& inputFiles, const eckit::PathName& outputFileName);
	static void merge(const std::vector<eckit::PathName>& inputFiles, const std::vector<std::string>& sqls, const eckit::PathName& outputFileName);

private:
// No copy allowed

    MergeTool(const MergeTool&);
    MergeTool& operator=(const MergeTool&);

	static char* dummyArgv_[];

	std::vector<eckit::PathName> inputFiles_;
    std::vector<std::string> sql_;
	eckit::PathName outputFile_;
    bool sqlFiltering_;

};

} // namespace tool 
} // namespace odb 

#endif 
