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

#ifndef odb_api_LibOdc_H
#define odb_api_LibOdc_H

#include "eckit/system/Library.h"

namespace odc {

//----------------------------------------------------------------------------------------------------------------------

class LibOdc : public eckit::system::Library {
public:

    LibOdc();

    static const LibOdc& instance();

    virtual std::string version() const;

    virtual std::string gitsha1(unsigned int count) const;

protected:

    const void* addr() const;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace odc

#endif  // odb_api_LibOdc_H
