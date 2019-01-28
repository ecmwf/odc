/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

///
/// \file odc_tools_c.cc
///
/// @author Piotr Kuchta, September 2015

#include "eckit/exception/Exceptions.h"

#include "odc/tools/ImportTool.h"

using namespace eckit;
using namespace odc;

extern "C" {

int import_text(const char* text, const char* output_file)
{
    try {
        odc::tool::ImportTool::importText(text, output_file);
        return 0;
    } catch (Exception e) {
        return 1;
    }
}

} // extern "C" 

