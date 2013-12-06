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
/// This file will be updated autmatically by the make.sms script
///

//#include "odblib/Header.h"

#include "odblib/ODBAPIVersion.h"

namespace odb {

    const char *ODBAPIVersion::version() { return ODB_API_VERSION_STR; }

	unsigned int ODBAPIVersion::formatVersionMajor() { return FORMAT_VERSION_NUMBER_MAJOR; }
	unsigned int ODBAPIVersion::formatVersionMinor() { return FORMAT_VERSION_NUMBER_MINOR ;}

} // namespace odb

