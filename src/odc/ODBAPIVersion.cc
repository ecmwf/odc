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

#include "odc_config.h"
#include "odc/Header.h"
#include "odc/ODBAPIVersion.h"

namespace odc {

    const char *ODBAPIVersion::version() { return odc_VERSION_STR; }
    const char *gitsha1() { return odc_GIT_SHA1; }

	unsigned int ODBAPIVersion::formatVersionMajor() { return FORMAT_VERSION_NUMBER_MAJOR; }
	unsigned int ODBAPIVersion::formatVersionMinor() { return FORMAT_VERSION_NUMBER_MINOR; }
	const char *ODBAPIVersion::installPrefix()       { return odc_INSTALL_PREFIX; }
	const char *ODBAPIVersion::buildDirectory()      { return odc_BINARY_DIR; }

} // namespace odc

