/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

//#include "odblib/SQLOutputConfig.h"

namespace odb {
namespace sql {

const char* SQLOutputConfig::defaultDelimiter_("	");
const char* SQLOutputConfig::defaultOutputFile_("output.odb");
const char* SQLOutputConfig::defaultFormat_("default");

const SQLOutputConfig SQLOutputConfig::defaultConfig_;

} // namespace sql
} // namespace odb

