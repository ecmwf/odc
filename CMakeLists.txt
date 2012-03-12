############################################################################################
# cmake
#   options:
#       -DCMAKE_BUILD_TYPE=Debug|RelWithDebInfo|Release|Production
#       -DCMAKE_INSTALL_PREFIX=/path/to/install
#
#       -DCMAKE_C_COMPILER=gcc
#       -DCMAKE_C_COMPILER=g++
#
#       -DCMAKE_PREFIX_PATH=/path/to/jasper:/path/to/any/package/out/of/place
#
#       -DGRIB_API_PATH=/path/to/grib
#       -DHPSS_PATH=/path/to/hpss
#       -DADSM_PATH=/path/to/adsm
#       -DREADLINE_PATH=/path/to/readline
#
#       -DWITH_READLINE=ON          [default: OFF]
#       -DWITH_DUMMY_TAPES=ON       [default: OFF]

cmake_minimum_required( VERSION 2.6.4 FATAL_ERROR )

project( odb_api C CXX )

message( STATUS "---------------------------------------------------------" )
message( STATUS " [${PROJECT_NAME}]" )

set( ECMWF_PROJECTS ${ECMWF_PROJECTS} ${PROJECT_NAME} CACHE INTERNAL "list of (sub)projects" )

###############################################################################
# include our cmake modules first because we override some cmake default macros
# but only do so if this is the top project
if( ${PROJECT_NAME} STREQUAL ${CMAKE_PROJECT_NAME} )

  set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH} )

  # add backward support from 2.8 to 2.6
  if( ${CMAKE_VERSION} VERSION_LESS "2.8" )
      set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/2.8" ${CMAKE_MODULE_PATH} )
  endif()

  include(ecmwf_include_macros)  # define our macros
  include(ecmwf_variables)       # define mars global variables
  include(CTest)                # add cmake testing support
  include(ecmwf_define_options)  # define build options
  include(ecmwf_check_os)        # check for os characteristics
  include(ecmwf_check_functions) # check for available functions
  include(ecmwf_find_packages)   # find packages we depend on

endif()

###############################################################################
# some variables/options of this project

option( BUILD_ODB_MIGRATOR "whether or not to build the migrator" OFF )
option( BUILD_ODB_FORTRAN  "whether or not to build the fortan interface" OFF )

###############################################################################
# macro processing

# define installation paths
include(ecmwf_define_paths)

find_package( SWIG )
if( SWIG_FOUND )
    include( ${SWIG_USE_FILE} )
endif()

############################################################################################
# contents

set( ODB_API_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_BINARY_DIR}/src PARENT_SCOPE )
set( ODB_API_LIBRARIES Odb PARENT_SCOPE )
set( ODB_API_FOUND TRUE PARENT_SCOPE )

# source files
add_subdirectory( src )

############################################################################################
# summary

# local project summary
include( project_summary.cmake )

# add cmake packing support
# include(ecmwf_install_package)

# print the summary of the configuration
include(ecmwf_print_summary)

# issue warnings / errors in case ther are unused project files
include(ecmwf_warn_unused_files)


