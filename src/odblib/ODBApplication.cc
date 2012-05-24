/// \file ODBApplication.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#include <sstream>

#include "eclib/Context.h"

#include "odblib/Tool.h"
#include "odblib/TestCase.h"
#include "odblib/ToolFactory.h"
#include "odblib/ODBApplication.h"
#include "odblib/TestRunner.h"
#include "odblib/ODBBehavior.h"

namespace odb {
namespace tool {

ODBApplication::ODBApplication (int argc, char **argv)
: clp_(argc, argv)
{
	 Context::instance().setup( argc, argv, new ODBBehavior() );
}

ODBApplication::~ODBApplication() {}

CommandLineParser& ODBApplication::commandLineParser() { return clp_; }

void ODBApplication::start()
{
	run();
}

} // namespace tool 
} // namespace odb 

