############################################################################################
# cmake options:
#
#       -DCMAKE_BUILD_TYPE=Debug|RelWithDebInfo|Release|Production
#       -DCMAKE_INSTALL_PREFIX=/path/to/install
#
#       -DCMAKE_MODULE_PATH=/path/to/ecbuild/cmake
#
#       -DCMAKE_C_COMPILER=gcc
#       -DCMAKE_C_COMPILER=g++
#
#       -DCMAKE_PREFIX_PATH=/path/to/any/package/out/of/place
#       -DBUILD_SHARED_LIBS=ON

cmake_minimum_required( VERSION 2.8.11 FATAL_ERROR )

project( odb_api CXX )

# Add path for custom modules
set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../ecbuild/cmake")

include( ecbuild_system NO_POLICY_SCOPE )

ecbuild_requires_macro_version( 1.9 )

###############################################################################
# local project

ecbuild_use_package( PROJECT eckit VERSION 0.9 REQUIRED )
ecbuild_use_package( PROJECT metkit VERSION 0.3 REQUIRED )

ecbuild_declare_project()

ecbuild_add_resources( TARGET ${PROJECT_NAME}_no_odb DONT_PACK_REGEX "*.odb" )
ecbuild_add_resources( TARGET ${PROJECT_NAME}_no_oda DONT_PACK_REGEX "*.oda" )

ecbuild_dont_pack(REGEX "*.odb")
ecbuild_dont_pack(REGEX ".gdbinit")

###############################################################################
# some variables/options of this project

ecbuild_add_option( FEATURE MIGRATOR
                    DESCRIPTION "whether or not to build the ODB migrator tool"
                    DEFAULT OFF
                    REQUIRED_PACKAGES "PROJECT odb VERSION 1.0 QUIET" )

ecbuild_add_option( FEATURE FORTRAN
                    DESCRIPTION "whether or not to build the Fortran interface"
                    DEFAULT OFF )

ecbuild_add_option( FEATURE NETCDF
                    DESCRIPTION "whether or not to build the odb2netcdf tool"
                    DEFAULT OFF
                    REQUIRED_PACKAGES "NetCDF COMPONENTS CXX" )

ecbuild_add_option( FEATURE ODB_API_SERVER_SIDE
                    DESCRIPTION "Support for keyword SERVER_SIDE"
                    DEFAULT OFF )

ecbuild_add_option( FEATURE ODB_SERVER_TIME_FORMAT_FOUR_DIGITS
                    DESCRIPTION "ODB Server uses four digits format of time"
                    DEFAULT OFF )

if( HAVE_FORTRAN OR HAVE_MIGRATOR )

    ecbuild_enable_fortran( REQUIRED MODULE_DIRECTORY ${CMAKE_BINARY_DIR}/module )
    ecbuild_find_fortranlibs( REQUIRED )
    message( "Fortran libraries: [${FORTRAN_LIBRARIES}]" )

endif()

#if( HAVE_MIGRATOR )
    #if(CMAKE_Fortran_COMPILER_ID STREQUAL "PGI")
    #    ecbuild_add_fortran_flags("-r8")
    #    elseif(CMAKE_Fortran_COMPILER_ID STREQUAL "GNU")
    #    # NOTE that if we add -fdefault-real-8 then we NEED -fdefault-double-8 to avoid quadmath
    #    ecbuild_add_fortran_flags("-fdefault-real-8 -fdefault-double-8")
    #endif()
#endif()

if(CMAKE_Fortran_COMPILER_ID STREQUAL "GNU")
  ecbuild_add_fortran_flags("-fPIC -ffree-line-length-none")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  ecbuild_add_cxx_flags("-fPIC -Wl,--as-needed")
endif()

ecbuild_add_option( FEATURE PYTHON
                    DESCRIPTION "whether or not to build the Python interface"
                    DEFAULT OFF
                    REQUIRED_PACKAGES "Python VERSION 2.7 NO_LIBS" SWIG )

###############################################################################
# contents

set_directory_properties( PROPERTIES COMPILE_DEFINITIONS "${ECKIT_DEFINITIONS}" )

get_directory_property( ODB_API_DEFINITIONS COMPILE_DEFINITIONS )

set( ODB_API_INCLUDE_DIRS   ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_BINARY_DIR}/src )
set( ODB_API_LIBRARIES      Odb )

if( HAVE_FORTRAN )
  list( APPEND ODB_API_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/module )
  list( INSERT ODB_API_LIBRARIES 0 Odb_fortran )
endif()

add_subdirectory( src )
add_subdirectory( tests )

############################################################################################
# finalize

ecbuild_pkgconfig( NAME ${PROJECT_NAME}
                   DESCRIPTION "ECMWF coding, decoding and processing of observational data"
                   URL "https://software.ecmwf.int/wiki/display/ODB/ODB+API"
                   LIBRARIES Odb )

ecbuild_install_project( NAME OdbAPI )

ecbuild_print_summary()

