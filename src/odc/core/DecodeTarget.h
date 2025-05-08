/*
 * (C) Copyright 1996-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Simon Smart
/// @date Dec 2018

#ifndef odc_core_DecodeTarget_H
#define odc_core_DecodeTarget_H

#include <vector>

#include "odc/api/StridedData.h"


namespace odc {
namespace core {

//----------------------------------------------------------------------------------------------------------------------


class DecodeTarget {

public:  // methods

    DecodeTarget(const std::vector<std::string>& columns, const std::vector<api::StridedData>& facades);
    DecodeTarget(const std::vector<std::string>& columns, std::vector<api::StridedData>&& facades);
    ~DecodeTarget();

    const std::vector<std::string>& columns() const;
    std::vector<api::StridedData>& dataFacades();

    DecodeTarget slice(size_t rowOffset, size_t nrows);

private:  // members

    std::vector<std::string> columns_;
    std::vector<api::StridedData> columnFacades_;
};


//----------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace odc

#endif
