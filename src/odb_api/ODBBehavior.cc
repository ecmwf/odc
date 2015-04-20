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
#include "ODBBehavior.h"


#include "eckit/log/Colour.h"
#include "eckit/log/Channel.h"
#include "eckit/log/ChannelBuffer.h"
#include "eckit/log/FormatChannel.h"

#include "eckit/runtime/Monitor.h"
#include "eckit/config/Resource.h"
#include "eckit/thread/ThreadSingleton.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#include "eckit/thread/Once.h"

#include "eckit/os/BackTrace.h"

#include "ODBBehavior.h"
#include "ODBFormatBuffer.h"
//#include "MarsApplication.h"

using namespace std;
using namespace eckit;

namespace odb {

ODBBehavior::ODBBehavior() {}
ODBBehavior::~ODBBehavior() {}

static Once<Mutex> local_mutex; /* protects the following global resources */

static bool useCerr = false;
static bool useNull = false;

static ChannelBuffer* create_dhs_log_target()
{
    AutoLock<Mutex> lock(local_mutex);
    return new ChannelBuffer(std::cerr);
}

struct CreateInfoChannel 
{
    Channel* operator()() 
    { 
        Channel* target = new Channel( create_dhs_log_target() );
        ColorizeFormat* colorizer = new ODBFormatBuffer();
        colorizer->prefix("(I) ");
        return new FormatChannel(target,colorizer);
    }
};

Channel& ODBBehavior::infoChannel()
{
    static ThreadSingleton<Channel,CreateInfoChannel> x;
    return x.instance();
}

struct CreateWarnChannel 
{
    Channel* operator()() 
    { 
        Channel* target = new Channel( create_dhs_log_target() );
        ColorizeFormat* colorizer = new ODBFormatBuffer();
        colorizer->setColor(&Colour::yellow);
        colorizer->resetColor(&Colour::reset);
        colorizer->prefix("(W) ");
        return new FormatChannel(target,colorizer);
    }
};

Channel& ODBBehavior::warnChannel()
{
    static ThreadSingleton<Channel, CreateWarnChannel> x;
    return x.instance();
}

struct CreateErrorChannel 
{
    Channel* operator()() 
    { 
        Channel* target = new Channel( create_dhs_log_target() );
        ColorizeFormat* colorizer = new ODBFormatBuffer();
        colorizer->setColor(&Colour::red);
        colorizer->resetColor(&Colour::reset);
        colorizer->prefix("(E) ");
        return new FormatChannel(target, colorizer);
    }
};

Channel& ODBBehavior::errorChannel()
{
    static ThreadSingleton<Channel, CreateErrorChannel> x;
    return x.instance();
}

struct CreateDebugChannel 
{
    Channel* operator()() 
    { 
        Channel* target = new Channel( create_dhs_log_target() );
        ColorizeFormat* colorizer = new ODBFormatBuffer();
        colorizer->prefix("(D) ");
        return new FormatChannel(target,colorizer);
    }
};

Channel& ODBBehavior::debugChannel()
{
    static ThreadSingleton<Channel,CreateDebugChannel> x;
    return x.instance();
}

} // namespace odb

template class eckit::ThreadSingleton<Channel,odb::CreateInfoChannel>;
template class eckit::ThreadSingleton<Channel,odb::CreateWarnChannel>;
template class eckit::ThreadSingleton<Channel,odb::CreateErrorChannel>;
template class eckit::ThreadSingleton<Channel,odb::CreateDebugChannel>;


