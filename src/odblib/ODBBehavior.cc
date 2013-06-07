/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <stdlib.h>

#include "eckit/log/LogBuffer.h"
#include "eckit/log/StdLogger.h"
#include "eckit/thread/ThreadSingleton.h"

#include "odblib/ODBBehavior.h"

using namespace std;
using namespace eckit;

//-----------------------------------------------------------------------------

namespace odb {

//-----------------------------------------------------------------------------

ODBBehavior::ODBBehavior()
{
}

ODBBehavior::~ODBBehavior()
{
}

//-----------------------------------------------------------------------------

LogStream& ODBBehavior::infoChannel()
{
    return errorChannel();
}

LogStream& ODBBehavior::warnChannel()
{
    return errorChannel();
}

LogStream& ODBBehavior::errorChannel()
{
    typedef NewAlloc1<ErrorStream,Logger*> Alloc;
    static ThreadSingleton<ErrorStream,Alloc> x( Alloc( new StdLogger( std::cerr ) ) );
    return x.instance();
}

LogStream& ODBBehavior::debugChannel()
{
    return errorChannel();
}

//-----------------------------------------------------------------------------

} // namespace odb
