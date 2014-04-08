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

set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../ecbuild/cmake")

include( ecbuild_system )

ecbuild_requires_macro_version( 1.3 )

###############################################################################
# local project

ecbuild_use_package( PROJECT eckit VERSION 0.6 REQUIRED )

ecbuild_declare_project()

ecbuild_add_resources( TARGET ${PROJECT_NAME}_no_odb DONT_PACK_REGEX "*.odb" )
ecbuild_add_resources( TARGET ${PROJECT_NAME}_no_oda DONT_PACK_REGEX "*.oda" )

###############################################################################
# some variables/options of this project

option( ENABLE_MIGRATOR "whether or not to build the migrator"           OFF )
option( ENABLE_PMIGRATOR "whether or not to build the parallel migrator" OFF )
option( ENABLE_FORTRAN  "whether or not to build the Fortran interface"  OFF )
option( ENABLE_PYTHON   "whether or not to build the Python interface"   OFF )

###############################################################################
# find extra packages

ecbuild_find_python( VERSION 2.7 )

find_package( SWIG )
if( SWIG_FOUND )
    include( ${SWIG_USE_FILE} )
endif()

if( WITH_ODB OR DEFINED ODB_PATH OR ENABLE_MIGRATOR ) # old version of odb
    find_package(ODB) 
endif()

###############################################################################
# contents

set_directory_properties( PROPERTIES COMPILE_DEFINITIONS "${ECKIT_DEFINITIONS}" )

get_directory_property( ODB_API_DEFINITIONS COMPILE_DEFINITIONS )

set( ODB_API_INCLUDE_DIRS   ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_BINARY_DIR}/src )
set( ODB_API_LIBRARIES      Odb )
set( ODB_API_TPLS           eckit )

# source files
add_subdirectory( src )

############################################################################################
# finalize

ecbuild_install_project( NAME OdbAPI )

ecbuild_print_summary()

