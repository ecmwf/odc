/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file Tool.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#include <iostream>
using namespace std;

#include "eclib/Application.h"
#include "eclib/Translator.h"

#include "odblib/MemoryBlock.h"
#include "eclib/FileHandle.h"
#include "eclib/PathName.h"
#include "Tool.h"
#include "odblib/Endian.h"
#include "eclib/Regex.h"

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

