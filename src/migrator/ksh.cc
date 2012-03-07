/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "oda.h"

#include <iostream>
#include <fstream>

#include "ksh.h"

namespace odb {
namespace tool {


int ksh(const char *cmd, const char *file, int line, bool assertSuccess)
{
	std::string c = std::string("ksh -c \"") + cmd + "\"";

	Log::info() << "Executing '" + c + "' ";
	if (file) Log::info() << " " << file << " +" << line;
	Log::info() << endl;

	int rc = system(c.c_str());

	if (assertSuccess)
		ASSERT(rc == 0);
	return rc;
}

} // namespace tool 
} // namespace odb 

