%module pyodbdump
%{
#define SWIG_FILE_WITH_INIT

#include "odbdump.h"

%}


#include <errno.h>

%include "odbdump.h"

