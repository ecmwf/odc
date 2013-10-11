%module pyodbdump
%{
#define SWIG_FILE_WITH_INIT


#include "odbdump.h"
#include <odbdump_sami.c>

%}

#include <errno.h>
%include "odbdump.h"

#include <odbdump_sami.c>

