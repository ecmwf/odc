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

#ifndef odc_core_DataStream_H
#define odc_core_DataStream_H

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "eckit/io/Buffer.h"
#include "eckit/io/Offset.h"

#include "odc/core/Exceptions.h"


namespace odc {
namespace core {

//----------------------------------------------------------------------------------------------------------------------

// Specialise behaviour for big/little endianness

struct SameByteOrder {
    /// With same byte order, we always to nothing!
    template <typename T>
    static void swap(T&) {}
    static void swap(char* addr, size_t size) {}
};


struct OtherByteOrder {
    template <typename T>
    static void swap(T& o) {
        std::reverse(reinterpret_cast<char*>(&o), reinterpret_cast<char*>(&o) + sizeof(T));
    }
    static void swap(char* addr, size_t size) { std::reverse(addr, addr + size); }
};

//----------------------------------------------------------------------------------------------------------------------

template <typename ByteOrder>
class DataStream {

public:

    DataStream(const void* data, size_t size, bool const = true);
    DataStream(const eckit::Buffer& data);
    DataStream(void* data, size_t size);
    DataStream(eckit::Buffer& data);
    ~DataStream();

    eckit::Offset position() const;

    // Reading

    template <typename T>
    void read(T& elem);
    template <typename T>
    void read(std::vector<T>& vec);
    template <typename T, typename S>
    void read(std::map<T, S>& props);
    void read(std::string& s);

    void read(void* addr, size_t bytes);
    void readBytes(void* addr, size_t bytes);  // ReadBytes does no endianness checks

    // Writing

    template <typename T>
    void write(const T& elem);
    template <typename T>
    void write(const std::vector<T>& vec);
    template <typename T, typename S>
    void write(const std::map<T, S>& props);
    void write(const std::string& s);

    void write(const void* addr, size_t bytes);
    void writeBytes(const void* addr, size_t bytes);  // ReadBytes does no endianness checks

    // These are a hack to get WriterBufferingIterator::doWriteRow to work.
    // TODO: Remove this hack.

    char* get() { return current_; }
    void set(char* p) {
        ASSERT(p >= start_);
        ASSERT(p <= end_);
        current_ = p;
    }
    void advance(size_t nbytes) {
        current_ += nbytes;
        ASSERT(current_ <= end_);
    }

private:  // members

    bool const_;
    char* start_;
    char* current_;
    char* end_;
};

//----------------------------------------------------------------------------------------------------------------------

// GeneralDataStream provides a way to store DataStreams.
// This can probably be done better, but I want the read/write functions to be both
// templated, and non-virtual, in the specific-data-order cases.

class GeneralDataStream {

public:

    GeneralDataStream() {}

    template <typename... Args>
    GeneralDataStream(bool otherByteOrder, Args&&... args) :
        sameDs_(otherByteOrder ? 0 : new DataStream<SameByteOrder>(std::forward<Args>(args)...)),
        otherDs_(otherByteOrder ? new DataStream<OtherByteOrder>(std::forward<Args>(args)...) : 0) {}

    ~GeneralDataStream() {}

    GeneralDataStream(GeneralDataStream&& rhs)            = default;
    GeneralDataStream& operator=(GeneralDataStream&& rhs) = default;

    bool isOther() const { return !!otherDs_; }
    DataStream<SameByteOrder>& same() {
        ASSERT(sameDs_);
        return *sameDs_;
    }
    DataStream<OtherByteOrder>& other() {
        ASSERT(otherDs_);
        return *otherDs_;
    }

    eckit::Offset position() const {
        ASSERT(sameDs_ || otherDs_);
        return sameDs_ ? sameDs_->position() : otherDs_->position();
    }

    template <typename... Args>
    void read(Args&&... args) {
        ASSERT(sameDs_ || otherDs_);
        sameDs_ ? sameDs_->read(std::forward<Args>(args)...) : otherDs_->read(std::forward<Args>(args)...);
    }

    template <typename... Args>
    void readBytes(Args&&... args) {
        ASSERT(sameDs_ || otherDs_);
        sameDs_ ? sameDs_->readBytes(std::forward<Args>(args)...) : otherDs_->readBytes(std::forward<Args>(args)...);
    }

    template <typename... Args>
    void write(Args&&... args) {
        ASSERT(sameDs_ || otherDs_);
        sameDs_ ? sameDs_->write(std::forward<Args>(args)...) : otherDs_->write(std::forward<Args>(args)...);
    }

    template <typename... Args>
    void writeBytes(Args&&... args) {
        ASSERT(sameDs_ || otherDs_);
        sameDs_ ? sameDs_->writeBytes(std::forward<Args>(args)...) : otherDs_->writeBytes(std::forward<Args>(args)...);
    }

private:  // members

    std::unique_ptr<DataStream<SameByteOrder>> sameDs_;
    std::unique_ptr<DataStream<OtherByteOrder>> otherDs_;
};


//----------------------------------------------------------------------------------------------------------------------

template <typename ByteOrder>
inline DataStream<ByteOrder>::DataStream(const void* data, size_t size, bool cnst) :
    const_(cnst),
    start_(const_cast<char*>(reinterpret_cast<const char*>(data))),
    current_(start_),
    end_(start_ + size) {}


template <typename ByteOrder>
inline DataStream<ByteOrder>::DataStream(const eckit::Buffer& buffer) :
    DataStream<ByteOrder>(buffer.data(), buffer.size()) {}


template <typename ByteOrder>
inline DataStream<ByteOrder>::DataStream(void* data, size_t size) : DataStream<ByteOrder>(data, size, false) {}


template <typename ByteOrder>
inline DataStream<ByteOrder>::DataStream(eckit::Buffer& buffer) : DataStream<ByteOrder>(buffer.data(), buffer.size()) {}


template <typename ByteOrder>
inline DataStream<ByteOrder>::~DataStream() {}


template <typename ByteOrder>
inline eckit::Offset DataStream<ByteOrder>::position() const {
    return static_cast<size_t>(current_ - start_);
}


template <typename ByteOrder>
template <typename T>
inline void DataStream<ByteOrder>::read(T& elem) {
    read(&elem, sizeof(elem));
}


template <typename ByteOrder>
template <typename T>
inline void DataStream<ByteOrder>::read(std::vector<T>& vec) {
    vec.clear();
    int32_t count;
    read(count);
    vec.resize(count);
    for (auto& elem : vec)
        read(elem);
}


template <typename ByteOrder>
template <typename T, typename S>
inline void DataStream<ByteOrder>::read(std::map<T, S>& props) {
    props.clear();
    int32_t count;
    read(count);
    for (int32_t i = 0; i < count; i++) {
        T first;
        S second;
        read(first);
        read(second);
        props.emplace(std::move(first), std::move(second));
    }
}


template <typename ByteOrder>
inline void DataStream<ByteOrder>::read(std::string& s) {

    int32_t len;
    read(len);

    s.resize(len);
    readBytes(&s[0], len);  // n.b. raw read. Bytes are in order.
}


// read --> takes endianness into account.
template <typename ByteOrder>
inline void DataStream<ByteOrder>::read(void* addr, size_t bytes) {
    readBytes(addr, bytes);
    ByteOrder::swap(reinterpret_cast<char*>(addr), bytes);
}


// readBytes is a raw read --> do not flip endianness.
template <typename ByteOrder>
inline void DataStream<ByteOrder>::readBytes(void* addr, size_t bytes) {

    char* newpos = current_ + bytes;

    if (newpos > end_) {
        std::stringstream ss;
        ss << "Attempting to read " << bytes << " bytes from DataStream with only " << (end_ - current_)
           << " bytes remaining";
        throw ODBEndOfDataStream(ss.str(), Here());
    }

    ::memcpy(addr, current_, bytes);
    current_ = newpos;
}


template <typename ByteOrder>
template <typename T>
inline void DataStream<ByteOrder>::write(const T& elem) {
    write(&elem, sizeof(elem));
}


template <typename ByteOrder>
template <typename T>
inline void DataStream<ByteOrder>::write(const std::vector<T>& vec) {
    int32_t len = vec.size();
    write(len);
    for (const auto& elem : vec)
        write(elem);
}


template <typename ByteOrder>
template <typename T, typename S>
inline void DataStream<ByteOrder>::write(const std::map<T, S>& props) {
    int32_t len = props.size();
    write(len);
    for (const auto& kv : props) {
        write(kv.first);
        write(kv.second);
    }
}


template <typename ByteOrder>
inline void DataStream<ByteOrder>::write(const std::string& s) {

    int32_t len = s.length();
    write(len);
    writeBytes(&s[0], len);  // n.b. raw read. Bytes are in order.
}


// read --> takes endianness into account.
template <typename ByteOrder>
inline void DataStream<ByteOrder>::write(const void* addr, size_t bytes) {
    writeBytes(addr, bytes);
    ByteOrder::swap(current_ - bytes, bytes);  // n.b. swap bytes on mutable target, not unknown source.
}


// readBytes is a raw read --> do not flip endianness.
template <typename ByteOrder>
inline void DataStream<ByteOrder>::writeBytes(const void* addr, size_t bytes) {

    ASSERT(!const_);

    char* newpos = current_ + bytes;

    if (newpos > end_) {
        std::stringstream ss;
        ss << "Attempting to write " << bytes << " to DataStream with only " << (end_ - current_) << " bytes remaining";
        throw ODBEndOfDataStream(ss.str(), Here());
    }

    ::memcpy(current_, addr, bytes);
    current_ = newpos;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace odc

#endif
