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

namespace odc {
namespace api {

//----------------------------------------------------------------------------------------------------------------------

class StridedData {

public: // types

    using value_type = char;

    using iterator = StridedData;

public: // methods

    // Construction and copy

    StridedData(void* data, size_t nelem, size_t dataSize, size_t stride, bool cnst=false) :
        data_(reinterpret_cast<char*>(data)), nelem_(nelem), dataSize_(dataSize), stride_(stride), const_(cnst) {}

    StridedData(const void* data, size_t nelem, size_t dataSize, size_t stride) :
        StridedData(const_cast<void*>(data), nelem, dataSize, stride, true),
        const_(true) {}

    ~StridedData() {}

    StridedData(const StridedData& rhs) = default;
    StridedData& operator=(const StridedData& rhs) = default;

    // Accessing the data

    size_t nelem() const { return nelem_; }
    size_t dataSize() const { return dataSize_; }
    size_t stride() const { return stride_; }

    value_type* operator[](int i) { ASSERT(!const_); return &data_[i*stride_]; }
    const value_type* operator[](int i) const { return &data_[i*stride_]; }

    value_type* operator*() { ASSERT(!const_); return data_; }
    const value_type* operator*() const { return data_; }

    // Iteration functionality

    iterator begin() const { return *this; }
    iterator end() const { return StridedData(data_ + (nelem_*stride_), 0, dataSize_, stride_, const_); }

    StridedData& operator++() {
        data_ += stride_;
        --nelem_;
        return *this;
    }

    StridedData& operator++(int) {
        auto ret = *this;
        ++(*this);
        return ret;
    }

    bool operator==(const StridedData& rhs) {
        return (data_ == rhs.data_ &&
                nelem_ == rhs.data_ &&
                const_ == rhs.const_ &&
                dataSize_ == rhs.dataSize_ &&
                stride_ == rhs.stride_);
    }
    bool operator!=(const StridedData& rhs) {
        return !(*this == rhs);
    }

private: // members

    value_type* data_;
    bool const_;

    size_t nelem_;
    size_t dataSize_;
    size_t stride_;
};


//----------------------------------------------------------------------------------------------------------------------

} // namespace api
} // namespace odc

#endif // odc_api_StridedData_H
