/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef ImportTool_H
#define ImportTool_H

namespace odb {
namespace tool {

class ImportTool : public Tool {
public:
	ImportTool (int argc, char *argv[]); 

	void run(); 

	static void help(ostream &o)
	{ o << "Imports data from a text file"; }

	static void usage(const string& name, ostream &o)
	{ o << name << " [-d delimiter] <input.file> <output.file>"; }

	static void importFile(const PathName& in, const PathName& out, const string& delimiter = defaultDelimiter);
	static void importText(const string& s, const PathName& out, const string& delimiter = defaultDelimiter);
private:
// No copy allowed
    ImportTool(const ImportTool&);
    ImportTool& operator=(const ImportTool&);

	static string defaultDelimiter;
};

} // namespace tool 
} // namespace odb 

#endif 

