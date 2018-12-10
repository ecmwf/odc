/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */
/// @author Piotr Kuchta, October 2016

#ifndef odb_mars_HttpHandleFactory_H
#define odb_mars_HttpHandleFactory_H

#include "odb_api/data/DataHandleFactory.h"

namespace odc {

class HttpHandleFactory : public DataHandleFactory
{
public:
    HttpHandleFactory();
protected:
    eckit::DataHandle* makeHandle(const std::string&) const;
};

} // namespace odc

#endif
