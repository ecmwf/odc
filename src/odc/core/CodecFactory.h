/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Simon Smart
/// @date January 2019

#ifndef odc_core_CodecFactory_H
#define odc_core_CodecFactory_H

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>

#include "eckit/memory/NonCopyable.h"

#include "odc/ODBAPISettings.h"
#include "odc/api/ColumnType.h"
#include "odc/core/Exceptions.h"

namespace odc {
namespace core {

//----------------------------------------------------------------------------------------------------------------------

class CodecBuilderBase;
class Codec;
template <typename ByteOrder>
class DataStream;
struct SameByteOrder;
struct OtherByteOrder;


class CodecFactory : private eckit::NonCopyable {

public:  // methods

    CodecFactory();
    ~CodecFactory();

    static CodecFactory& instance();

    void enregister(const std::string& name, CodecBuilderBase& builder);
    void deregister(const std::string& name, CodecBuilderBase& builder);

    template <typename ByteOrder>
    std::unique_ptr<Codec> build(const std::string& name, api::ColumnType type) const;

    template <typename ByteOrder>
    std::unique_ptr<Codec> load(DataStream<ByteOrder>& ds, api::ColumnType type) const;

private:  // members

    mutable std::mutex m_;
    std::map<std::string, std::reference_wrapper<CodecBuilderBase>> builders_;
};


//----------------------------------------------------------------------------------------------------------------------


class CodecBuilderBase {

protected:  // methods

    CodecBuilderBase(const std::string& name);
    ~CodecBuilderBase();

public:  // methods

    virtual std::unique_ptr<Codec> make(const SameByteOrder&, api::ColumnType) const  = 0;
    virtual std::unique_ptr<Codec> make(const OtherByteOrder&, api::ColumnType) const = 0;

private:  // members

    std::string name_;
};

//----------------------------------------------------------------------------------------------------------------------

template <template <typename> class CODEC>
class CodecBuilder : public CodecBuilderBase {

#if !defined(_CRAYC) && !(defined(__GNUC__) && __GNUC__ < 8)
    static_assert(std::string_view(CODEC<SameByteOrder>::codec_name()) ==
                      std::string_view(CODEC<OtherByteOrder>::codec_name()),
                  "Invalid name");
#endif

public:  // methods

    CodecBuilder() : CodecBuilderBase(CODEC<SameByteOrder>::codec_name()) {}
    ~CodecBuilder() {}

private:  // methods

    std::unique_ptr<Codec> make(const SameByteOrder&, api::ColumnType type) const override {
        return std::unique_ptr<Codec>(new CODEC<SameByteOrder>(type));
    }
    std::unique_ptr<Codec> make(const OtherByteOrder&, api::ColumnType type) const override {
        return std::unique_ptr<Codec>(new CODEC<OtherByteOrder>(type));
    }
};

//----------------------------------------------------------------------------------------------------------------------

// For integers we have two representations. Either int64_t or double.
// This is for backward compatibility with the IFS

template <template <typename ByteOrder, typename ValueType> class CODEC_T>
class IntegerCodecBuilder : public CodecBuilderBase {

public:  // methods

    IntegerCodecBuilder() : CodecBuilderBase(CODEC_T<SameByteOrder, double>::codec_name()) {}
    ~IntegerCodecBuilder() {}

private:  // methods

    std::unique_ptr<Codec> make(const SameByteOrder&, api::ColumnType type) const override {
        if ((type == api::INTEGER || type == api::BITFIELD) && !ODBAPISettings::instance().integersAsDoubles()) {
            return std::unique_ptr<Codec>(new CODEC_T<SameByteOrder, int64_t>(type));
        }
        else {
            return std::unique_ptr<Codec>(new CODEC_T<SameByteOrder, double>(type));
        }
    }
    std::unique_ptr<Codec> make(const OtherByteOrder&, api::ColumnType type) const override {
        if ((type == api::INTEGER || type == api::BITFIELD) && !ODBAPISettings::instance().integersAsDoubles()) {
            return std::unique_ptr<Codec>(new CODEC_T<OtherByteOrder, int64_t>(type));
        }
        else {
            return std::unique_ptr<Codec>(new CODEC_T<OtherByteOrder, double>(type));
        }
    }
};

//----------------------------------------------------------------------------------------------------------------------

template <typename ByteOrder>
std::unique_ptr<Codec> CodecFactory::build(const std::string& name, api::ColumnType type) const {
    std::lock_guard<std::mutex> lock(m_);

    auto it = builders_.find(name);
    if (it == builders_.end())
        throw ODBDecodeError(std::string("Codec '") + name + "' not found", Here());
    return it->second.get().make(ByteOrder(), type);
}

template <typename ByteOrder>
std::unique_ptr<Codec> CodecFactory::load(DataStream<ByteOrder>& ds, api::ColumnType type) const {

    std::string codecName;
    ds.read(codecName);

    auto c = build<ByteOrder>(codecName, type);
    c->load(ds);
    return c;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace odc

#endif
