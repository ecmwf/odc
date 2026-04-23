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

/// @note We have some strange behaviour in here. In particular, we support BOTH decoding
/// and encoding integers from a representation as doubles, and also as integers.
/// This is a little bit strange, and reflects the history of these being used in the IFS
/// and ODB1 where everything was a double.
///
/// n.b. The decoded size is int64_t, even though the codec only goes up to int32, as we
/// want to support bigger numbers in the future.

namespace odc {
namespace codec {

//----------------------------------------------------------------------------------------------------------------------

template <typename ByteOrder, typename ValueType>
class BaseCodecInteger : public core::DataStreamCodec<ByteOrder> {

public:  // definitions

    using value_type = ValueType;

public:  // methods

    BaseCodecInteger(api::ColumnType type, const std::string& name, double minmaxmissing = odc::MDI::integerMDI()) :
        core::DataStreamCodec<ByteOrder>(name, type), castedMissingValue_(static_cast<ValueType>(minmaxmissing)) {

        this->min_          = minmaxmissing;
        this->max_          = minmaxmissing;
        this->missingValue_ = minmaxmissing;
    }

    ~BaseCodecInteger() override {}

private:  // methods

    void missingValue(double v) override {
        core::Codec::missingValue(v);
        castedMissingValue_ = static_cast<ValueType>(this->missingValue_);
    }

    double missingValue() const override {
        static_assert(sizeof(ValueType) == sizeof(double), "unsafe casting check");
        const double* punned_value = reinterpret_cast<const double*>(&castedMissingValue_);
        return *punned_value;
    }

    void load(odc::core::DataStream<ByteOrder>& ds) override {
        core::DataStreamCodec<ByteOrder>::load(ds);
        castedMissingValue_ = static_cast<ValueType>(this->missingValue_);
    }

    void gatherStats(const double& v) override {
        static_assert(sizeof(ValueType) == sizeof(v), "unsafe casting check");
        const ValueType& val(reinterpret_cast<const ValueType&>(v));
        core::Codec::gatherStats(val);
    }

protected:  // members

    /// @note - this indirection via castedMissingValue_ rather than just using missingValue_
    ///         directly where needed is to work around a Cray 8.7 compiler bug, where
    ///         where the punned version gets optimised out
    ValueType castedMissingValue_;
};


//----------------------------------------------------------------------------------------------------------------------

template <typename ByteOrder, typename ValueType, typename InternalValueType,
          class DerivedCodec>  // codec_nam passed through CRTP as char* is odd to deal with in template
class CodecIntegerOffset : public BaseCodecInteger<ByteOrder, ValueType> {

public:  // methods

    CodecIntegerOffset(api::ColumnType type) :
        BaseCodecInteger<ByteOrder, ValueType>(type, DerivedCodec::codec_name()) {}
    ~CodecIntegerOffset() override {}

private:  // methods

    unsigned char* encode(unsigned char* p, const double& d) override {
        static_assert(sizeof(ValueType) == sizeof(d), "unsafe casting check");

        // Static cast to uint64_t ensures that the narrowing cast doesn't trigger the
        // undefined behaviour sanitizer. It is correct that we can generate an integer
        // that is too big, and throw it away, if invalid data gets here.
        const ValueType& val(reinterpret_cast<const ValueType&>(d));
        InternalValueType s = static_cast<uint64_t>(val - this->min_);
        ByteOrder::swap(s);
        ::memcpy(p, &s, sizeof(s));
        return p + sizeof(s);
    }

    void decode(double* out) override {
        static_assert(sizeof(ValueType) == sizeof(out), "unsafe casting check");

        ValueType* val_out = reinterpret_cast<ValueType*>(out);
        InternalValueType s;
        this->ds().read(s);
        (*val_out) = s + this->min_;
    }

    void skip() override { this->ds().advance(sizeof(InternalValueType)); }
};


//----------------------------------------------------------------------------------------------------------------------

template <typename ByteOrder, typename ValueType, typename InternalValueType,
          class DerivedCodec>  // codec_nam passed through CRTP as char* is odd to deal with in template
class CodecIntegerDirect : public BaseCodecInteger<ByteOrder, ValueType> {

public:  // methods

    CodecIntegerDirect(api::ColumnType type) :
        BaseCodecInteger<ByteOrder, ValueType>(type, DerivedCodec::codec_name()) {}
    ~CodecIntegerDirect() override {}

private:  // methods

    unsigned char* encode(unsigned char* p, const double& d) override {
        static_assert(sizeof(ValueType) == sizeof(d), "unsafe casting check");

        const ValueType& val(reinterpret_cast<const ValueType&>(d));
        InternalValueType s = val;
        ByteOrder::swap(s);
        ::memcpy(p, &s, sizeof(s));
        return p + sizeof(s);
    }

    void decode(double* out) override {
        static_assert(sizeof(ValueType) == sizeof(out), "unsafe casting check");

        ValueType* val_out = reinterpret_cast<ValueType*>(out);
        InternalValueType s;
        this->ds().read(s);
        (*val_out) = s;
    }

    void skip() override { this->ds().advance(sizeof(InternalValueType)); }
};

//----------------------------------------------------------------------------------------------------------------------

template <typename ByteOrder, typename ValueType>
struct CodecInt8 : public CodecIntegerOffset<ByteOrder, ValueType, uint8_t, CodecInt8<ByteOrder, ValueType>> {
    constexpr static const char* codec_name() { return "int8"; }
    using CodecIntegerOffset<ByteOrder, ValueType, uint8_t, CodecInt8<ByteOrder, ValueType>>::CodecIntegerOffset;
};

//----------------------------------------------------------------------------------------------------------------------

template <typename ByteOrder, typename ValueType>
struct CodecInt16 : public CodecIntegerOffset<ByteOrder, ValueType, uint16_t, CodecInt16<ByteOrder, ValueType>> {
    constexpr static const char* codec_name() { return "int16"; }
    using CodecIntegerOffset<ByteOrder, ValueType, uint16_t, CodecInt16<ByteOrder, ValueType>>::CodecIntegerOffset;
};

//----------------------------------------------------------------------------------------------------------------------

template <typename ByteOrder, typename ValueType>
struct CodecInt32 : public CodecIntegerDirect<ByteOrder, ValueType, int32_t, CodecInt32<ByteOrder, ValueType>> {
    constexpr static const char* codec_name() { return "int32"; }
    using CodecIntegerDirect<ByteOrder, ValueType, int32_t, CodecInt32<ByteOrder, ValueType>>::CodecIntegerDirect;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace codec
}  // namespace odc

#endif
