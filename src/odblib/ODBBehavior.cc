/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eclib/Logger.h"
#include "eclib/StandardBehavior.h"
#include "eclib/StdLogger.h"
#include "eclib/DHSStdLogger.h"
#include "eclib/RotLogger.h"

#include "ODBBehavior.h"

Logger* ODBBehavior::createInfoLogger()
{
    DHSLogger * logger = 0;
    if( ! toFile() )    
        logger = new DHSStdLogger( std::cerr);
    else
        logger = new RotLogger(); 
    
    logger->prefix("(I) ");
    
    return logger;
}


Logger* ODBBehavior::createDebugLogger() 
{    
    DHSLogger * logger = 0;
    if( ! toFile() )  
        logger = new DHSStdLogger( std::cerr);
    else
        logger = new RotLogger();        
    
    logger->prefix("(D) ");
    
    return logger;
}
