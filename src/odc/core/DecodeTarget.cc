/*
 * (C) Copyright 1996-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odc/core/DecodeTarget.h".h"


namespace odc {
namespace core {

//----------------------------------------------------------------------------------------------------------------------

DecodeTarget::DecodeTarget(const std::vector<api::StridedData>& facades) :
    columnFacades_(facades) {}

DecodeTarget::~DecodeTarget() {}

std::vector<api::StridedData>& DecodeTarget::dataFacades() {
    return columnFacades_;
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace core
} // namespace odc

