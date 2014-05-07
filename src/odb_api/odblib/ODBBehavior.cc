/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/log/ChannelBuffer.h"
#include "eckit/thread/ThreadSingleton.h"
#include "odb_api/odblib/ODBBehavior.h"

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

struct CreateErrChannel 
{
    Channel* operator()() { return  new Channel(  new ChannelBuffer(std::cerr) ); }
};

//-----------------------------------------------------------------------------

Channel& ODBBehavior::infoChannel()
{
    return errorChannel();
}

Channel& ODBBehavior::warnChannel()
{
    return errorChannel();
}

Channel& ODBBehavior::errorChannel()
{
    static ThreadSingleton<Channel,CreateErrChannel> x;
    return x.instance();
}

Channel& ODBBehavior::debugChannel()
{
    return errorChannel();
}

//-----------------------------------------------------------------------------

} // namespace odb
