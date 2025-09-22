/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef Comparator_H
#define Comparator_H

#include <cmath>
#include <set>
#include <vector>

#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"

namespace eckit {
class PathName;
class DataHandle;
}  // namespace eckit

namespace odc {

namespace core {
class MetaData;
class Column;
}  // namespace core

const double maxAbsoluteError = 1e-9;
const double maxRelativeError = 1e-9;


class Comparator {
public:

    Comparator(bool skipTestingHaveMissing = false);

    void operator()() { run(); }
    void run();

    template <typename T1, typename T2>
    bool compare(T1& it1, const T1& end1, T2& it2, const T2& end2, const std::string& desc1, const std::string& desc2);

    template <typename T1, typename T2>
    bool compare(T1& it1, const T1& end1, T2& it2, const T2& end2, const std::string& desc1, const std::string& desc2,
                 const std::vector<std::string>& excludedColumnsTypes, const std::vector<std::string>& excludedColumns);

    void compare(eckit::DataHandle&, eckit::DataHandle&);
    void compare(const eckit::PathName&, const eckit::PathName&);

    void compare(const eckit::PathName& pathName1, const eckit::PathName& pathName2,
                 const std::vector<std::string>& excludedColumnsTypes, const std::vector<std::string>& excludedcolumns);

    void compare(const core::MetaData& metaData1, const core::MetaData& metaData2,
                 const std::set<std::string>& excludedColumnsTypes, const std::set<std::string>& excludedColumns,
                 std::vector<int>& skipCols);

    void compare(int nCols, const double* data1, const double* data2, const core::MetaData& metaData1,
                 const core::MetaData& metaData2);

    void compare(int nCols, const double* data1, const double* data2, const core::MetaData& metaData1,
                 const core::MetaData& metaData2, const std::vector<int>& skipCols);

    inline static double err(double A, double B) {
        double relativeError;

        if (fabs(A) <= maxAbsoluteError || fabs(B) <= maxAbsoluteError)
            relativeError = fabs(A - B);
        else if (fabs(B) > fabs(A))
            relativeError = fabs((A - B) / B);
        else
            relativeError = fabs((A - B) / A);
        return relativeError;
    }

    inline static int same(double A, double B) { return err(A, B) < maxRelativeError; }

    void raiseNotEqual(const core::Column&, double, double);

private:

    bool skipTestingHaveMissing_;
    long nRow_;
    bool NaN_isOK_;
};

template <typename T1, typename T2>
bool Comparator::compare(T1& it1, const T1& end1, T2& it2, const T2& end2, const std::string& desc1,
                         const std::string& desc2) {
    std::vector<std::string> noExcludedColumnsTypes;
    std::vector<std::string> noExcludedColumns;
    return compare(it1, end1, it2, end2, desc1, desc2, noExcludedColumnsTypes, noExcludedColumns);
}

template <typename T1, typename T2>
bool Comparator::compare(T1& it1, const T1& end1, T2& it2, const T2& end2, const std::string& desc1,
                         const std::string& desc2, const std::vector<std::string>& excludedColumnsTypes,
                         const std::vector<std::string>& excludedColumns) {
    eckit::Log::info() << "Comparator::compare: (1) " << desc1 << " to (2) " << desc2 << std::endl;

    nRow_ = 0;

    // The columns to skip are filled in by the column comparator function
    std::vector<int> skipCols;

    // Convert the excluded columns/types into sets, for more efficient lookups
    std::set<std::string> excludedColumnsTypesSet(excludedColumnsTypes.begin(), excludedColumnsTypes.end());
    std::set<std::string> excludedColumnsSet(excludedColumns.begin(), excludedColumns.end());

    compare(it1->columns(), it2->columns(), excludedColumnsTypesSet, excludedColumnsSet, skipCols);

    for (; it1 != end1 && it2 != end2; ++it1, ++it2) {
        ++nRow_;

        if (it1->isNewDataset())
            compare(it1->columns(), it2->columns(), excludedColumnsTypesSet, excludedColumnsSet, skipCols);
        if (it2->isNewDataset())
            compare(it1->columns(), it2->columns(), excludedColumnsTypesSet, excludedColumnsSet, skipCols);

        compare(it1->columns().size(), it1->data(), it2->data(), it1->columns(), it2->columns(), skipCols);
    }

    ASSERT("First file has more rows!" && !(it1 != end1));
    ASSERT("Second file has more rows!" && !(it2 != end2));
    return true;  // ?
}

}  // namespace odc

#endif
