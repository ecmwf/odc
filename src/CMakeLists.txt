### check if all sources are used
ecbuild_find_project_files()

### config header

ecbuild_generate_config_headers( DESTINATION ${INSTALL_INCLUDE_DIR}/odb_api )

configure_file( odb_api_config.h.in odb_api_config.h )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/odb_api_config.h
		DESTINATION ${INSTALL_INCLUDE_DIR}/odb_api )

### include directories

include_directories( ${ODB_API_INCLUDE_DIRS} ${ECKIT_INCLUDE_DIRS} )

### libs

add_subdirectory( odb_api )

add_subdirectory( fortran )

add_subdirectory( tests )


