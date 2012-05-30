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

namespace odb {
namespace tool {

class MergeTool : public Tool {
public:
	MergeTool (int argc, char *argv[]); 

	void run(); 

	static void help(ostream &o) { o << "Merges rows from files"; }
	static void usage(const string& name, ostream &o)
	{
		o << name << " -o <output-file.odb> <input1.odb> <input2.odb> ...";
	}

	static void merge(const vector<PathName>& inputFiles, const PathName& outputFileName);

private:
// No copy allowed

    MergeTool(const MergeTool&);
    MergeTool& operator=(const MergeTool&);

	static char* dummyArgv_[];

	vector<PathName> inputFiles_;
	PathName outputFile_;

};

} // namespace tool 
} // namespace odb 

#endif 
