/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odc/core/Span.h"

#include <cstring>

#include "eckit/exception/Exceptions.h"
#include "eckit/types/Types.h"


using namespace eckit;

namespace odc {
namespace core {

//----------------------------------------------------------------------------------------------------------------------

Span::Span(Offset start, Length length) :
    start_(start),
    length_(length) {}

Span::~Span() {}

void Span::extend(const Span& other) {

    if (start_ != other.start_) {
        length_ += other.length_;
    }

    for (const auto& kv : other.integerValues_) addValues(kv.first, kv.second);
    for (const auto& kv : other.realValues_) addValues(kv.first, kv.second);
    for (const auto& kv : other.stringValues_) addValues(kv.first, kv.second);
}

void Span::extend(Length length) {
    length_ += length;
}

void Span::addValue(const std::string& column, api::ColumnType t, double val) {

    switch (t) {

    case api::INTEGER:
    case api::BITFIELD:
        ASSERT(realValues_.find(column) == realValues_.end());
        ASSERT(stringValues_.find(column) == stringValues_.end());
        integerValues_[column].insert(static_cast<int64_t>(val));
        break;

    case api::REAL:
    case api::DOUBLE:
        ASSERT(integerValues_.find(column) == integerValues_.end());
        ASSERT(stringValues_.find(column) == stringValues_.end());
        realValues_[column].insert(val);
        break;

    case api::STRING: {
        ASSERT(realValues_.find(column) == realValues_.end());
        ASSERT(integerValues_.find(column) == integerValues_.end());
        const char* c = reinterpret_cast<const char*>(&val);
        stringValues_[column].insert(std::string(c, ::strnlen(c, sizeof(double))));
        }
        break;

    case api::IGNORE:
    default: {
        std::stringstream ss;
        ss << "Unsupported column type " << t << " found for column '" << column << "'";
        throw SeriousBug(ss.str(), Here());
    }
    };
}

void Span::addValues(const std::string& column, const std::set<double>& vals) {

    ASSERT(integerValues_.find(column) == integerValues_.end());
    ASSERT(stringValues_.find(column) == stringValues_.end());
    realValues_[column].insert(vals.cbegin(), vals.cend());
}

void Span::addValues(const std::string& column, const std::set<long>& vals) {

    ASSERT(realValues_.find(column) == realValues_.end());
    ASSERT(stringValues_.find(column) == stringValues_.end());
    integerValues_[column].insert(vals.begin(), vals.end());
}

void Span::addValues(const std::string& column, const std::set<std::string>& vals) {

    ASSERT(integerValues_.find(column) == integerValues_.end());
    ASSERT(realValues_.find(column) == realValues_.end());
    stringValues_[column].insert(vals.begin(), vals.end());
}

bool Span::operator==(const Span& rhs) const {
    return (stringValues_ == rhs.stringValues_ && realValues_ == rhs.realValues_ &&
            integerValues_ == rhs.integerValues_);
}

const std::set<long>& Span::getIntegerValues(const std::string& column) const {
    auto it = integerValues_.find(column);
    if (it == integerValues_.end()) {
        throw SeriousBug(std::string("Unknown integer column in span: " + column, Here()));
    }
    return it->second;
}

const std::set<double>& Span::getRealValues(const std::string& column) const {
    auto it = realValues_.find(column);
    if (it == realValues_.end()) {
        throw SeriousBug(std::string("Unknown real column in span: " + column, Here()));
    }
    return it->second;
}

const std::set<std::string>& Span::getStringValues(const std::string& column) const {
    auto it = stringValues_.find(column);
    if (it == stringValues_.end()) {
        throw SeriousBug(std::string("Unknown string column in span: " + column, Here()));
    }
    return it->second;
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace core
} // namespace odc

