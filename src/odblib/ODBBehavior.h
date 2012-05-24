#ifndef ODBBehavior_H
#define ODBBehavior_H

#include "eclib/StandardBehavior.h"
#include "eclib/DHSBehavior.h"

class ODBBehavior : public DHSBehavior
{
    Logger* createInfoLogger();
    Logger* createDebugLogger();
};

#endif

