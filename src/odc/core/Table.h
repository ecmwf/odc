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

#include "odc/core/ThreadSharedDataHandle.h"
#include "odc/MetaData.h"


namespace eckit { class DataHandle; }

namespace odc {
namespace core {

//----------------------------------------------------------------------------------------------------------------------


class Table {

public: // methods

    Table();
    Table(const ThreadSharedDataHandle& dh, eckit::Offset startPosition, eckit::Offset nextPosition, MetaData&& md, Properties&& props);

    eckit::Offset startPosition() const;
    eckit::Offset nextPosition() const;

    size_t numRows() const;

    // Construct a table. This is a static function rather than a constructor
    // so that we can return false for no-more-data rather than throwing an
    // exception

    static bool readTable(ThreadSharedDataHandle& dh, Table& t);

private: // members

    ThreadSharedDataHandle dh_;

    eckit::Offset startPosition_;
    eckit::Offset nextPosition_;

    MetaData metadata_;
    Properties properties_;
};


//----------------------------------------------------------------------------------------------------------------------

} // namespace core
} // namespace odc

#endif
