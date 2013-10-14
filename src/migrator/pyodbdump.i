%module pyodbdump
%{
#define SWIG_FILE_WITH_INIT

extern "C" {
#include "odbdump.h"
}

%}

#include <errno.h>

extern "C" {
%include "odbdump.h"
}

