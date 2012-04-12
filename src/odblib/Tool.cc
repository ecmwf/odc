/// \file Tool.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#include <strings.h>
#include <iostream>

using namespace std;

#include "eclib/Application.h"

#include "odblib/CommandLineParser.h"
#include "odblib/Tool.h"

namespace odb {
namespace tool {

Tool::~Tool() {}

Tool::Tool(int argc, char **argv)
: CommandLineParser(argc, argv)
{}

Tool::Tool(const CommandLineParser& clp)
: CommandLineParser(clp)
{}

} // namespace tool 
} // namespace odb 

