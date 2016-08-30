/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file ODBApplication.h
///
/// @author Piotr Kuchta, ECMWF, Feb 2009

#include "eckit/config/Resource.h"
#include "odb_api/ODBApplication.h"

using namespace eckit;

namespace odb {
namespace tool {

ODBApplication::ODBApplication (int argc, char **argv) :
    Application(argc,argv),
    clp_(argc, argv) {
}

ODBApplication::~ODBApplication() {}

CommandLineParser& ODBApplication::commandLineParser() { return clp_; }

} // namespace tool 
} // namespace odb 

