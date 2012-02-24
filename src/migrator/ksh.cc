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

