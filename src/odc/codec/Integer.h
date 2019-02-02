/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef odc_core_codec_Integer_H
#define odc_core_codec_Integer_H

#include "odc/core/Codec.h"

namespace odc {
namespace codec {

//----------------------------------------------------------------------------------------------------------------------

template<typename ByteOrder>
class BaseCodecInteger : public core::DataStreamCodec<ByteOrder> {

public: // methods

    BaseCodecInteger(const std::string& name, double minmaxmissing=odc::MDI::integerMDI()) :
        core::DataStreamCodec<ByteOrder>(name) {

            this->min_ = minmaxmissing;
            this->max_ = minmaxmissing;
            this->missingValue_ = minmaxmissing;
    }

    ~BaseCodecInteger() override {}
};


//----------------------------------------------------------------------------------------------------------------------


template<typename ByteOrder>
class CodecInt8 : public BaseCodecInteger<ByteOrder> {

public: // definitions

    constexpr static const char* codec_name() { return "int8"; }

public: // methods

    CodecInt8() : BaseCodecInteger<ByteOrder>(codec_name()) {}
    ~CodecInt8() override {}

private: // methods

    unsigned char* encode(unsigned char* p, const double& d) override {
        uint8_t s = d - this->min_;
        ::memcpy(p, &s, sizeof(s));
        return p + sizeof(s);
    }

    void decode(double* out) override {
        uint8_t s;
        this->ds().read(s);
        (*out) = s + this->min_;
    }
};


//----------------------------------------------------------------------------------------------------------------------


template<typename ByteOrder>
class CodecInt16 : public BaseCodecInteger<ByteOrder> {

public: // definitions

    constexpr static const char* codec_name() { return "int16"; }

public: // methods

    CodecInt16() : BaseCodecInteger<ByteOrder>(codec_name()) {}
    ~CodecInt16() override {}

private: // methods

    unsigned char* encode(unsigned char* p, const double& d) override {
        uint16_t s = d - this->min_;
        ByteOrder::swap(s);
        ::memcpy(p, &s, sizeof(s));
        return p + sizeof(s);
    }

    void decode(double* out) override {
        uint16_t s;
        this->ds().read(s);
        (*out) = s + this->min_;
    }
};


//----------------------------------------------------------------------------------------------------------------------


template<typename ByteOrder>
class CodecInt32 : public BaseCodecInteger<ByteOrder> {

public: // definitions

    constexpr static const char* codec_name() { return "int32"; }

public: // methods

    CodecInt32() : BaseCodecInteger<ByteOrder>(codec_name()) {}
    ~CodecInt32() override {}

private: // methods

    unsigned char* encode(unsigned char* p, const double& d) override {
        int32_t s = d;
        ByteOrder::swap(s);
        ::memcpy(p, &s, sizeof(s));
        return p + sizeof(s);
    }

    void decode(double* out) override {
        int32_t s;
        this->ds().read(s);
        (*out) = s;
    }
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace codec
} // namespace odc

#endif

