/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Piotr Kuchta
/// @author Simon Smart
/// @date January 2019

#ifndef odc_core_codec_Constant_H
#define odc_core_codec_Constant_H

#include "odc/codec/String.h"
#include "odc/core/Codec.h"

namespace odc {
namespace codec {

//----------------------------------------------------------------------------------------------------------------------

// Now the actual codecs!


template <typename ByteOrder, typename ValueType>
class CodecConstant : public core::DataStreamCodec<ByteOrder> {

public:  // definitions

    constexpr static const char* codec_name() { return "constant"; }

public:  // methods

    CodecConstant(api::ColumnType type, const std::string& name = codec_name()) :
        core::DataStreamCodec<ByteOrder>(name, type) {}
    ~CodecConstant() {}

private:  // methods

    void gatherStats(const double& v) override;
    unsigned char* encode(unsigned char* p, const double& d) override;
    void decode(double* out) override;
    void skip() override;

    void print(std::ostream& s) const override;
};


template <typename ByteOrder>
class CodecConstantString : public CodecConstant<ByteOrder, double> {

public:  // name

    constexpr static const char* codec_name() { return "constant_string"; }

public:  // methods

    CodecConstantString(api::ColumnType type) : CodecConstant<ByteOrder, double>(type, codec_name()) {}

private:  // methods

    unsigned char* encode(unsigned char* p, const double& d) override;
    void decode(double* out) override;
    void skip() override;

    void print(std::ostream& s) const override;
    size_t numStrings() const override { return 1; }

    using CodecConstant<ByteOrder, double>::load;
    using CodecConstant<ByteOrder, double>::save;
    void load(core::DataStream<ByteOrder>& ds) override;
    void save(core::DataStream<ByteOrder>& ds) override;
};

template <typename ByteOrder>
class CodecLongConstantString : public CodecChars<ByteOrder> {

public:  // methods

    constexpr static const char* codec_name() { return "long_constant_string"; }
    CodecLongConstantString(api::ColumnType type) : CodecChars<ByteOrder>(type, codec_name()) {};
    ~CodecLongConstantString() override {}

private:  // methods

    unsigned char* encode(unsigned char* p, const double& d) override;
    void decode(double* out) override;
    void skip() override;

    size_t numStrings() const override { return 1; }

    void load(core::DataStream<ByteOrder>& ds) override;
    void save(core::DataStream<ByteOrder>& ds) override;
};

//----------------------------------------------------------------------------------------------------------------------

// Implementation of Constant

template <typename ByteOrder, typename ValueType>
void CodecConstant<ByteOrder, ValueType>::gatherStats(const double& v) {
    static_assert(sizeof(ValueType) == sizeof(v), "unsafe casting check");
    const ValueType& val(reinterpret_cast<const ValueType&>(v));
    core::Codec::gatherStats(val);
}

template <typename ByteOrder, typename ValueType>
unsigned char* CodecConstant<ByteOrder, ValueType>::encode(unsigned char* p, const double&) {
    return p;
}

template <typename ByteOrder, typename ValueType>
void CodecConstant<ByteOrder, ValueType>::decode(double* out) {
    static_assert(sizeof(ValueType) == sizeof(double), "unsafe casting check");
    *reinterpret_cast<ValueType*>(out) = static_cast<ValueType>(this->min_);
}

template <typename ByteOrder, typename ValueType>
void CodecConstant<ByteOrder, ValueType>::skip() {}

template <typename ByteOrder, typename ValueType>
void CodecConstant<ByteOrder, ValueType>::print(std::ostream& s) const {
    s << this->name_ << ", value=" << std::fixed << static_cast<ValueType>(this->min_)
      << ", hasMissing=" << (this->hasMissing_ ? "true" : "false");
    if (this->hasMissing_) {
        s << ", missingValue=" << this->missingValue_;
    }
}

//----------------------------------------------------------------------------------------------------------------------

// Implementation of ConstantString

template <typename ByteOrder>
unsigned char* CodecConstantString<ByteOrder>::encode(unsigned char* p, const double&) {
    return p;
}

template <typename ByteOrder>
void CodecConstantString<ByteOrder>::decode(double* out) {
    (*out) = this->min_;
}

template <typename ByteOrder>
void CodecConstantString<ByteOrder>::skip() {}

template <typename ByteOrder>
void CodecConstantString<ByteOrder>::load(core::DataStream<ByteOrder>& ds) {
    core::DataStreamCodec<ByteOrder>::load(ds);
    ByteOrder::swap(this->min_);
    ByteOrder::swap(this->max_);
}

template <typename ByteOrder>
void CodecConstantString<ByteOrder>::save(core::DataStream<ByteOrder>& ds) {
    ByteOrder::swap(this->min_);
    ByteOrder::swap(this->max_);
    core::DataStreamCodec<ByteOrder>::save(ds);
    ByteOrder::swap(this->min_);
    ByteOrder::swap(this->max_);
}

template <typename ByteOrder>
void CodecConstantString<ByteOrder>::print(std::ostream& s) const {
    const char* cstr = reinterpret_cast<const char*>(&this->min_);
    s << this->name_ << ", value='" << std::string(cstr, ::strnlen(cstr, sizeof(double))) << "'";
}

//----------------------------------------------------------------------------------------------------------------------

// Implementation of LongConstantString

template <typename ByteOrder>
unsigned char* CodecLongConstantString<ByteOrder>::encode(unsigned char* p, const double&) {
    return p;
}

template <typename ByteOrder>
void CodecLongConstantString<ByteOrder>::decode(double* out) {
    ::memset(out, 0, this->decodedSizeDoubles_ * sizeof(double));
    ::memcpy(reinterpret_cast<char*>(out), &this->strings_[0][0],
             std::min(this->strings_[0].length(), this->decodedSizeDoubles_ * sizeof(double)));
}

template <typename ByteOrder>
void CodecLongConstantString<ByteOrder>::skip() {}

template <typename ByteOrder>
void CodecLongConstantString<ByteOrder>::load(core::DataStream<ByteOrder>& ds) {
    core::DataStreamCodec<ByteOrder>::load(ds);
    std::string s;
    ds.read(s);
    this->decodedSizeDoubles_ = ((s.length() - 1) / sizeof(double)) + 1;
    this->strings_.push_back(s);
}

template <typename ByteOrder>
void CodecLongConstantString<ByteOrder>::save(core::DataStream<ByteOrder>& ds) {
    core::DataStreamCodec<ByteOrder>::save(ds);
    ds.write(this->strings_[0]);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace codec
}  // namespace odc

#endif
