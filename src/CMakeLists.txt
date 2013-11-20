# TODO: eventually this should be removed
#       as ifdef's should be based on features not for OS name
add_definitions( -D${EC_OS_NAME} )

### check if all sources are used
ecbuild_find_project_files()

### config header

configure_file( odb_api_config.h.in odb_api_config.h )

install( FILES ${CMAKE_CURRENT_BINARY_DIR}/odb_api_config.h DESTINATION ${INSTALL_INCLUDE_DIR} )

### include directories

include_directories( ${ODB_API_INCLUDE_DIRS} ${ECKIT_INCLUDE_DIRS} )

### libs

add_subdirectory( odblib  )

add_subdirectory( fortran )

### apps

add_subdirectory( odb )

add_subdirectory( migrator )
