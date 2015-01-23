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

#include "odb_api/tools/Tool.h"
#include "odb_api/Types.h"

namespace odb {
namespace tool {

class MDSetTool : public Tool {
public:
	MDSetTool (int argc, char *argv[]); 

	void run(); 

	static void help(std::ostream &o)
	{ 
        using namespace std;

        o << "Creates a new file resetting types or values (constants only) of columns." << endl << endl

          << "Syntax:" << endl
          << "  odb <update-list> <input.odb> <output.odb>" << endl << endl

          << "Syntax of the <update-list> is a comma separated list of expressions of the form:" << endl
          << "  <column-name> : <type> = <value>" << endl << endl
          << "<type> can be one of: integer, real, double, string. If ommited, the existing type of the column will not be changed." << endl
          << "Both type and value are optional; at least one of the two should be present. For example:" << endl
          << "  odb mdset \"expver='    0008'\" input.odb patched.odb " << endl
          << "" << endl
          << "" << endl
          ;
    }

	static void usage(const std::string& name, std::ostream &o)
	{ o << name << " <update-list> <input.odb> <output.odb>"; }

private:
// No copy allowed

    MDSetTool(const MDSetTool&);
    MDSetTool& operator=(const MDSetTool&);

	void parseUpdateList(const std::string& s,
                         std::vector<std::string>& columns,
                         std::vector<std::string>& types,
                         std::vector<std::string>& values,
                         std::vector<BitfieldDef>& bitfieldDefs);
};

} // namespace tool 
} // namespace odb 

#endif 

