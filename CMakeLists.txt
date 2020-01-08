cmake_minimum_required( VERSION 3.6 FATAL_ERROR )

project( odc C CXX )

set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../ecbuild/cmake")

include( ecbuild_system NO_POLICY_SCOPE )

ecbuild_requires_macro_version( 2.9 )

ecbuild_declare_project()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

########################################################################################################################
### dependencies and options

ecbuild_use_package( PROJECT eckit  VERSION 1.4 REQUIRED )

ecbuild_add_option( FEATURE FORTRAN
                    DESCRIPTION "whether or not to build the Fortran interface"
                    DEFAULT OFF )

#ecbuild_add_option( FEATURE NETCDF
#                    DESCRIPTION "whether or not to build the odb2netcdf tool"
#                    DEFAULT OFF
#                    REQUIRED_PACKAGES "NetCDF COMPONENTS CXX" )


if( HAVE_FORTRAN )

    set( Fortran Fortran )
    ecbuild_enable_fortran( REQUIRED MODULE_DIRECTORY ${PROJECT_BINARY_DIR}/module )

endif()

########################################################################################################################
# contents

set_directory_properties( PROPERTIES COMPILE_DEFINITIONS "${ECKIT_DEFINITIONS}" )

get_directory_property( ODC_DEFINITIONS COMPILE_DEFINITIONS )

set( ODC_INCLUDE_DIRS   ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_BINARY_DIR}/src )
set( ODC_LIBRARIES      odccore )

if( HAVE_FORTRAN )
  list( APPEND ODC_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/module )
  list( INSERT ODC_LIBRARIES 0 fodc odc_fortran ) # odc_fortran is deprecated
endif()

set( odc_INCLUDE_DIRS ${ODC_INCLUDE_DIRS} )
set( odc_LIBRARIES ${ODC_LIBRARIES} )

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
