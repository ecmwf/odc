/*
 * (C) Copyright 2019- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "odc/api/Odb.h"

#include "eckit/filesystem/PathName.h"
#include "eckit/log/Log.h"

#include "odc/core/TablesReader.h"
#include "odc/api/TableImpl.h"

using namespace eckit;

namespace odc {
namespace api {

//----------------------------------------------------------------------------------------------------------------------

// Internal API class definition

class OdbImpl {

public: // methods

    OdbImpl(const eckit::PathName& path);
    ~OdbImpl();

    const std::vector<Table>& tables();

private: // members

    eckit::PathName path_;

    std::vector<Table> tables_;
};

//----------------------------------------------------------------------------------------------------------------------

// API Forwarding

Odb::Odb(const std::string& path) :
    impl_(std::make_shared<OdbImpl>(path)) {}

Odb::~Odb() {}

const std::vector<Table>& Odb::tables() {
    return impl_->tables();
}

int Odb::numTables() {
    return impl_->tables().size();
}

//----------------------------------------------------------------------------------------------------------------------

// Implementation definition

OdbImpl::OdbImpl(const eckit::PathName& path) :
    path_(path) {

    Log::info() << "Construct: " << path << std::endl;
}

OdbImpl::~OdbImpl() {
    Log::info() << "Destroy: " << std::endl;
}

const std::vector<Table>& OdbImpl::tables() {

    // Lazily read the tables

    if (tables_.empty()) {

        core::TablesReader reader(path_);

        for (odc::core::Table& t : reader) {
            tables_.push_back(Table(std::make_shared<TableImpl>(t)));
        }
    }

    return tables_;
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace api
} // namespace odc
