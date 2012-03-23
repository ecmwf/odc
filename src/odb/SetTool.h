/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef SetTool_H
#define SetTool_H

#include "odb/Tool.h"

namespace odb {
namespace tool {

class SetTool : public Tool {
public:
	SetTool (int argc, char *argv[]); 

	void run(); 

	static void help(ostream &o)
	{ o << "Creates a new file setting columns to given values"; }

	static void usage(const string& name, ostream &o)
	{ o << name << " <update-list> <input.odb> <output.odb>"; }

private:
// No copy allowed

    SetTool(const SetTool&);
    SetTool& operator=(const SetTool&);

	void parseUpdateList(string s, vector<std::string>& columns, vector<double>& values);
};

} // namespace tool 
} // namespace odb 

#endif 

