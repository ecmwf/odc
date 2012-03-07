/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef ODB2ODATool_H
#define ODB2ODATool_H

#define SRC __FILE__, __LINE__

namespace odb {
namespace tool {

class ODB2ODATool : public Tool {
public:
	ODB2ODATool (int argc, char *argv[]); 
	ODB2ODATool (const Application *); 
    ODB2ODATool (const CommandLineParser &);

	void run(); 

private:
// No copy allowed
    ODB2ODATool(const ODB2ODATool&);
    ODB2ODATool& operator=(const ODB2ODATool&);
};

} // namespace tool 
} //namespace odb 

#endif 
