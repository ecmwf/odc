# Config file for the ecbuild package
# Defines the following variables:
#
#  ECBUILD_INCLUDE_DIRS - include directories
#  ECBUILD_DEFINITIONS  - preprocessor definitions
#  ECBUILD_LIBRARIES    - libraries to link against
#
# Also defines ecbuild third-party library dependencies:
#  ECBUILD_TPLS             - package names of  third-party library dependencies
#  ECBUILD_TPL_INCLUDE_DIRS - include directories
#  ECBUILD_TPL_DEFINITIONS  - preprocessor definitions
#  ECBUILD_TPL_LIBRARIES    - libraries to link against

# compute paths

get_filename_component(ECBUILD_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

set( ECBUILD_SELF_INCLUDE_DIRS "${ECBUILD_CMAKE_DIR}/../../../include" )
set( ECBUILD_SELF_DEFINITIONS  "" )
set( ECBUILD_SELF_LIBRARIES    "" )

set( ECBUILD_TPLS             "" )

set( ECBUILD_TPL_INCLUDE_DIRS "" )
set( ECBUILD_TPL_DEFINITIONS  "" )
set( ECBUILD_TPL_LIBRARIES    "" )

# include paths as absolute paths
set( ECBUILD_INCLUDE_DIRS "" )
foreach( path ${ECBUILD_SELF_INCLUDE_DIRS} )
	get_filename_component( abspath ${path} ABSOLUTE )
	list( APPEND ECBUILD_INCLUDE_DIRS ${abspath} )
endforeach()
list( APPEND ECBUILD_INCLUDE_DIRS ${ECBUILD_TPL_INCLUDE_DIRS} )

set( ECBUILD_DEFINITIONS      ${ECBUILD_SELF_DEFINITIONS} ${ECBUILD_TPL_DEFINITIONS} )
set( ECBUILD_LIBRARIES        ${ECBUILD_SELF_LIBRARIES}  ${ECBUILD_TPL_LIBRARIES} )


# insert definitions for IMPORTED targets

if( NOT ecbuild_BINARY_DIR )

	set( IS_BUILD_DIR_EXPORT OFF )

	if( IS_BUILD_DIR_EXPORT )
		include( "/vol/marsdev/data/build/linux/x86_64/opensuse131/builds/ecbuild/ecbuild-targets.cmake" OPTIONAL )
	else()
		include( "${ECBUILD_CMAKE_DIR}/ecbuild-targets.cmake" )
	endif()

endif()

# here goes the imports of the TPL's

include( ${CMAKE_CURRENT_LIST_FILE}.tpls OPTIONAL )

# publish this file as imported

set( ECBUILD_IMPORT_FILE ${CMAKE_CURRENT_LIST_FILE} )
mark_as_advanced( ECBUILD_IMPORT_FILE )

