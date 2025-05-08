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
/// \file CodecOptimizer.h
///
/// @author Piotr Kuchta, Jan 2010

#ifndef odc_core_CodecOptimizer_H
#define odc_core_CodecOptimizer_H

#include "odc/api/ColumnType.h"
#include "odc/codec/Real.h"
#include "odc/core/CodecFactory.h"
#include "odc/core/MetaData.h"

namespace odc {
namespace codec {

//----------------------------------------------------------------------------------------------------------------------

class CodecOptimizer {
public:

    CodecOptimizer();
    template <typename DATASTREAM>
    int setOptimalCodecs(core::MetaData& columns);

private:

    static std::map<api::ColumnType, std::string> defaultCodec_;
};

template <typename ByteOrder>
int CodecOptimizer::setOptimalCodecs(core::MetaData& columns) {
    // std::ostream &LOG = eckit::Log::error();
    for (size_t i = 0; i < columns.size(); i++) {
        core::Column& col = *columns[i];
        long long n;
        double min      = col.min();
        double max      = col.max();
        bool hasMissing = col.hasMissing();
        double missing  = col.rawMissingValue();
        // LOG << "CodecOptimizer::setOptimalCodecs: " << i << " " << col.name() << ", min=" << min << ", max=" << max
        // << std::endl;
        std::string codec(defaultCodec_[col.type()]);
        switch (col.type()) {
            case api::REAL: {

                // Currently the real data is (whist in the column) encoded using the LongReal codec.
                // n.b. CodecOptimizer doesn't currently support OtherByteOrder.
                CodecLongReal<core::SameByteOrder>* codec_long =
                    dynamic_cast<CodecLongReal<core::SameByteOrder>*>(&col.coder());
                ASSERT(codec_long != 0);

                if (max == min) {
                    codec = col.hasMissing() ? "real_constant_or_missing" : "constant";
                }
                else if (codec_long->hasShortReal2InternalMissing()) {
                    ASSERT(!codec_long->hasShortRealInternalMissing());
                    codec = "short_real";
                }
                else if (codec_long->hasShortRealInternalMissing()) {
                    codec = "short_real2";
                }

                col.coder(core::CodecFactory::instance().build<ByteOrder>(codec, col.type()));
                col.hasMissing(hasMissing);
                col.missingValue(missing);
                col.min(min);
                col.max(max);
                // LOG << " REAL has values in range <" << col.min() << ", " << col.max()
                //         << ">. Codec: "  << col.coder()
                //         << std::endl;
                break;
            }

            case api::DOUBLE:
                if (max == min)
                    codec = col.hasMissing() ? "real_constant_or_missing" : "constant";
                col.coder(core::CodecFactory::instance().build<ByteOrder>(codec, col.type()));
                col.hasMissing(hasMissing);
                col.missingValue(missing);
                col.min(min);
                col.max(max);
                // LOG << " DOUBLE has values in range <" << col.min() << ", " << col.max()
                //         << ">. Codec: "  << col.coder()
                //         << std::endl;
                break;

            case api::STRING: {
                n = col.coder().numStrings();
                if (n == 1 && col.coder().dataSizeDoubles() == 1)
                    codec = "constant_string";
                else if (n == 1 && std::getenv("ODC_ENABLE_WRITING_LONG_STRING_CODEC") != NULL)
                    codec = "long_constant_string";
                else if (n < 256)
                    codec = "int8_string";
                else if (n < 65536)
                    codec = "int16_string";


                std::unique_ptr<core::Codec> newCodec =
                    core::CodecFactory::instance().build<ByteOrder>(codec, col.type());
                if (codec == "constant_string") {
                    ASSERT(col.coder().dataSizeDoubles() == 1);
                }
                else {
                    newCodec->dataSizeDoubles(col.coder().dataSizeDoubles());
                    newCodec->copyStrings(col.coder());
                }
                col.coder(std::move(newCodec));
                col.hasMissing(hasMissing);
                col.missingValue(missing);
                col.min(min);
                col.max(max);
                // LOG << " STRING has " << n << " different value(s). Codec: " << codec
                //         << std::endl;
            } break;

            case api::BITFIELD:
            case api::INTEGER:
                n = max - min;
                // LOG << " { min=" << min << ", max=" << max << ", n=" << n << "} ";
                if (col.hasMissing()) {
                    if (n == 0)
                        codec = "constant_or_missing";
                    else if (n < 0xff)
                        codec = "int8_missing";
                    else if (n < 0xffff)
                        codec = "int16_missing";
                }
                else {
                    if (n == 0)
                        codec = "constant";
                    else if (n <= 0xff)
                        codec = "int8";
                    else if (n <= 0xffff)
                        codec = "int16";
                }
                col.coder(core::CodecFactory::instance().build<ByteOrder>(codec, col.type()));
                col.hasMissing(hasMissing);
                col.missingValue(missing);
                col.min(min);
                col.max(max);
                // LOG << (col.type() == BITFIELD ? " BITFIELD" : " INTEGER")
                //	<< " has " << n + 1 << " different value(s)"
                //	<< (col.hasMissing() ? " and has missing value. " : ". ")
                //	<< "Codec: " << col.coder() << "."
                //	<< std::endl;
                break;

            default:
                eckit::Log::error() << "Unsupported type: [" << col.type() << "]" << std::endl;
                break;
        }

        // if (odc::ODBAPISettings::debug && i == 28) eckit::Log::info() << ": AFTER " << col << " -> " << col.coder()
        // << std::endl;
    }
    return 0;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace codec
}  // namespace odc

#endif
