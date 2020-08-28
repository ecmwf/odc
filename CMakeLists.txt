# (C) Copyright 2011- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.

########################################################################################################################

cmake_minimum_required( VERSION 3.12 FATAL_ERROR )

find_package( ecbuild 3.4 REQUIRED )

project( odc LANGUAGES C CXX )

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

########################################################################################################################
### dependencies and options

ecbuild_find_package( NAME eckit VERSION 1.4 REQUIRED )

#ecbuild_add_option( FEATURE NETCDF
#                    DESCRIPTION "whether or not to build the odb2netcdf tool"
#                    DEFAULT OFF
#                    REQUIRED_PACKAGES "NetCDF COMPONENTS CXX" )

ecbuild_add_option( FEATURE FORTRAN
                    DESCRIPTION "whether or not to build the Fortran interface"
                    DEFAULT OFF )

if( HAVE_FORTRAN )
    ecbuild_enable_fortran( REQUIRED MODULE_DIRECTORY ${PROJECT_BINARY_DIR}/module )
endif()

########################################################################################################################
# contents

add_subdirectory( src )
add_subdirectory( regressions )
add_subdirectory( tests )

############################################################################################
# finalize

ecbuild_pkgconfig( NAME ${PROJECT_NAME}
                   DESCRIPTION "ECMWF encoding and decoding of observational data in ODB2 format"
                   URL "https://software.ecmwf.int/wiki/display/ODC"
                   LIBRARIES odccore )

ecbuild_install_project( NAME odc )

ecbuild_print_summary()
