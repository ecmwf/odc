/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef MergeTool_H
#define MergeTool_H

#include "odb/Tool.h"

namespace odb {
namespace tool {

class MergeTool : public Tool {
public:
	MergeTool (int argc, char *argv[]); 

	void run(); 

	static void help(ostream &o) { o << "Merges rows from two files"; }
	static void usage(const string& name, ostream &o)
	{
		o << name << " <file1.odb> <file2.odb> <output-file.odb>";
	}

	static void merge(const PathName& file1, const PathName& file2, const PathName& outputFileName);

private:
// No copy allowed

    MergeTool(const MergeTool&);
    MergeTool& operator=(const MergeTool&);

	static char* dummyArgv_[];

	PathName file1_;
	PathName file2_;
	PathName outputFile_;

};

} // namespace tool 
} // namespace odb 

#endif 
