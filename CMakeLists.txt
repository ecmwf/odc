###############################################################################
# cmake
#   options:
#       -DCMAKE_BUILD_TYPE=Debug|RelWithDebInfo|Release|Production
#       -DCMAKE_INSTALL_PREFIX=/path/to/install
#
#       -DCMAKE_C_COMPILER=gcc
#       -DCMAKE_C_COMPILER=g++
#
#       -DCMAKE_PREFIX_PATH=/path/to/jasper:/path/to/any/package/out/of/place

cmake_minimum_required( VERSION 2.6.4 FATAL_ERROR )

project( odb_api C CXX )

###############################################################################
# include our cmake macros, but only do so if this is the top project
if( ${PROJECT_NAME} STREQUAL ${CMAKE_PROJECT_NAME} )
    
    set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH} )

    # add backward support from 2.8 to 2.6
    if( ${CMAKE_VERSION} VERSION_LESS "2.8" )
        set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/2.8" ${CMAKE_MODULE_PATH} )
    endif()

    include(CTest)                 # add cmake testing support
    enable_testing()
    
    include(ecmwf_include_macros)  # define our macros
    include(ecmwf_define_options)  # define build options
    include(ecmwf_check_os)        # check for os characteristics
    include(ecmwf_check_functions) # check for available functions
    include(ecmwf_find_packages)   # find packages we depend on

endif()

###############################################################################
# local project

ecmwf_declare_project()

###############################################################################
# some variables/options of this project

option( ODB_API_MIGRATOR "whether or not to build the migrator"         OFF )
option( ODB_API_FORTRAN  "whether or not to build the fortan interface" OFF )
option( ODB_API_PYTHON   "whether or not to build the python interface" OFF )

###############################################################################
# macro processing

# define installation paths
include(ecmwf_define_paths)

find_package( SWIG )
if( SWIG_FOUND )
    include( ${SWIG_USE_FILE} )
endif()

if( WITH_ODB OR DEFINED ODB_PATH ) # old version of odb
    find_package(ODB) 
endif()

###############################################################################
# contents

set( ODB_API_INCLUDE_DIRS   ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_BINARY_DIR}/src PARENT_SCOPE )
set( ODB_API_LIBRARIES      Odb PARENT_SCOPE )
set( ODB_API_FOUND          TRUE PARENT_SCOPE )

# source files
add_subdirectory( src )

###############################################################################
# summary

# local project summary
include( project_summary.cmake )

# add cmake packing support
# include(ecmwf_install_package)

# print the summary of the configuration
include(ecmwf_print_summary)

# issue warnings / errors in case ther are unused project files
include(ecmwf_warn_unused_files)


