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
