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
#       -DCMAKE_PREFIX_PATH=/path/to/jasper:/path/to/any/package/out/of/place
#       -DBUILD_SHARED_LIBS=ON

cmake_minimum_required( VERSION 2.6.4 FATAL_ERROR )

project( odb_api )

set( EC_FORTRAN "OPTIONAL" )

set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH} )

include( ecbuild_system )

###############################################################################
# local project

ecbuild_use_package( PROJECT eclib NAME EcLib REQUIRED )

ecbuild_declare_project()

ecbuild_add_resources( TARGET ${PROJECT_NAME}_no_odb DONT_PACK_REGEX "*.odb" )
ecbuild_add_resources( TARGET ${PROJECT_NAME}_no_oda DONT_PACK_REGEX "*.oda" )

###############################################################################
# some variables/options of this project

option( ODB_API_MIGRATOR "whether or not to build the migrator"         OFF )
option( ODB_API_FORTRAN  "whether or not to build the fortan interface" OFF )
option( ODB_API_PYTHON   "whether or not to build the python interface" OFF )

###############################################################################
# find extra packages

find_package( SWIG )
if( SWIG_FOUND )
    include( ${SWIG_USE_FILE} )
endif()

if( WITH_ODB OR DEFINED ODB_PATH OR ODB_API_MIGRATOR ) # old version of odb
    find_package(ODB) 
endif()

###############################################################################
# contents

if( NOT ${PROJECT_NAME} STREQUAL ${CMAKE_PROJECT_NAME} )
    set( ODB_API_INCLUDE_DIRS   ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_BINARY_DIR}/src PARENT_SCOPE )
    set( ODB_API_LIBRARIES      Odb PARENT_SCOPE )
    set( ODB_API_FOUND          TRUE PARENT_SCOPE )
endif()

# source files
add_subdirectory( src )

############################################################################################
# finalize

ecbuild_install_project( NAME OdbAPI )

ecbuild_print_summary()

