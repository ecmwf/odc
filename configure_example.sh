#!/bin/sh

# This is a template of a script for building ODB API. See instructions in file INSTALL.
# Please edit values of the cmake options or delete them.

use odb

SRC=$(cd ../.. && pwd)

/usr/local/apps/cmake/new/bin/cmake ../.. \
    -DCMAKE_BUILD_TYPE=$(basename $(pwd) | sed 's/\W[a-zA-Z0-9]*//') \
    -DCMAKE_PREFIX_PATH=/usr/local/apps/python/2.7 \
    -DCMAKE_C_COMPILER=xlc \
    -DCMAKE_CXX_COMPILER=xlc++ \
    -DCMAKE_Fortran_COMPILER=xlf \
    -DCMAKE_INSTALL_PREFIX=$SRC/`cat ../../VERSION.cmake|awk '{print $3}'|sed 's/["]//g'`/  \
    -DODB_PATH=$ODB_ROOT \
    -DCMAKE_MODULE_PATH=$SRC/ecbuild/cmake \
    -DECLIB_SOURCE=$SRC/eclib \
    -DBUILD_SHARED_LIBS=OFF \
    -DODB_API_MIGRATOR=ON \
    -DODB_API_FORTRAN=ON \
    -DODB_API_PYTHON=ON \
    -DBISON_EXECUTABLE=/usr/local/apps/bison/current/bin/bison \
    -DSWIG_EXECUTABLE=/usr/local/apps/swig/current/bin/swig

#-DPGI_PATH=/path/to/pgf95 
#-DXLF_PATH=/path/to/xlf 
#-DFLEX_EXECUTABLE=/usr/local/apps/bison/current/bin/flex 

