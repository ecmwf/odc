/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef odc_core_Column_H
#define odc_core_Column_H

#include <memory>

#include "eckit/os/BackTrace.h"
#include "eckit/sql/SQLTypedefs.h"
#include "odc/api/ColumnType.h"
#include "odc/core/Codec.h"

namespace eckit {
class DataHandle;
}

namespace odc {
namespace core {

//----------------------------------------------------------------------------------------------------------------------

class MetaData;

class Column {

public:

    Column(MetaData&);
    Column(const Column&);

    static const char* columnTypeName(api::ColumnType type);

    static api::ColumnType type(const std::string&);

    Column& operator=(const Column&);

    bool operator==(const Column&) const;
    bool operator!=(const Column& other) const { return !(*this == other); }
    bool equals(const Column& other, bool compareDataSizes = true) const;

    virtual ~Column();

    template <typename ByteOrder>
    void load(DataStream<ByteOrder>& ds);
    template <typename ByteOrder>
    void save(DataStream<ByteOrder>& ds);

    Codec& coder() const { return *coder_; }
    void coder(std::unique_ptr<Codec> c) { std::swap(coder_, c); }

    size_t dataSizeDoubles() const { return coder_->dataSizeDoubles(); }
    void dataSizeDoubles(size_t count) { coder_->dataSizeDoubles(count); }

    void name(const std::string name) { name_ = name; }
    const std::string& name() const { return name_; }

    template <typename ByteOrder>
    void type(api::ColumnType t);
    template <typename ByteOrder>
    void resetCodec();

    void type(api::ColumnType t) { type_ = t; }
    api::ColumnType type() const { return api::ColumnType(type_); }

    bool hasInitialisedCoder() const { return coder_ != 0; }
    bool isConstant();

    /// Delegations to Codec:

    void hasMissing(bool h) { coder_->hasMissing(h); }
    int32_t hasMissing() const { return coder_->hasMissing(); }

    void min(double m) { coder_->min(m); }
    double min() const { return coder_->min(); }

    void max(double m) { coder_->max(m); }
    double max() const { return coder_->max(); }

    void missingValue(double v) { coder_->missingValue(v); }
    double missingValue() const { return coder_->missingValue(); }
    double rawMissingValue() const { return coder_->rawMissingValue(); }

    void resetStats() {
        ASSERT(coder_);
        coder_->resetStats();
    }

    void bitfieldDef(const eckit::sql::BitfieldDef& b) { bitfieldDef_ = b; }
    const eckit::sql::BitfieldDef& bitfieldDef() const { return bitfieldDef_; }

    virtual void print(std::ostream& s) const;

#ifdef SWIGPYTHON
    const std::string __repr__() {
        return  // std::string("<") +
            name_ + ":" + columnTypeName(odc::api::ColumnType(type_))
            //+ ">"
            ;
    }
    const std::string __str__() { return name_; }
#endif

    friend std::ostream& operator<<(std::ostream& s, const Column& p) {
        p.print(s);
        return s;
    }

private:

    MetaData& owner_;
    std::string name_;
    /// Note: type_ should be ColumnType, but it is saved on file so must be of a fixed size type.
    int32_t type_;
    std::unique_ptr<Codec> coder_;
    /// bitfieldDef_ is not empty if type_ == BITFIELD.
    eckit::sql::BitfieldDef bitfieldDef_;
    // std::string typeSignature_;
};

//----------------------------------------------------------------------------------------------------------------------

template <typename ByteOrder>
void Column::load(DataStream<ByteOrder>& ds) {
    ds.read(name_);
    ds.read(type_);
    if (type_ == api::BITFIELD) {
        eckit::sql::FieldNames& names(bitfieldDef_.first);
        eckit::sql::Sizes& sizes(bitfieldDef_.second);

        static_assert(std::is_same<std::remove_reference<decltype(names)>::type::value_type, std::string>::value,
                      "Format sanity check");
        static_assert(std::is_same<std::remove_reference<decltype(sizes)>::type::value_type, int32_t>::value,
                      "Format sanity check");

        names.clear();
        sizes.clear();

        ds.read(names);
        ds.read(sizes);
        ASSERT(names.size() == sizes.size());
    }

    coder_ = CodecFactory::instance().load(ds, api::ColumnType(type_));
}

template <typename ByteOrder>
void Column::save(DataStream<ByteOrder>& ds) {

    ds.write(name_);
    ds.write(static_cast<int32_t>(type_));

    if (type_ == api::BITFIELD) {
        eckit::sql::FieldNames& names(bitfieldDef_.first);
        eckit::sql::Sizes& sizes(bitfieldDef_.second);

        static_assert(std::is_same<std::remove_reference<decltype(names)>::type::value_type, std::string>::value,
                      "Format sanity check");
        static_assert(std::is_same<std::remove_reference<decltype(sizes)>::type::value_type, int32_t>::value,
                      "Format sanity check");
        ASSERT(names.size() == sizes.size());

        ds.write(names);
        ds.write(sizes);
    }

    // And write the coder name
    ds.write(coder_->name());

    coder_->save(ds);
}

template <typename ByteOrder>
void Column::type(api::ColumnType t) {
    using namespace odc::api;
    type_ = t;
    std::string codecName;
    switch (type_) {
        case INTEGER:
            codecName = "int32";
            break;
        case BITFIELD:
            // TODO: 'unsigned_int64'
            codecName = "int32";
            break;
        case REAL:
            codecName = "long_real";
            break;
        case DOUBLE:
            codecName = "long_real";
            break;
        case STRING:
            codecName = "chars";
            break;
        default:
            ASSERT(!"Type not supported");
            break;
    }

    coder(CodecFactory::instance().build<ByteOrder>(codecName, t));

    if (type_ == BITFIELD)
        missingValue(MDI::bitfieldMDI());
}

template <typename ByteOrder>
void Column::resetCodec() {
    int ds = dataSizeDoubles();  // Preserve byte size (part of type information)
    type<ByteOrder>(static_cast<api::ColumnType>(type_));
    dataSizeDoubles(ds);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace odc

#endif
