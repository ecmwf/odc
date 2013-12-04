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
/// \file ODA.cc
///
/// @author Baudouin Raoult, Dec 2013

#include "odblib/DirectAccess.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <errno.h>
#include <math.h>
#include <climits>
#include <cfloat>

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/DataHandle.h"
#include "eckit/io/FileHandle.h"

using namespace std;
using namespace eckit;

namespace odb {

DirectAccess::DirectAccess(DataHandle &dh)
    : dataHandle_(&dh),
      deleteDataHandle_(false)
{}

DirectAccess::DirectAccess()
    : dataHandle_(0),
      deleteDataHandle_(true),
      path_("")
{}

DirectAccess::DirectAccess(const std::string& path)
    : dataHandle_(new FileHandle(path)),
      deleteDataHandle_(true),
      path_(path)
{
    dataHandle_->openForRead();
}

DirectAccess::~DirectAccess()
{
    if (dataHandle_ && deleteDataHandle_)
    {
        dataHandle_->close();
        delete dataHandle_;
    }
}

DirectAccessIterator* DirectAccess::createReadIterator(const PathName& pathName)
{
    return new DirectAccessIterator(*this, pathName);
}

DirectAccessIterator* DirectAccess::createReadIterator()
{
    return createReadIterator(path_);
}

DirectAccess::iterator DirectAccess::begin()
{
    DirectAccessIterator * it = new DirectAccessIterator(*this);
    it->next();
    return iterator(it);
}

const DirectAccess::iterator DirectAccess::end() { return iterator(0); }

} // namespace odb
