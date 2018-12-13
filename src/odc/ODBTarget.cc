/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/runtime/Monitor.h"
#include "eckit/log/TimeStamp.h"

#include "odc/ODBTarget.h"

namespace odc {

ODBTarget::ODBTarget(const char* tag, eckit::LogTarget* target)
: eckit::WrapperTarget(target),
  tag_(tag)
{}

void ODBTarget::writePrefix() {

    std::ostringstream oss;
    oss //<< std::setw(3)
        //<< std::setfill('0')
        //<< Monitor::instance().self()
        << "000"
        << std::setfill(' ') << ' '
        << eckit::TimeStamp() << ' ';

    if(tag_ && *tag_) {
        oss << tag_ << ' ';
    }

    const std::string& s (oss.str());
    const char* p (s.c_str());
    target_->write(p, p + s.size());
}

void ODBTarget::print(std::ostream& s) const
{
    s << "ODBTarget";
}

void ODBTarget::writeSuffix() {} 

} // namespace odc
