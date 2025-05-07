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

#ifndef odc_api_StridedData_H
#define odc_api_StridedData_H

#include <string.h>
#include <algorithm>
#include <cstdint>

#include "eckit/exception/Exceptions.h"


namespace odc {
namespace api {

//----------------------------------------------------------------------------------------------------------------------

/** Describes the layout of periodic data in memory. This is the template base class for the StridedData and
 * ConstStridedData types. */
template <typename value_type>
class StridedDataT {

public:  // types

    using const_value_type = typename std::add_const<value_type>::type;

    using iterator       = StridedDataT<value_type>;
    using const_iterator = StridedDataT<const_value_type>;

private:  // types

    using void_arg_t = typename std::conditional<std::is_const<value_type>::value, const void*, void*>::type;

public:  // methods

    // Construction and copy

    /** Constructor
     * \param data Data array
     * \param nelem Number of data elements
     * \param dataSize Size of each data element
     * \param stride Size between periodic data elements
     */
    StridedDataT(void_arg_t data, size_t nelem, size_t dataSize, size_t stride) :
        data_(reinterpret_cast<value_type*>(data)), nelem_(nelem), dataSize_(dataSize), stride_(stride) {}
    StridedDataT() : StridedDataT(0, 0, 0, 0) {}

    ~StridedDataT() {}

    StridedDataT(const StridedDataT<value_type>& rhs)                        = default;
    StridedDataT<value_type>& operator=(const StridedDataT<value_type>& rhs) = default;

    // Slice the StridedData to get a sub-strided-data

    /** Returns a new object which references a contiguous subset of the data elements of the original
     * \param rowOffset Data element offset where to start slicing
     * \param nrows Number of data elements to slice
     * \returns Subset of current data
     */
    StridedDataT<value_type> slice(size_t rowOffset, size_t nrows) {
        ASSERT(rowOffset + nrows <= nelem_);
        return StridedDataT<value_type>(get(rowOffset), nrows, dataSize_, stride_);
    }

    StridedDataT<value_type> slice(size_t rowOffset, size_t nrows) const {
        ASSERT(rowOffset + nrows <= nelem_);
        return StridedDataT<const_value_type>(get(rowOffset), nrows, dataSize_, stride_);
    }

    // Accessing the data

    /** Returns number of data elements
     * \returns Number of data elements
     */
    size_t nelem() const { return nelem_; }
    /** Returns size of each data element
     * \returns Size of each data element
     */
    size_t dataSize() const { return dataSize_; }
    /** Returns size between periodic data elements
     * \returns Size between periodic data elements
     */
    size_t stride() const { return stride_; }

    /** Returns the address of the i'th data element (mutable)
     * \param i Periodic offset
     * \returns Target mutable data element
     */
    value_type* get(int i) { return &data_[i * stride_]; }
    /** Returns the address of the i'th data element (const)
     * \param i Periodic offset
     * \returns Target constant data element
     */
    const_value_type* get(int i) const { return &data_[i * stride_]; }

    value_type* operator[](int i) { return get(i); }
    const_value_type* operator[](int i) const { return get(i); }

    value_type* operator*() { return data_; }
    const_value_type* operator*() const { return data_; }

    /** Copy the value contained in one data element into the following contiguous elements.
     * \param sourceRow Source data element offset
     * \param finalRow Target data element offset
     */
    void fill(int sourceRow, int finalRow);

    /** Checks if data element differs from the previous data element
     * \param row Data element offset to check
     * \param *True* if data element does not exist yet, *false* otherwise
     */
    bool isNewValue(size_t row) const {
        if (row == 0)
            return true;
        return ::memcmp(get(row), get(row - 1), dataSize_) != 0;
    }

    // Iteration functionality

    iterator begin() { return *this; }
    iterator end() { return iterator{data_ + (nelem_ * stride_), 0, dataSize_, stride_}; }

    const_iterator begin() const { return const_iterator{data_, nelem_, dataSize_, stride_}; }
    const_iterator end() const { return const_iterator{data_ + (nelem_ * stride_), 0, dataSize_, stride_}; }

    StridedDataT<value_type>& operator++() {
        data_ += stride_;
        --nelem_;
        return *this;
    }

    StridedDataT<value_type> operator++(int) {
        auto ret = *this;
        ++(*this);
        return ret;
    }

    bool operator==(const StridedDataT<value_type>& rhs) const {
        return (data_ == rhs.data_ && nelem_ == rhs.nelem_ && dataSize_ == rhs.dataSize_ && stride_ == rhs.stride_);
    }
    bool operator!=(const StridedDataT<value_type>& rhs) const { return !(*this == rhs); }

private:  // methods

    friend std::ostream& operator<<(std::ostream& o, const StridedDataT<value_type>& s) {
        o << "StridedData(0x" << (void*)s.data_ << "-" << s.dataSize_ << ":" << s.stride_ << "x" << s.nelem_ << ")";
        return o;
    }

private:  // members

    value_type* data_;

    size_t nelem_;
    size_t dataSize_;
    size_t stride_;
};

//----------------------------------------------------------------------------------------------------------------------

template <typename value_type>
inline void StridedDataT<value_type>::fill(int sourceRow, int finalRow) {

    ASSERT(sourceRow <= finalRow);

    // Specialisations for speed

    if (dataSize_ == 8) {
        if (stride_ == 8) {
            std::fill(reinterpret_cast<uint64_t*>(get(sourceRow + 1)), reinterpret_cast<uint64_t*>(get(finalRow + 1)),
                      *reinterpret_cast<uint64_t*>(get(sourceRow)));
        }
        else {
            uint64_t src  = *reinterpret_cast<uint64_t*>(get(sourceRow));
            uint64_t* end = reinterpret_cast<uint64_t*>(get(finalRow));
            for (uint64_t* p = reinterpret_cast<uint64_t*>(get(sourceRow + 1)); p <= end;
                 p           = reinterpret_cast<uint64_t*>(reinterpret_cast<char*>(p) + stride_)) {
                *p = src;
            }
        }
    }
    else {
        char* src = get(sourceRow);
        for (int row = sourceRow + 1; row <= finalRow; ++row) {
            ::memcpy(get(row), src, dataSize_);
        }
    }
}

/** Describes the layout of periodic data in memory (mutable) */
typedef StridedDataT<char> StridedData;
/** Describes the layout of periodic data in memory (const) */
typedef StridedDataT<const char> ConstStridedData;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace api
}  // namespace odc

#endif  // odc_api_StridedData_H
