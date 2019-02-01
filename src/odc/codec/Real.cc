/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "odc/codec/Real.h"
#include "odc/core/CodecFactory.h"

namespace odc {
namespace codec {

//----------------------------------------------------------------------------------------------------------------------

// Self registration

namespace {
    core::CodecBuilder<CodecLongReal> longRealBuilder;
    core::CodecBuilder<CodecShortReal> shortRealBuilder;
    core::CodecBuilder<CodecShortReal2> shortReal2Builder;
}

template <> constexpr const char* CodecLongReal<odc::core::SameByteOrder>::codec_name;
template <> constexpr const char* CodecLongReal<odc::core::OtherByteOrder>::codec_name;

template <> constexpr const char* CodecShortReal<odc::core::SameByteOrder>::codec_name;
template <> constexpr const char* CodecShortReal<odc::core::OtherByteOrder>::codec_name;

template <> constexpr const char* CodecShortReal2<odc::core::SameByteOrder>::codec_name;
template <> constexpr const char* CodecShortReal2<odc::core::OtherByteOrder>::codec_name;

//----------------------------------------------------------------------------------------------------------------------

} // namespace codec
} // namespace odc
