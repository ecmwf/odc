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

template <typename ByteOrder, typename ValueType, typename InternalValueType,
          class DerivedCodec>  // codec_nam passed through CRTP as char* is odd to deal with in template
class BaseCodecMissing : public BaseCodecInteger<ByteOrder, ValueType> {

public:  // methods

    // n.b. We should be able to call name=DerivedCodec::codec_name() directly, but
    // this causes a compilation error with Cray C++ 8.6
    BaseCodecMissing(api::ColumnType type, const std::string& name = codec_name_str(),
                     double minmaxmissing = odc::MDI::integerMDI()) :
        BaseCodecInteger<ByteOrder, ValueType>(type, name, minmaxmissing) {}
    ~BaseCodecMissing() {}

private:  // methods

    static std::string codec_name_str() { return DerivedCodec::codec_name(); }

    unsigned char* encode(unsigned char* p, const double& d) override {
        static_assert(sizeof(ValueType) == sizeof(d), "unsafe casting check");

        const ValueType& val(reinterpret_cast<const ValueType&>(d));
        InternalValueType s;
        if (val == this->missingValue_) {
            s = DerivedCodec::missingMarker;
        }
        else {
            s = val - this->min_;
            ASSERT(s != DerivedCodec::missingMarker);
        }
        ByteOrder::swap(s);
        ::memcpy(p, &s, sizeof(s));
        return p + sizeof(s);
    }

    void decode(double* out) override {
        static_assert(sizeof(ValueType) == sizeof(out), "unsafe casting check");

        ValueType* val_out = reinterpret_cast<ValueType*>(out);
        InternalValueType s;
        this->ds().read(s);
        (*val_out) = (s == DerivedCodec::missingMarker ? this->missingValue_ : (s + this->min_));
    }

    void skip() override { this->ds().advance(sizeof(InternalValueType)); }
};


//----------------------------------------------------------------------------------------------------------------------

template <typename ByteOrder, typename ValueType>
struct CodecInt8Missing
    : public BaseCodecMissing<ByteOrder, ValueType, uint8_t, CodecInt8Missing<ByteOrder, ValueType>> {
    constexpr static const char* codec_name() { return "int8_missing"; }
    constexpr static uint8_t missingMarker = 0xff;
    using BaseCodecMissing<ByteOrder, ValueType, uint8_t, CodecInt8Missing<ByteOrder, ValueType>>::BaseCodecMissing;
};

//----------------------------------------------------------------------------------------------------------------------

template <typename ByteOrder, typename ValueType>
struct CodecInt16Missing
    : public BaseCodecMissing<ByteOrder, ValueType, uint16_t, CodecInt16Missing<ByteOrder, ValueType>> {
    constexpr static const char* codec_name() { return "int16_missing"; }
    constexpr static uint16_t missingMarker = 0xffff;
    using BaseCodecMissing<ByteOrder, ValueType, uint16_t, CodecInt16Missing<ByteOrder, ValueType>>::BaseCodecMissing;
};

//----------------------------------------------------------------------------------------------------------------------


template <typename ByteOrder, typename ValueType>
struct CodecConstantOrMissing
    : public BaseCodecMissing<ByteOrder, ValueType, uint8_t, CodecConstantOrMissing<ByteOrder, ValueType>> {

    static_assert(sizeof(ValueType) == sizeof(double), "unsafe casting check");

    constexpr static const char* codec_name() { return "constant_or_missing"; }
    constexpr static uint8_t missingMarker = 0xff;

    using BaseCodecMissing<ByteOrder, ValueType, uint8_t,
                           CodecConstantOrMissing<ByteOrder, ValueType>>::BaseCodecMissing;

private:  // methods

    void print(std::ostream& s) const {
        s << this->name_ << ", value=";

        if (this->min_ == this->missingValue_) {
            s << "NULL";
        }
        else {
            s << std::fixed << this->min_;
        }
        s << ", hasMissing=" << (this->hasMissing_ ? "true" : "false");

        if (this->hasMissing_) {
            s << ", missingValue=" << this->missingValue_;
        }
    }
};

//----------------------------------------------------------------------------------------------------------------------

template <typename ByteOrder>
class CodecRealConstantOrMissing : public CodecConstantOrMissing<ByteOrder, double> {

public:  // definitions

    constexpr static const char* codec_name() { return "real_constant_or_missing"; }

public:  // methods

    CodecRealConstantOrMissing(api::ColumnType type) :
        CodecConstantOrMissing<ByteOrder, double>(type, codec_name(), odc::MDI::realMDI()) {}

    ~CodecRealConstantOrMissing() override {}

    double missingValue() const override { return this->missingValue_; }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace codec
}  // namespace odc

#endif
