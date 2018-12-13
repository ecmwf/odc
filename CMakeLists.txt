cmake_minimum_required( VERSION 3.1 FATAL_ERROR )

project( odc C CXX )

set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../ecbuild/cmake")

include( ecbuild_system NO_POLICY_SCOPE )

ecbuild_requires_macro_version( 2.9 )

ecbuild_declare_project()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

########################################################################################################################
### dependencies and options

ecbuild_use_package( PROJECT eckit  VERSION 0.21 REQUIRED )
ecbuild_use_package( PROJECT metkit VERSION 0.11 REQUIRED )

ecbuild_add_resources( TARGET ${PROJECT_NAME}_no_odb DONT_PACK_REGEX "*.odb" )
ecbuild_add_resources( TARGET ${PROJECT_NAME}_no_oda DONT_PACK_REGEX "*.oda" )

ecbuild_dont_pack(REGEX "*.odb")
ecbuild_dont_pack(REGEX ".gdbinit")

###############################################################################
# some variables/options of this project

ecbuild_add_option( FEATURE FORTRAN
                    DESCRIPTION "whether or not to build the Fortran interface"
                    DEFAULT OFF )

#ecbuild_add_option( FEATURE NETCDF
#                    DESCRIPTION "whether or not to build the odb2netcdf tool"
#                    DEFAULT OFF
#                    REQUIRED_PACKAGES "NetCDF COMPONENTS CXX" )

ecbuild_add_option( FEATURE ODB_SERVER_TIME_FORMAT_FOUR_DIGITS
                    DESCRIPTION "ODB Server uses four digits format of time"
                    DEFAULT OFF )


SET(odc_SCHEMA_PATH "" CACHE STRING "Path to schema file that will be loaded before executing SQL")

if( HAVE_FORTRAN )

    set( Fortran Fortran )
    ecbuild_enable_fortran( REQUIRED MODULE_DIRECTORY ${CMAKE_BINARY_DIR}/module )

endif()


ecbuild_add_option( FEATURE PYTHON
                    DESCRIPTION "whether or not to build the Python interface"
                    DEFAULT OFF
                    REQUIRED_PACKAGES "Python VERSION 2.7" SWIG )

###############################################################################
# contents

set_directory_properties( PROPERTIES COMPILE_DEFINITIONS "${ECKIT_DEFINITIONS}" )

get_directory_property( odc_DEFINITIONS COMPILE_DEFINITIONS )

set( odc_INCLUDE_DIRS   ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_BINARY_DIR}/src )
set( odc_LIBRARIES      Odb )

if( HAVE_FORTRAN )
  list( APPEND odc_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/module )
  list( INSERT odc_LIBRARIES 0 Odb_fortran )
endif()

add_subdirectory( src )
add_subdirectory( regressions )
add_subdirectory( tests )

############################################################################################
# finalize

ecbuild_pkgconfig( NAME ${PROJECT_NAME}
                   DESCRIPTION "ECMWF coding, decoding and processing of observational data"
                   URL "https://software.ecmwf.int/wiki/display/ODB/ODB+API"
                   LIBRARIES Odb )

ecbuild_install_project( NAME OdbAPI )

ecbuild_print_summary()

