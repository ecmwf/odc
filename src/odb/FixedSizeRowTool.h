/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef FixedSizeRowTool_H
#define FixedSizeRowTool_H

namespace odb {
namespace tool {

class FixedSizeRowTool : public Tool {
public:
	FixedSizeRowTool (int argc, char *argv[]); 

	void run(); 
	
	static void help(ostream &o)
	{ o << "Converts file to a format with fixed size rows"; }

	static void usage(const string& name, ostream &o)
	{ o << name << " <input.odb> <output.odb>"; }

private:
// No copy allowed

    FixedSizeRowTool(const FixedSizeRowTool&);
    FixedSizeRowTool& operator=(const FixedSizeRowTool&);
};

template <> struct ExperimentalTool<FixedSizeRowTool> { enum { experimental = true }; };

} // namespace tool 
} // namespace odb 

#endif 
