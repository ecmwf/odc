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

#ifndef odc_core_Table_H
#define odc_core_Table_H

#include <memory>

#include "eckit/io/Buffer.h"

#include "odc/core/ThreadSharedDataHandle.h"
#include "odc/MetaData.h"


namespace eckit { class DataHandle; }

namespace odc {
namespace core {

class DecodeTarget;

//----------------------------------------------------------------------------------------------------------------------


class Table {

public: // methods

    // Construct a table. This is a static function rather than a constructor
    // so that we can return false for no-more-data rather than throwing an
    // exception

    static std::unique_ptr<Table> readTable(ThreadSharedDataHandle& dh);

    eckit::Offset startPosition() const;
    eckit::Offset nextPosition() const;
    eckit::Length encodedDataSize() const;

    size_t numRows() const;
    size_t numColumns() const;
    int32_t byteOrder() const;

    const MetaData& columns() const;
    const Properties& properties() const;

    eckit::Buffer readEncodedData();

    void decode(DecodeTarget& target);

private: // methods

    Table(const ThreadSharedDataHandle& dh);

private: // members

    ThreadSharedDataHandle dh_;

    eckit::Offset startPosition_;
    eckit::Offset dataPosition_;
    eckit::Length dataSize_;
    eckit::Offset nextPosition_;
    int32_t byteOrder_;

    MetaData metadata_;
    Properties properties_;
};


//----------------------------------------------------------------------------------------------------------------------

} // namespace core
} // namespace odc

#endif
