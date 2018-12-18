
/*
 * (C) Copyright 1996-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odc/core/ThreadSharedDataHandle.h"

namespace odc {
namespace core {

//----------------------------------------------------------------------------------------------------------------------


ThreadSharedDataHandle::Internal::Internal(eckit::DataHandle* dh, bool owned) :
    dh_(dh),
    owned_(owned) {

    if (owned_) {
        dh_->openForRead();
    }
}

ThreadSharedDataHandle::Internal::~Internal() {
    if (owned_) {
        dh_->close();
        delete dh_;
    }
}


ThreadSharedDataHandle::ThreadSharedDataHandle(eckit::DataHandle& dh) :
    internal_(std::make_shared<ThreadSharedDataHandle::Internal>(&dh, false)),
    position_(internal_->dh_->position()) {}


ThreadSharedDataHandle::ThreadSharedDataHandle(eckit::DataHandle* dh) :
    internal_(std::make_shared<ThreadSharedDataHandle::Internal>(dh, true)),
    position_(internal_->dh_->position()) {}


ThreadSharedDataHandle::~ThreadSharedDataHandle() {}

ThreadSharedDataHandle::ThreadSharedDataHandle(const ThreadSharedDataHandle& other):
    internal_(other.internal_),
    position_(other.position_) {}

ThreadSharedDataHandle& ThreadSharedDataHandle::operator=(const ThreadSharedDataHandle& rhs) {
    internal_ = rhs.internal_;
    position_ = rhs.position_;
    return *this;
}

ThreadSharedDataHandle::ThreadSharedDataHandle(ThreadSharedDataHandle&& other) :
    internal_(std::move(other.internal_)),
    position_(other.position_) {}

ThreadSharedDataHandle& ThreadSharedDataHandle::operator=(ThreadSharedDataHandle&& rhs) {
    std::swap(internal_, rhs.internal_);
    position_ = rhs.position_;
    return *this;
}

bool ThreadSharedDataHandle::operator!=(const ThreadSharedDataHandle& other) {
    return !(*this == other);
}

bool ThreadSharedDataHandle::operator==(const ThreadSharedDataHandle& other) {
    return (internal_ == other.internal_ && position_ == other.position_);
}


void ThreadSharedDataHandle::print(std::ostream& s) const {
    s << "ThreadSharedDataHandle(" << *internal_->dh_ << ")";
}

eckit::Length ThreadSharedDataHandle::openForRead() {
    ASSERT(internal_);
    std::lock_guard<std::mutex> lock(internal_->m_);
    return internal_->dh_->openForRead();
}

void ThreadSharedDataHandle::openForWrite(const eckit::Length&) { NOTIMP; }

void ThreadSharedDataHandle::openForAppend(const eckit::Length&) { NOTIMP; }

long ThreadSharedDataHandle::read(void* buffer, long length) {

    ASSERT(internal_);
    std::lock_guard<std::mutex> lock(internal_->m_);

    if (position_ != internal_->dh_->position()) {
        internal_->dh_->seek(position_);
    }

    long delta = internal_->dh_->read(buffer, length);
    position_ += delta;
    return delta;
}

long ThreadSharedDataHandle::write(const void*, long) { NOTIMP; }

void ThreadSharedDataHandle::close() {
    ASSERT(internal_);
    std::lock_guard<std::mutex> lock(internal_->m_);
    internal_->dh_->close();
}

eckit::Offset ThreadSharedDataHandle::position() {
    return position_;
}

eckit::Offset ThreadSharedDataHandle::seek(const eckit::Offset& position) {
    position_ = position;
    return position_;
}

//----------------------------------------------------------------------------------------------------------------------

}
}
