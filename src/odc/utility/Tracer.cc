/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odc/utility/Tracer.h"

#include <ostream>


namespace odc {
namespace utility {

//----------------------------------------------------------------------------------------------------------------------

Tracer::Tracer(std::ostream& o, const std::string& m) : out_(o), message_(m) {
    // out_ << message_ << " BEGIN" <<  std::endl;
    out_ << "BEGIN " << message_ << std::endl;
}

Tracer::~Tracer() {
    // out_ << message_ << " END" <<  std::endl;
    out_ << "END " << message_ << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace utility
}  // namespace odc
