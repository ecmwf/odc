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

#include "odc/core/Codec.h"

namespace odc {
namespace codec {

//----------------------------------------------------------------------------------------------------------------------

// Now the actual codecs!


template<typename ByteOrder>
class CodecConstant : public core::DataStreamCodec<ByteOrder> {

public: // definitions

    constexpr static char codec_name[] = "constant";

public: // methods

    CodecConstant(const std::string& name=codec_name) : core::DataStreamCodec<ByteOrder>(name) {}
    ~CodecConstant() {}

private: // methods

    unsigned char* encode(unsigned char* p, const double& d) override;
    void decode(double* out) override;

    void print(std::ostream& s) const;
};


template<typename ByteOrder>
class CodecConstantString : public CodecConstant<ByteOrder> {

public: // name

    constexpr static char codec_name[] = "constant_string";

public: // methods

    CodecConstantString() : CodecConstant<ByteOrder>(codec_name) {}

private: // methods

    unsigned char* encode(unsigned char* p, const double& d) override;
    void decode(double* out) override;

    void print(std::ostream& s) const;
    size_t numStrings() const override { return 1; }

    void load(core::DataStream<ByteOrder>& ds) override;
    void save(core::DataStream<ByteOrder>& ds) override;
};

//----------------------------------------------------------------------------------------------------------------------

// Implementation of Constant

template <typename ByteOrder>
unsigned char* CodecConstant<ByteOrder>::encode(unsigned char* p, const double&) {
    return p;
}

template <typename ByteOrder>
void CodecConstant<ByteOrder>::decode(double* out) {
    (*out) = this->min_;
}

template <typename ByteOrder>
void CodecConstant<ByteOrder>::print(std::ostream& s) const {
    s << this->name_ << ", value=" << std::fixed << this->min_;
}

//----------------------------------------------------------------------------------------------------------------------

// Implementation of ConstantString

template <typename ByteOrder>
unsigned char* CodecConstantString<ByteOrder>::encode(unsigned char* p, const double& d) {
    return p;
}

template <typename ByteOrder>
void CodecConstantString<ByteOrder>::decode(double* out) {
    (*out) = this->min_;
}

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
    s << this->name_ << ", value='"
      << std::string(reinterpret_cast<const char*>(&this->min_), sizeof(double))
      << "'" << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace codec
} // namespace odc

#endif

