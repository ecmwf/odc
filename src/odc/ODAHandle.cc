/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/log/Log.h"
#include "eckit/types/Types.h"

#include "odc/LibOdc.h"
#include "odc/ODAHandle.h"

using namespace eckit;

namespace odc {


ODAHandle::ODAHandle(Offset start, Offset end) : start_(start), end_(end) {
    LOG_DEBUG_LIB(LibOdc) << "ODAHandle::ODAHandle(" << start << ", " << end << ")" << std::endl;
}

void ODAHandle::print(std::ostream& o) const {
    o << "[start:" << start_ << ", end_:" << end_ << ", values_:" /*<< values_ <<*/ "]";
}

ODAHandle::~ODAHandle() {
    LOG_DEBUG_LIB(LibOdc) << "ODAHandle::~ODAHandle()" << std::endl;
}

void ODAHandle::addValue(const std::string& columnName, double v) {
    LOG_DEBUG_LIB(LibOdc) << "ODAHandle::addValue('" << columnName << "', '" << v << "')" << std::endl;
    ASSERT(values_.find(columnName) == values_.end());
    values_[columnName] = v;
}

}  // namespace odc
