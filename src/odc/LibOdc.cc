/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Simon Smart
/// @date   June 2017

#include <algorithm>
#include <string>

#include "odc/LibOdc.h"

#include "odc/ODBAPIVersion.h"

namespace odc {

//----------------------------------------------------------------------------------------------------------------------

LibOdc::LibOdc() : Library("odc") {}

const LibOdc& LibOdc::instance() {
    static LibOdc theinstance;
    return theinstance;;
}

const void* LibOdc::addr() const { return this; }

std::string LibOdc::version() const { return ODBAPIVersion::version(); }

std::string LibOdc::gitsha1(unsigned int count) const {
    std::string sha1(ODBAPIVersion::gitsha1());
    if(sha1.empty()) {
        return "not available";
    }

    return sha1.substr(0,std::min(count,40u));
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

