/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef MigratorTool_H
#define MigratorTool_H

#include "tools/Tool.h"

namespace odb {
namespace tool {

class MigratorTool : public Tool {
public:
	MigratorTool (int argc, char *argv[]); 
    MigratorTool (const CommandLineParser &);

    void resetMDI(const std::string&);

	void run(); 

private:
// No copy allowed
    MigratorTool(const MigratorTool&);
    MigratorTool& operator=(const MigratorTool&);
};

} // namespace tool 
} //namespace odb 

#endif 
