/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */
#ifndef MDSetTool_H
#define MDSetTool_H

namespace odb {
namespace tool {

class MDSetTool : public Tool {
public:
	MDSetTool (int argc, char *argv[]); 

	void run(); 

	static void help(std::ostream &o)
	{ o << "Creates a new file resetting types of columns"; }

	static void usage(const string& name, std::ostream &o)
	{ o << name << " <update-list> <input.odb> <output.odb>"; }

private:
// No copy allowed

    MDSetTool(const MDSetTool&);
    MDSetTool& operator=(const MDSetTool&);

	void parseUpdateList(string s, vector<std::string>& columns, vector<std::string>& values);
};

} // namespace tool 
} // namespace odb 

#endif 

