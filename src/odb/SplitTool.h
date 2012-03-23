/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef SplitTool_H
#define SplitTool_H

#include "odb/Tool.h"

namespace odb {
namespace tool {

class SplitTool : public Tool {
public:
	SplitTool (int argc, char *argv[]); 

	void run(); 

	static void help(ostream &o)
	{
		o << "Splits file according to given template";
	}

	static void usage(const string& name, ostream &o)
	{
		o << name << " <input.odb> <output_template.odb>";
	}

private:
// No copy allowed

    SplitTool(const SplitTool&);
    SplitTool& operator=(const SplitTool&);
};

} // namespace tool 
} // namespace odb 

#endif 
