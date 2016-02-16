/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */
/// @author Piotr Kuchta, January 2016

#ifndef odb_api_LocalHandleFactory_H
#define odb_api_LocalHandleFactory_H

#include "experimental/eckit/ecml/data/DataHandleFactory.h"

class LocalHandleFactory : public eckit::DataHandleFactory
{
public:
    LocalHandleFactory();

protected:
    eckit::DataHandle* makeHandle(const std::string&) const;
};

#endif
