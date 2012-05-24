#include "eclib/Logger.h"
#include "eclib/StandardBehavior.h"
#include "eclib/StdLogger.h"

#include "ODBBehavior.h"

Logger* ODBBehavior::createInfoLogger() { return new StdLogger( std::cerr ); }
Logger* ODBBehavior::createDebugLogger() { return new StdLogger( std::cerr ); } 
