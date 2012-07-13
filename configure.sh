#!/bin/sh

# This is a template of a script for building ODB API. See instructions in file INSTALL.
# Please edit values of the cmake options or delete them.

cmake -DCMAKE_BUILD_TYPE=$(basename $(pwd) | sed 's/\W[a-zA-Z0-9]*//') \
	-DCMAKE_C_COMPILER=gcc \
	-DCMAKE_CXX_COMPILER=g++ \
	-DCMAKE_Fortran_COMPILER=gfortran \
	-DCMAKE_INSTALL_PREFIX=/usr/local/lib/odb_api/`cat ../../VERSION.cmake|awk '{print $3}'|sed 's/["]//g'`/  \
    -DCMAKE_PREFIX_PATH=/usr/local/apps/python/2.7 \
	-DPGI_PATH=/path/to/pgf95 \
    -DODB_PATH=$ODB_ROOT \
    -DCMAKE_MODULE_PATH=/tmp/test/OdbAPI-0.9.20-dev-Source/ecbuild/cmake \
    -DECLIB_SOURCE=/tmp/test/OdbAPI-0.9.20-dev-Source/eclib \
    -DBUILD_SHARED_LIBS=ON \
    -DODB_API_MIGRATOR=ON \
    -DODB_API_FORTRAN=ON \
    -DODB_API_PYTHON=ON \
    -DSWIG_EXECUTABLE=swig \
    -DXLF_PATH=/path/to/xlf \
    -DBISON_EXECUTABLE=/path/to/bison \
    -DFLEX_EXECUTABLE=/path/to/flex
