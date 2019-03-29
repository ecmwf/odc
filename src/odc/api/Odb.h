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
#include "odc/api/ColumnInfo.h"
#include "odc/api/StridedData.h"

namespace eckit {
    class DataHandle;
}


namespace odc {
namespace api {

//----------------------------------------------------------------------------------------------------------------------

// Utility. proto- std::optional.

template <typename T>
struct Optional {
    Optional() : valid_(false) {}
    Optional(T&& v) : valid_(true) { new (&val_) T(std::forward<T>(v)); }
    Optional(const Optional<T>& rhs) : valid_(rhs.valid_) { if (valid_) new (&val_) T(*reinterpret_cast<const T*>(&rhs.val_)); }
    Optional(Optional<T>&& rhs) : valid_(rhs.valid_) { if (valid_) new (&val_) T(std::move(*reinterpret_cast<T*>(&rhs.val_))); }
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

    static long integerMissingValue();
    static void setIntegerMissingValue(int64_t val);

    static double doubleMissingValue();
    static void setDoubleMissingValue(double val);

    static const std::string& version();
    static const std::string& gitsha1();
};

//----------------------------------------------------------------------------------------------------------------------

class TableImpl;
class DecodeTarget;

class Table {

public: // methods

    Table(std::shared_ptr<TableImpl> t);
    ~Table();

    size_t numRows() const;
    size_t numColumns() const;

    const std::vector<ColumnInfo>& columnInfo() const;

    void decode(DecodeTarget& target, size_t nthreads) const;

private: // members

    std::shared_ptr<TableImpl> impl_;
};

//----------------------------------------------------------------------------------------------------------------------

class DecodeTargetImpl;

class DecodeTarget {

public: // methods

    DecodeTarget(const std::vector<std::string>& columns,
                 std::vector<StridedData>& columnFacades);
    ~DecodeTarget();

    DecodeTarget slice(size_t rowOffset, size_t nrows) const;

private: // members

    std::shared_ptr<DecodeTargetImpl> impl_;

    friend class TableImpl;
};

//----------------------------------------------------------------------------------------------------------------------

class OdbImpl;

class Odb {

public: // methods

    Odb(const std::string& path);
    Odb(eckit::DataHandle& dh);
    Odb(eckit::DataHandle* dh); // takes ownership
    ~Odb();

    /// Can combine multiple frames into one logical frame. Row limit < 0 is unlimited.
    Optional<Table> next(bool aggregated=true, long rowlimit=-1);

private: // members

    std::shared_ptr<OdbImpl> impl_;
};

//----------------------------------------------------------------------------------------------------------------------

void encode(eckit::DataHandle& out,
            const std::vector<ColumnInfo>& columns,
            const std::vector<ConstStridedData>& data,
            size_t maxRowsPerFrame=10000);

//----------------------------------------------------------------------------------------------------------------------

/// importText returns number of lines imported

size_t importText(eckit::DataHandle& dh_in, eckit::DataHandle& dh_out, const std::string& delimiter=",");
size_t importText(std::istream& is, eckit::DataHandle& dh_out, const std::string& delimiter=",");
size_t importText(const std::string& in, eckit::DataHandle& dh_out, const std::string& delimiter=",");

//----------------------------------------------------------------------------------------------------------------------

} // namespace api
} // namespace odc

#endif // odc_api_Odb_H
