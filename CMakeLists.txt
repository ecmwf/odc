# (C) Copyright 2011- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.

########################################################################################################################

cmake_minimum_required( VERSION 3.12 FATAL_ERROR )

find_package( ecbuild 3.4 REQUIRED HINTS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../ecbuild)

project( odc LANGUAGES C CXX )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

########################################################################################################################
### sanity checks

### long and double must be 64 bits exactly

# test by c compilation
ecbuild_check_c_source_return(
    "#include <stdio.h>\nint main(){printf(\"%ld\",sizeof(long));return 0;}"
    VAR check_long_final
    OUTPUT __sizeof_long_final
)
ecbuild_check_c_source_return(
    "#include <stdio.h>\nint main(){printf(\"%ld\",sizeof(double));return 0;}"
    VAR check_double_final
    OUTPUT __sizeof_double_final
)

if( NOT check_long_final OR NOT __sizeof_long_final EQUAL "8" )
    message( FATAL_ERROR "operating system ${CMAKE_SYSTEM} (${EC_OS_BITS} bits) and sizeof long [${__sizeof_long_final}] -- long must be 64 bits exactly" )
elseif( NOT check_double_final OR NOT __sizeof_double_final EQUAL "8" )
    message( FATAL_ERROR "operating system ${CMAKE_SYSTEM} (${EC_OS_BITS} bits) and sizeof double [${__sizeof_double_final}] -- double must be 64 bits exactly" )
endif()

# test by cxx compilation
ecbuild_check_cxx_source_return(
    "#include <iostream>\nint main(){ std::cout << sizeof(long);return 0;}"
    VAR cxx_check_long_final
    OUTPUT __cxx_sizeof_long_final
)
ecbuild_check_cxx_source_return(
    "#include <iostream>\nint main(){ std::cout << sizeof(double);return 0;}"
    VAR cxx_check_double_final
    OUTPUT __cxx_sizeof_double_final
)

if( NOT cxx_check_long_final OR NOT __cxx_sizeof_long_final EQUAL "8" )
    message( FATAL_ERROR "operating system ${CMAKE_SYSTEM} (${EC_OS_BITS} bits) and sizeof long [${__cxx_sizeof_long_final}] -- long must be 64 bits exactly" )
elseif( NOT cxx_check_double_final OR NOT __cxx_sizeof_double_final EQUAL "8" )
    message( FATAL_ERROR "operating system ${CMAKE_SYSTEM} (${EC_OS_BITS} bits) and sizeof double [${__cxx_sizeof_double_final}] -- double must be 64 bits exactly" )
endif()

check_type_size( long ODC_SIZEOF_LONG )
check_type_size( double ODC_SIZEOF_DOUBLE )

if( NOT ODC_SIZEOF_LONG EQUAL "8" )
    message( FATAL_ERROR "operating system ${CMAKE_SYSTEM} (${EC_OS_BITS} bits) and sizeof long [${ODC_SIZEOF_LONG}] -- long must be 64 bits exactly" )
elseif( NOT ODC_SIZEOF_LONG EQUAL "8" )
    message( FATAL_ERROR "operating system ${CMAKE_SYSTEM} (${EC_OS_BITS} bits) and sizeof double [${ODC_SIZEOF_LONG}] -- double must be 64 bits exactly" )
endif()

########################################################################################################################
### dependencies and options

ecbuild_find_package( NAME eckit VERSION 1.29 REQUIRED )

ecbuild_add_option( FEATURE FORTRAN
                    DESCRIPTION "whether or not to build the Fortran interface"
                    DEFAULT OFF )

if( HAVE_FORTRAN )
    ecbuild_enable_fortran( REQUIRED MODULE_DIRECTORY ${PROJECT_BINARY_DIR}/module )
endif()

########################################################################################################################
# contents

include(cmake/compiler_warnings.cmake) # optionally handle compiler specific warnings

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
