/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "odc/codec/IntegerMissing.h"
#include "odc/core/CodecFactory.h"

namespace odc {
namespace codec {

//----------------------------------------------------------------------------------------------------------------------

// Self registration

namespace {
    core::CodecBuilder<CodecInt8Missing> int8MissingBuilder;
    core::CodecBuilder<CodecInt16Missing> int16MissingBuilder;
    core::CodecBuilder<CodecConstantOrMissing> constantOrMissingBuilder;
    core::CodecBuilder<CodecRealConstantOrMissing> realConstantOrMissingBuilder;
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace codec
} // namespace odc
