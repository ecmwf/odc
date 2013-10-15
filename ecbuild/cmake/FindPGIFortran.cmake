# (C) Copyright 1996-2012 ECMWF.
# 
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
# In applying this licence, ECMWF does not waive the privileges and immunities 
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.

###############################################################################
# FORTRAN support

list( APPEND pg_libs  pgmp pgbind numa pgf90 pgf90_rpm1 pgf902 pgf90rtl  pgftnrtl nspgc pgc rt pgsse1 pgsse2 )
foreach( pglib ${pg_libs} )
    if( DEFINED PGI_PATH )
        if( DEFINED BUILD_SHARED_LIBS AND BUILD_SHARED_LIBS STREQUAL "ON" )
            find_library( ${pglib}_lib  ${pglib} PATH ${PGI_PATH}/libso NO_DEFAULT_PATH )
        else()
            find_library( ${pglib}_lib  ${pglib} PATH ${PGI_PATH}/lib NO_DEFAULT_PATH )
        endif()
    else()
        find_library( ${pglib}_lib  ${pglib} )
    endif()
    if( ${pglib}_lib )
        list( APPEND PGIFORTRAN_LIBRARIES ${${pglib}_lib} )
        debug_var( ${pglib}_lib )
    endif()
endforeach()
