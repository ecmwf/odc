/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

///
/// \file DirectAccessIterator.h
///
/// @author Baudouin Raoult, Dec 2013


#ifndef DirectAccessIterator_H
#define DirectAccessIterator_H

#include "eckit/eckit.h"

namespace odc {

class MetaData;

class DirectAccess;

class DirectAccessIterator
{
public:
    DirectAccessIterator (DirectAccess &owner);
    ~DirectAccessIterator ();

    MetaData& columns();
    double &data(size_t n);

    size_t refCount_;

private:

    DirectAccess& owner_;

};

} // namespace odc

#endif
