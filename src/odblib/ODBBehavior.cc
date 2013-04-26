/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <stdlib.h>

#include "eclib/Application.h"
#include "eclib/Colour.h"
#include "eclib/DHSStdLogger.h"
#include "eclib/Log.h"
#include "eclib/Monitor.h"
#include "eclib/Resource.h"
#include "eclib/RotLogger.h"

#include "ODBBehavior.h"

namespace odb {

ODBBehavior::ODBBehavior() : taskId_(0), out_(cerr), name_() {}
ODBBehavior::~ODBBehavior() { eclib::Monitor::shutdown(); }

void ODBBehavior::initialize()
{
    try
    {
        eclib::Monitor::active( true );
        eclib::Monitor::startup();
        taskId_ = eclib::Monitor::self();
    }
    catch(exception& e) {
        std::cerr << "** " << e.what() << " Caught in " << Here() <<  std::endl;
        std::cerr << "** Exception is re-thrown" << std::endl;
        throw;
    }
}

eclib::DHSLogger* ODBBehavior::getLogger() { return new eclib::DHSStdLogger(out_); }

void ODBBehavior::finalize() {}

string ODBBehavior::runName() const { return name_.size() ? name_ : eclib::Application::appName(); }

void ODBBehavior::runName( const string& name ) { name_ = name; }

long ODBBehavior::taskId() const { return taskId_; }

eclib::Logger* ODBBehavior::createInfoLogger()
{
    eclib::DHSLogger * logger = getLogger();
    logger->prefix("(I) ");
    return logger;
}

eclib::Logger* ODBBehavior::createDebugLogger()
{    
    eclib::DHSLogger * logger = getLogger();
    logger->prefix("(D) ");
    return logger;
}

eclib::Logger* ODBBehavior::createWarningLogger()
{    
    eclib::DHSLogger * logger = getLogger();
    logger->prefix("(W) ");
    logger->setColor(&eclib::Colour::yellow);
    logger->resetColor(&eclib::Colour::reset);
    return logger;
}

eclib::Logger* ODBBehavior::createErrorLogger()
{    
    eclib::DHSLogger * logger = getLogger();
    logger->prefix("(E) ");
    logger->setColor(&eclib::Colour::red);
    logger->resetColor(&eclib::Colour::reset);
    return logger;
}

} // namespace odb 
 
