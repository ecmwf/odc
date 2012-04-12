/// \file Tool.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#include <iostream>
using namespace std;

#include "Application.h"
#include "Translator.h"

#include "MemoryBlock.h"
#include "FileHandle.h"
#include "PathName.h"
#include "Tool.h"
#include "Endian.h"
#include "Regex.h"

#include <strings.h>

namespace odb {
namespace tool {

Tool::~Tool() {}

Tool::Tool(int argc, char **argv)
: CommandLineParser(argc, argv)
{}

Tool::Tool(const Application *app)
: CommandLineParser(app)
{}

Tool::Tool(const CommandLineParser& clp)
: CommandLineParser(clp)
{}

} // namespace tool 
} // namespace odb 

