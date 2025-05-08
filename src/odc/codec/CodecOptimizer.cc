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
/// \file CodecOptimizer.cc
///
/// @author Piotr Kuchta, Jan 2010

#include "odc/codec/CodecOptimizer.h"
#include "eckit/config/Resource.h"
#include "eckit/utils/StringTools.h"

//

namespace odc {
namespace codec {

//----------------------------------------------------------------------------------------------------------------------

std::map<api::ColumnType, std::string> CodecOptimizer::defaultCodec_;

CodecOptimizer::CodecOptimizer() {
    if (defaultCodec_.empty()) {
        defaultCodec_[api::REAL]     = "short_real2";
        defaultCodec_[api::DOUBLE]   = "long_real";
        defaultCodec_[api::STRING]   = "chars";
        defaultCodec_[api::INTEGER]  = "int32";
        defaultCodec_[api::BITFIELD] = "int32";

        typedef eckit::StringTools S;
        std::vector<std::string> mappings(S::split(",", eckit::Resource<std::string>("$ODC_DEFAULT_CODEC", "")));

        for (size_t i = 0; i < mappings.size(); ++i) {
            std::vector<std::string> a(S::split(":", mappings[i]));
            ASSERT("Wrong format of $ODC_DEFAULT_CODEC" && a.size() == 2);
            defaultCodec_[core::Column::type(S::trim(a[0]))] = S::trim(a[1]);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace codec
}  // namespace odc
