/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "ODBFormatBuffer.h"

#include "eckit/log/TimeStamp.h"
#include "eckit/runtime/Monitor.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Once.h"

using namespace eckit;

ODBFormatBuffer::ODBFormatBuffer() {}

void ODBFormatBuffer::beginLine()
{       
    *target() << begin_
              << std::setw(3)
              << std::setfill('0')
              << Monitor::instance().self()
              << std::setfill(' ') << ' '
              << TimeStamp() << ' ' << prefix_;
}

void ODBFormatBuffer::endLine()
{
    if(hasLoc_)
    {
        *target() << " " << where_;
        hasLoc_ = false; // we consumed the log location 
    }
        
    *target() << end_;
}

static Once<Mutex> local_mutex;

bool ODBFormatBuffer::dumpBuffer()
{
    AutoLock<Mutex> lock(local_mutex);
    
    Monitor::instance().out(pbase(),pptr());
    
    if( has_target() )
    {
        const char *p = pbase();
        const char *begin = p;
        while( p != pptr() )
        {
            if(start_)
            {
                this->beginLine();
                start_ = false;
            }
            
            if(*p == '\n')
            {
                this->process(begin,p);
                this->endLine();
                *os_ << *p;
                begin = ++p;
                start_ = true;
                continue;
            }            
            p++;
        }
        this->process(begin,p);
    }    
    setp(pbase(), epptr());
    return true;
}    
