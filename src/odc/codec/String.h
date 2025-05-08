/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef odc_core_codec_String_H
#define odc_core_codec_String_H

#include "eckit/memory/Zero.h"
#include "odc/codec/Integer.h"
#include "odc/core/Codec.h"

namespace odc {
namespace codec {

//----------------------------------------------------------------------------------------------------------------------

/// @note CodecChars is _only_ used as an intermediate codec. It encodes data during the
///       normal Writer phase that is then _reencoded_ using Int16String,...
///       We should NEVER see 'chars' in the output data.

template <typename ByteOrder>
class CodecChars : public core::DataStreamCodec<ByteOrder> {

public:  // definitions

    constexpr static const char* codec_name() { return "chars"; }

public:  // methods

    CodecChars(api::ColumnType type, const std::string& name = codec_name());
    ~CodecChars() override {}

    using core::DataStreamCodec<ByteOrder>::load;
    using core::DataStreamCodec<ByteOrder>::save;
    void load(core::DataStream<ByteOrder>& ds) override;
    void save(core::DataStream<ByteOrder>& ds) override;

protected:  // methods

    std::unique_ptr<core::Codec> clone() override;

private:  // methods

    unsigned char* encode(unsigned char* p, const double& d) override;
    void decode(double* out) override;
    void skip() override;
    void gatherStats(const double& v) override;

    size_t numStrings() const override { return strings_.size(); }
    void copyStrings(core::Codec& rhs) override;

    size_t dataSizeDoubles() const override { return decodedSizeDoubles_; }
    void dataSizeDoubles(size_t count) override { decodedSizeDoubles_ = count; }

    void print(std::ostream& s) const override;

protected:  // members

    std::map<std::string, int64_t> stringLookup_;
    std::vector<std::string> strings_;
    size_t decodedSizeDoubles_;
};


//----------------------------------------------------------------------------------------------------------------------


template <typename ByteOrder, typename InternalCodec>
class IntStringCodecBase : public CodecChars<ByteOrder> {

    static_assert(std::is_same<typename InternalCodec::value_type, int64_t>::value, "Safety check");
    using InternalInt = typename InternalCodec::value_type;

public:  // methods

    IntStringCodecBase(api::ColumnType type, const std::string& name) :
        CodecChars<ByteOrder>(type, name), intCodec_(api::INTEGER) {

        this->min_          = odc::MDI::integerMDI();
        this->max_          = this->min_;
        this->missingValue_ = this->min_;
        intCodec_.min(0);
    }
    ~IntStringCodecBase() override {}

private:  // methods

    std::unique_ptr<core::Codec> clone() override {
        std::unique_ptr<core::Codec> cdc = CodecChars<ByteOrder>::clone();
        auto& c                          = static_cast<IntStringCodecBase<ByteOrder, InternalCodec>&>(*cdc);
        c.intCodec_.min(intCodec_.min());
        c.intCodec_.max(intCodec_.max());
        c.min(this->min());
        c.max(this->max());
        return cdc;
    }

    /// Ensure that data streams are passed through to the internal coder
    using CodecChars<ByteOrder>::setDataStream;
    void setDataStream(core::DataStream<ByteOrder>& ds) override {
        core::DataStreamCodec<ByteOrder>::setDataStream(ds);
        intCodec_.setDataStream(ds);
    }

    void clearDataStream() override {
        core::DataStreamCodec<ByteOrder>::clearDataStream();
        intCodec_.clearDataStream();
    }

    unsigned char* encode(unsigned char* p, const double& d) override {

        /// n.b. Yes this is ugly. This is a hack into the existing API - and it assumes
        ///      that the double& provided actually is the first element of a longer string.

        size_t len = ::strnlen(reinterpret_cast<const char*>(&d), this->decodedSizeDoubles_ * sizeof(double));
        std::string s(reinterpret_cast<const char*>(&d), len);

        auto it = this->stringLookup_.find(s);
        ASSERT(it != this->stringLookup_.end());

        // n.b. Reinterpret cast is yucky, but is for backward compatibility with old interface.
        // CodecInt*<, int64_t> undoes that internally.
        // WARNING: This is very type unsafe
        InternalInt internal = it->second;
        return static_cast<core::Codec&>(intCodec_).encode(p, reinterpret_cast<const double&>(internal));
    }

    void decode(double* out) override {

        // n.b. Reinterpret cast is yucky, but is for backward compatibility with old interface.
        // CodecInt*<, int64_t> undoes that internally.
        // WARNING: This is very type unsafe

        InternalInt i;
        static_cast<core::Codec&>(intCodec_).decode(reinterpret_cast<double*>(&i));

        ASSERT(i < long(this->strings_.size()));
        const std::string& s(this->strings_[i]);

        ::memset(out, 0, this->decodedSizeDoubles_ * sizeof(double));
        ::memcpy(reinterpret_cast<char*>(out), &s[0], std::min(s.length(), this->decodedSizeDoubles_ * sizeof(double)));
    }

    void skip() override { static_cast<core::Codec&>(intCodec_).skip(); }

    using CodecChars<ByteOrder>::load;
    void load(core::DataStream<ByteOrder>& ds) override {
        core::DataStreamCodec<ByteOrder>::load(ds);

        // Load the table of strings
        // This is based on the old-style hash-table storage, so it isn't a trivial list of strings
        int32_t numStrings;
        ds.read(numStrings);
        ASSERT(numStrings >= 0);

        this->strings_.resize(numStrings);

        // How many doubles-worth of memory is needed to decode the largest string?
        this->decodedSizeDoubles_ = 1;

        for (size_t i = 0; i < size_t(numStrings); i++) {
            std::string s;
            ds.read(s);

            int32_t cnt;
            ds.read(cnt);

            int32_t index;
            ds.read(index);

            ASSERT(index < numStrings);
            this->strings_[index] = s;

            if (s.length() != 0) {
                this->decodedSizeDoubles_ =
                    std::max(this->decodedSizeDoubles_, ((s.length() - 1) / sizeof(double)) + 1);
            }
        }

        // Ensure that the string lookup is EMPTY. We don't use it after reading
        ASSERT(this->stringLookup_.size() == 0);
    }

    using CodecChars<ByteOrder>::save;
    void save(core::DataStream<ByteOrder>& ds) override {

        core::DataStreamCodec<ByteOrder>::save(ds);

        ds.write(static_cast<int32_t>(this->strings_.size()));

        for (size_t i = 0; i < this->strings_.size(); i++) {
            ds.write(this->strings_[i]);
            ds.write(static_cast<int32_t>(0));  // "cnt" field is not used.
            ds.write(static_cast<int32_t>(i));
        }
    }

private:  // members

    InternalCodec intCodec_;
};

//----------------------------------------------------------------------------------------------------------------------


template <typename ByteOrder>
struct CodecInt8String : public IntStringCodecBase<ByteOrder, CodecInt8<ByteOrder, int64_t>> {
    constexpr static const char* codec_name() { return "int8_string"; }
    CodecInt8String(api::ColumnType type) :
        IntStringCodecBase<ByteOrder, CodecInt8<ByteOrder, int64_t>>(type, codec_name()) {}
    ~CodecInt8String() override {}
};


template <typename ByteOrder>
struct CodecInt16String : public IntStringCodecBase<ByteOrder, CodecInt16<ByteOrder, int64_t>> {
    constexpr static const char* codec_name() { return "int16_string"; }
    CodecInt16String(api::ColumnType type) :
        IntStringCodecBase<ByteOrder, CodecInt16<ByteOrder, int64_t>>(type, codec_name()) {}
    ~CodecInt16String() override {}
};


//----------------------------------------------------------------------------------------------------------------------

// Implementation

template <typename ByteOrder>
CodecChars<ByteOrder>::CodecChars(api::ColumnType type, const std::string& name) :
    core::DataStreamCodec<ByteOrder>(name, type), decodedSizeDoubles_(1) {}

template <typename ByteOrder>
unsigned char* CodecChars<ByteOrder>::encode(unsigned char* p, const double& s) {

    /// n.b. Yes this is ugly. This is a hack into the existing API - and it assumes
    ///      that the double& provided actually is the first element of a longer string.

    memcpy(p, &s, decodedSizeDoubles_ * sizeof(double));
    return p + (decodedSizeDoubles_ * sizeof(double));
}

template <typename ByteOrder>
void CodecChars<ByteOrder>::decode(double* out) {

    this->ds().readBytes(out, sizeof(double) * decodedSizeDoubles_);
}

template <typename ByteOrder>
void CodecChars<ByteOrder>::skip() {
    this->ds().advance(sizeof(double) * decodedSizeDoubles_);
}

template <typename ByteOrder>
void CodecChars<ByteOrder>::gatherStats(const double& v) {

    size_t len = ::strnlen(reinterpret_cast<const char*>(&v), decodedSizeDoubles_ * sizeof(double));
    std::string s(reinterpret_cast<const char*>(&v), len);

    if (stringLookup_.find(s) == stringLookup_.end()) {
        size_t index = strings_.size();
        strings_.push_back(s);
        stringLookup_[s] = index;
    }

    // In case the column is const, the const value will be copied and used by the optimized codec.
    // n.b. we don't just do this->min_ = minVal as there is no guarantee that the length of the
    //      string is >= 8 bytes. See AddressSanitizer failure on odc_test_codecs_write
    eckit::zero(this->min_);
    ::memcpy(&this->min_, &v, std::min(sizeof(double), len));
}


template <typename ByteOrder>
void CodecChars<ByteOrder>::load(core::DataStream<ByteOrder>& ds) {
    core::DataStreamCodec<ByteOrder>::load(ds);
    int32_t nStrings;
    ds.read(nStrings);
    ASSERT(nStrings == 0);  // No string table stored
}


template <typename ByteOrder>
void CodecChars<ByteOrder>::save(core::DataStream<ByteOrder>& ds) {
    // String table only stored in derived int-storing types
    core::DataStreamCodec<ByteOrder>::save(ds);
    ds.write(static_cast<int32_t>(0));
}

template <typename ByteOrder>
std::unique_ptr<core::Codec> CodecChars<ByteOrder>::clone() {

    std::unique_ptr<core::Codec> cdc = core::Codec::clone();
    auto& c                          = static_cast<CodecChars&>(*cdc);
    c.stringLookup_                  = stringLookup_;
    c.strings_                       = strings_;
    c.decodedSizeDoubles_            = decodedSizeDoubles_;
    ASSERT(c.min() == this->min_);
    ASSERT(c.max() == this->max_);
    return cdc;
}

template <typename ByteOrder>
void CodecChars<ByteOrder>::copyStrings(core::Codec& rhs) {
    CodecChars<ByteOrder>* c = dynamic_cast<CodecChars<ByteOrder>*>(&rhs);
    ASSERT(c);
    strings_      = c->strings_;
    stringLookup_ = c->stringLookup_;
}

template <typename ByteOrder>
void CodecChars<ByteOrder>::print(std::ostream& s) const {
    s << this->name_ << ", width=" << (decodedSizeDoubles_ * sizeof(double)) << ", #words=" << strings_.size();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace codec
}  // namespace odc

#endif
