/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef odc_core_codec_IntegerMissing_H
#define odc_core_codec_IntegerMissing_H

#include "odc/codec/Integer.h"

namespace odc {
namespace codec {

//----------------------------------------------------------------------------------------------------------------------

template <typename ByteOrder>
class CodecInt8Missing : public BaseCodecInteger<ByteOrder> {

public: // definitions

    constexpr static const char* codec_name() { return "int8_missing"; }
    using value_type = uint8_t;
    constexpr static value_type missingMarker = 0xff;

public: // methods

    CodecInt8Missing(const std::string& name=codec_name(), double minmaxmissing=odc::MDI::integerMDI()) :
        BaseCodecInteger<ByteOrder>(name, minmaxmissing) {}
    ~CodecInt8Missing() override {}

private: // methods

    unsigned char* encode(unsigned char* p, const double& d) override {
        value_type s;
        if (d == this->missingValue_) {
            s = missingMarker;
        } else {
            s = d - this->min_;
            ASSERT(s != missingMarker);
        }
        ::memcpy(p, &s, sizeof(s));
        return p + sizeof(s);
    }

    void decode(double* out) override {
        value_type s;
        this->ds().read(s);
        (*out) = (s == missingMarker ? this->missingValue_ : (s + this->min_));
    }
};


//----------------------------------------------------------------------------------------------------------------------

// This can be template combined with CodecInt8Missing...

template <typename ByteOrder>
class CodecInt16Missing : public BaseCodecInteger<ByteOrder> {

public: // definitions

    constexpr static const char* codec_name() { return "int16_missing"; }
    using value_type = uint16_t;
    constexpr static value_type missingMarker = 0xffff;

public: // methods

    CodecInt16Missing() : BaseCodecInteger<ByteOrder>(codec_name()) {}
    ~CodecInt16Missing() override {}

private: // methods

    unsigned char* encode(unsigned char* p, const double& d) override {
        value_type s;
        if (d == this->missingValue_) {
            s = missingMarker;
        } else {
            s = d - this->min_;
            ASSERT(s != missingMarker);
        }
        ByteOrder::swap(s);
        ::memcpy(p, &s, sizeof(s));
        return p + sizeof(s);
    }

    void decode(double* out) override {
        value_type s;
        this->ds().read(s);
        (*out) = (s == missingMarker ? this->missingValue_ : (s + this->min_));
    }
};

//----------------------------------------------------------------------------------------------------------------------

template <typename ByteOrder>
class CodecConstantOrMissing : public CodecInt8Missing<ByteOrder> {

public: // definitions

    constexpr static const char* codec_name() { return "constant_or_missing"; }

public: // methods

    CodecConstantOrMissing(const std::string& name=codec_name(), double minmaxmissing=odc::MDI::integerMDI()) :
        CodecInt8Missing<ByteOrder>(name, minmaxmissing) {}
    ~CodecConstantOrMissing() override {}

private: // methods

    void print(std::ostream &s) const {
        s << this->name_ << ", value=";

        if (this->min_ == this->missingValue_) {
            s << "NULL";
        } else {
            s << std::fixed << this->min_;
        }

        if (this->hasMissing_) {
            s << ", missingValue=" << this->missingValue_;
        }
    }
};

//----------------------------------------------------------------------------------------------------------------------

template <typename ByteOrder>
class CodecRealConstantOrMissing : public CodecConstantOrMissing<ByteOrder> {

public: // definitions

    constexpr static const char* codec_name() { return "real_constant_or_missing"; }

public: // methods

    CodecRealConstantOrMissing() :
        CodecConstantOrMissing<ByteOrder>(codec_name(), odc::MDI::realMDI()) {}

    ~CodecRealConstantOrMissing() override {}
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace codec
} // namespace odc

#endif

