/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef CountTool_H
#define CountTool_H

#include "odblib/Tool.h"

namespace odb {
namespace tool {

class CountTool : public Tool {
public:
    
	CountTool (int argc, char *argv[]); 

	static unsigned long long rowCount(const eckit::PathName &);
	static unsigned long long fastRowCount(const eckit::PathName &);

	void run(); 

	static void help(std::ostream &o)
	{
		o << "Counts number of rows";
	}

	static void usage(const string& name, std::ostream &o)
	{
		o << name << " <file.odb>";
	}

private:

// No copy allowed

    CountTool(const CountTool&);
    CountTool& operator=(const CountTool&);
};

} // namespace tool 
} // namespace odb 

#endif 
