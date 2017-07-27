/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef odb_api_tests_MockDataHandles_H
#define odb_api_tests_MockDataHandles_H

#include "eckit/io/DataHandle.h"

//----------------------------------------------------------------------------------------------------------------------

class MockReadDataHandle : public eckit::DataHandle {

public: // methods

    MockReadDataHandle(const std::vector<unsigned char>& buffer) :
        buffer_(buffer),
        position_(0) {}
    virtual ~MockReadDataHandle() {}

    virtual eckit::Length openForRead() {
        position_ = 0;
        return static_cast<long long>(buffer_.size());
    }

    virtual long read(void *p, long n) {
        ASSERT(n >= 0);
        ASSERT(position_ + n <= buffer_.size());
        ::memcpy(p, &buffer_[position_], static_cast<size_t>(n));
        position_ += n;
        return n;
    }

    virtual long write(const void*, long) { NOTIMP; }
    virtual void close() {}
    virtual void rewind() { NOTIMP; }
    virtual eckit::Length estimate() { NOTIMP; }
    virtual void openForWrite(const eckit::Length&) { NOTIMP; }
    virtual void openForAppend(const eckit::Length&) { NOTIMP; }
    virtual eckit::Offset position() { return eckit::Offset(position_); }
    virtual void print(std::ostream& s) const { s << "MockReadDataHandle()"; }

private:

    const std::vector<unsigned char>& buffer_;
    size_t position_;
};

// ------------------------------------------------------------------------------------------------------

class MockWriteDataHandle : public eckit::DataHandle {

public: // methods

    MockWriteDataHandle() :
        buffer_(1024),
        position_(0) {}
    virtual ~MockWriteDataHandle() {}

    virtual void openForWrite(const eckit::Length&) {
        position_ = 0;
    }

    virtual long write(const void* p, long n) {
        ASSERT(n >= 0);
        ASSERT(position_ + n <= buffer_.size());
        ::memcpy(&buffer_[position_], p, static_cast<size_t>(n));
        position_ += n;
        return n;
    }

    unsigned char* get() {
        return &buffer_[position_];
    }

    const unsigned char* getBuffer() const {
        return &buffer_[0];
    }

    void set(unsigned char* p) {
        ASSERT(p >= &buffer_[0]);
        ASSERT(p - &buffer_[0] < static_cast<long>(buffer_.size()));
        position_ = static_cast<size_t>(p - &buffer_[0]);
    }

    virtual long read(void*, long) { NOTIMP; }
    virtual void close() {}
    virtual void rewind() { NOTIMP; }
    virtual eckit::Length estimate() { NOTIMP; }
    virtual eckit::Length openForRead() { NOTIMP; }
    virtual void openForAppend(const eckit::Length&) { NOTIMP; }
    virtual eckit::Offset position() { return eckit::Offset(position_); }
    virtual void print(std::ostream& s) const { s << "MockWriteDataHandle()"; }

private:

    std::vector<unsigned char> buffer_;
    size_t position_;
};


//----------------------------------------------------------------------------------------------------------------------

#endif // odb_api_tests_MockDataHandles_H
