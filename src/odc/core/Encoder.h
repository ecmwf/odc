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
/// @date March 2019

#ifndef odc_core_Encoder_H
#define odc_core_Encoder_H

#include <vector>

#include "eckit/io/DataHandle.h"

#include "odc/api/ColumnInfo.h"
#include "odc/api/StridedData.h"

namespace odc {
namespace core {

//----------------------------------------------------------------------------------------------------------------------

void encodeFrame(eckit::DataHandle& out, const std::vector<api::ColumnInfo>& columns,
                 const std::vector<api::ConstStridedData>& data, const std::map<std::string, std::string>& properties);

//----------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace odc

#endif
