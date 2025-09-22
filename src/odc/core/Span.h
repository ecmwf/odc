/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef odc_core_Span_H
#define odc_core_Span_H

#include <map>
#include <set>
#include <string>

#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"

#include "odc/api/ColumnType.h"

namespace odc {
namespace core {

//----------------------------------------------------------------------------------------------------------------------

// Describe the data extent, and the keys, spanned by an ODB

class Span {

public:  // methods

    Span(eckit::Offset start, eckit::Length length);
    ~Span();

    eckit::Offset offset() const { return start_; }
    eckit::Length length() const { return length_; }

    void extend(const Span& other);
    void extend(eckit::Length length);

    // Add the values _as_a_double_. Not nice, but compatible with old decoding api
    void addValue(const std::string& column, api::ColumnType t, double val);

    void addValues(const std::string& column, const std::set<long>& vals);
    void addValues(const std::string& column, const std::set<double>& vals);
    void addValues(const std::string& column, const std::set<std::string>& vals);

    bool operator==(const Span& rhs) const;

    template <typename T>
    void visit(T& visitor) {
        for (const auto& kv : integerValues_)
            visitor(kv.first, kv.second);
        for (const auto& kv : realValues_)
            visitor(kv.first, kv.second);
        for (const auto& kv : stringValues_)
            visitor(kv.first, kv.second);
    }

    const std::set<long>& getIntegerValues(const std::string& column) const;
    const std::set<double>& getRealValues(const std::string& column) const;
    const std::set<std::string>& getStringValues(const std::string& column) const;

private:  // members

    eckit::Offset start_;
    eckit::Length length_;

    std::map<std::string, std::set<long>> integerValues_;
    std::map<std::string, std::set<double>> realValues_;
    std::map<std::string, std::set<std::string>> stringValues_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace odc

#endif
