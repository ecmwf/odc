/*
 * (C) Copyright 1996-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odc/core/DecodeTarget.h"


namespace odc {
namespace core {

//----------------------------------------------------------------------------------------------------------------------

DecodeTarget::DecodeTarget(const std::vector<std::string>& columns,
                           const std::vector<api::StridedData>& facades) :
    columns_(columns),
    columnFacades_(facades) {}

DecodeTarget::DecodeTarget(const std::vector<std::string>& columns,
                           std::vector<api::StridedData>&& facades) :
    columns_(columns),
    columnFacades_(std::move(facades)) {}

DecodeTarget::~DecodeTarget() {}

const std::vector<std::string>&DecodeTarget::columns() const {
    return columns_;
}

std::vector<api::StridedData>& DecodeTarget::dataFacades() {
    return columnFacades_;
}

DecodeTarget DecodeTarget::slice(size_t rowOffset, size_t nrows) {

    std::vector<api::StridedData> newFacades;
    newFacades.reserve(columnFacades_.size());
    for (auto& facade : columnFacades_) {
        newFacades.emplace_back(facade.slice(rowOffset, nrows));
    }

    return DecodeTarget(columns_, std::move(newFacades));
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace core
} // namespace odc

