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

cmake_minimum_required( VERSION 2.8.4 FATAL_ERROR )

project( odb_api CXX )

# Add path for custom modules
set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../ecbuild/cmake")

include( ecbuild_system )

ecbuild_requires_macro_version( 1.7 )

###############################################################################
# local project

ecbuild_use_package( PROJECT eckit VERSION 0.6 REQUIRED )
ecbuild_use_package( PROJECT odb   VERSION 1.0 QUIET )

ecbuild_declare_project()

ecbuild_add_resources( TARGET ${PROJECT_NAME}_no_odb DONT_PACK_REGEX "*.odb" )
ecbuild_add_resources( TARGET ${PROJECT_NAME}_no_oda DONT_PACK_REGEX "*.oda" )

###############################################################################
# some variables/options of this project

ecbuild_add_option( FEATURE MIGRATOR
                    DESCRIPTION "whether or not to build the migrator"
                    DEFAULT OFF
                    REQUIRED_PACKAGES "ODB" )

ecbuild_add_option( FEATURE FORTRAN
                    DESCRIPTION "whether or not to build the Fortran interface"
                    DEFAULT OFF )

if( HAVE_FORTRAN OR HAVE_MIGRATOR )

  ecbuild_enable_fortran( REQUIRED MODULE_DIRECTORY ${CMAKE_BINARY_DIR}/module )

endif()

if( HAVE_MIGRATOR )

    ecbuild_find_fortranlibs( REQUIRED )
    #if(CMAKE_Fortran_COMPILER_ID STREQUAL "PGI")
    #    cmake_add_fortran_flags("-r8")
    #    elseif(CMAKE_Fortran_COMPILER_ID STREQUAL "GNU")
    #    # NOTE that if we add -fdefault-real-8 then we NEED -fdefault-double-8 to avoid quadmath
    #    cmake_add_fortran_flags("-fdefault-real-8 -fdefault-double-8")
    #endif()
  message( "Fortran libraries: [${FORTRAN_LIBRARIES}]" )

endif()


ecbuild_add_option( FEATURE PYTHON
                    DESCRIPTION "whether or not to build the Python interface"
                    DEFAULT OFF )

###############################################################################
# find extra packages

ecbuild_find_python( VERSION 2.7 )

find_package( SWIG )
if( SWIG_FOUND )
    include( ${SWIG_USE_FILE} )
endif()

###############################################################################
# contents

set_directory_properties( PROPERTIES COMPILE_DEFINITIONS "${ECKIT_DEFINITIONS}" )

get_directory_property( ODB_API_DEFINITIONS COMPILE_DEFINITIONS )

set( ODB_API_INCLUDE_DIRS   ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_BINARY_DIR}/src )
set( ODB_API_LIBRARIES      Odb )
set( ODB_API_TPLS           eckit )

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

