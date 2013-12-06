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
/// \file DirectAccessIterator.cc
///
/// @author Baudouin Raoult, Dec 2013

#include "odblib/DirectAccess.h"
#include "odblib/DirectAccessIterator.h"

using namespace eckit;

namespace odb {

DirectAccessIterator::DirectAccessIterator(DirectAccess& owner):
    owner_(owner),
    refCount_(0)
{
}

DirectAccessIterator::~DirectAccessIterator()
{
}

MetaData& DirectAccessIterator::columns() {
    return *owner_.block_->metaData();
}

double &DirectAccessIterator::data(size_t n) {
    size_t i = owner_.idx_;
    return *(owner_.block_->data() + i + n);
}

} // namespace odb

