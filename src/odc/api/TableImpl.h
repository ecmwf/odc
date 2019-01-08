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

#ifndef odc_api_TableImpl_H
#define odc_api_TableImpl_H

#include "odc/core/Table.h"


namespace odc {
namespace api {

//----------------------------------------------------------------------------------------------------------------------

// Internal API class definition

class TableImpl {

public: // methods

    TableImpl(const core::Table& t);
    ~TableImpl();

    size_t numRows() const;
    size_t numColumns() const;

private: // members

    core::Table internal_;
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace api
} // namespace odc

#endif // odc_api_Odb_H
