/*
 * (C) Copyright 1996-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Simon Smart
/// @date Dec 2018

#ifndef odc_core_ThreadSharedDataHandle_H
#define odc_core_ThreadSharedDataHandle_H

#include <memory>
#include <mutex>

#include "eckit/io/DataHandle.h"

namespace eckit {
class DataHandle;
}

namespace odc {
namespace core {

//----------------------------------------------------------------------------------------------------------------------

class ThreadSharedDataHandle : public eckit::DataHandle {

public:  // methods

    ThreadSharedDataHandle(eckit::DataHandle& dh);
    ThreadSharedDataHandle(eckit::DataHandle* dh);
    ~ThreadSharedDataHandle() override;

    ThreadSharedDataHandle(const ThreadSharedDataHandle&);
    ThreadSharedDataHandle& operator=(const ThreadSharedDataHandle&);

    ThreadSharedDataHandle(ThreadSharedDataHandle&&);
    ThreadSharedDataHandle& operator=(ThreadSharedDataHandle&&);

    bool operator!=(const ThreadSharedDataHandle& other);
    bool operator==(const ThreadSharedDataHandle& other);

    void print(std::ostream& s) const override;

    eckit::Length openForRead() override;
    void openForWrite(const eckit::Length&) override;
    void openForAppend(const eckit::Length&) override;

    long read(void*, long) override;
    long write(const void*, long) override;
    void close() override;

    eckit::Length estimate() override;
    eckit::Offset position() override;
    eckit::Offset seek(const eckit::Offset&) override;

    std::string title() const override;

private:  // members

    struct Internal {

        Internal(eckit::DataHandle* dh, bool owned);
        ~Internal();

        std::mutex m_;
        eckit::DataHandle* dh_;
        bool owned_;
    };

    std::shared_ptr<Internal> internal_;

    eckit::Offset position_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace odc

#endif
