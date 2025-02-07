/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "odc/codec/Constant.h"
#include "odc/core/CodecFactory.h"
#include "odc/core/DataStream.h"

namespace odc {
namespace codec {

//----------------------------------------------------------------------------------------------------------------------

// Self registration

namespace {
    core::IntegerCodecBuilder<CodecConstant> constantBuilder;
    core::CodecBuilder<CodecConstantString> constantStringBuilder;
    core::CodecBuilder<CodecLongConstantString> LongConstantStringBuilder;
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace codec
} // namespace odc
