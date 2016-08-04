/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */
/// @author Piotr Kuchta, May 2015

#ifndef ecml_mars_MarsHandleFactory_H
#define ecml_mars_MarsHandleFactory_H

#include "ecml/data/DataHandleFactory.h"

namespace ecml {

class MarsHandleFactory : public ecml::DataHandleFactory
{
public:
    MarsHandleFactory();
protected:
    eckit::DataHandle* makeHandle(const std::string&) const;
};

} // namespace ecml

#endif
