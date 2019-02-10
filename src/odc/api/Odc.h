/*
 * (C) Copyright 2019- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


/// @author Simon Smart
/// @date January 2019

#ifndef odc_api_Odb_H
#define odc_api_Odb_H

#include <string>
#include <memory>
#include <vector>
#include <type_traits>

#include "odc/api/ColumnType.h"
#include "odc/api/StridedData.h"

namespace odc {
namespace api {

//----------------------------------------------------------------------------------------------------------------------

// Utility. proto- std::optional.

template <typename T>
struct Optional {
    Optional() : valid_(false) {}
    Optional(T&& v) : valid_(true) { new (&val_) T(std::forward<T>(v)); }
    ~Optional() { if(valid_) reinterpret_cast<T*>(&val_)->~T(); }
    explicit operator bool() const { return valid_; }
    T& get() { return *reinterpret_cast<T*>(&val_); }
    const T& get() const { return *reinterpret_cast<const T*>(&val_); }
private:
    typename std::aligned_storage<sizeof(T), alignof(T)>::type val_;
    bool valid_;
};

//----------------------------------------------------------------------------------------------------------------------

// Global settings

class Settings {
public: // methods

    static void treatIntegersAsDoubles(bool flag);

    static void setIntegerMissingValue(int64_t val);

    static void setDoubleMissingValue(double val);
};

//----------------------------------------------------------------------------------------------------------------------

class TableImpl;
class DecodeTarget;

class Table {

public: // methods

    Table();
    Table(std::shared_ptr<TableImpl> t);
    ~Table();

    size_t numRows() const;
    size_t numColumns() const;

    const std::string& columnName(int col) const;
    ColumnType columnType(int col) const;
    size_t columnDecodedSize(int col) const;

    void decode(DecodeTarget& target) const;

private: // members

    std::shared_ptr<TableImpl> impl_;
};

//----------------------------------------------------------------------------------------------------------------------

class DecodeTargetImpl;

class DecodeTarget {

public: // methods

    DecodeTarget(std::vector<StridedData>& columnFacades);
    ~DecodeTarget();

private: // members

    std::shared_ptr<DecodeTargetImpl> impl_;

    friend class Table;
};

//----------------------------------------------------------------------------------------------------------------------

class OdbImpl;

class Odb {

public: // methods

    Odb(const std::string& path);
    ~Odb();

    Optional<Table> next();

private: // members

    std::shared_ptr<OdbImpl> impl_;
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace api
} // namespace odc

#endif // odc_api_Odb_H
