## check if all sources are used
ecbuild_find_project_files()

### config header

ecbuild_generate_config_headers( DESTINATION ${INSTALL_INCLUDE_DIR}/odc )

configure_file( odc_config.h.in odc_config.h )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/odc_config.h DESTINATION ${INSTALL_INCLUDE_DIR}/odc )

### libs

add_subdirectory( odc )
add_subdirectory( odc/tools )
add_subdirectory( odc/odb2netcdf )

### fortran bindings -- OFF for the moment due to errors with gfortran 6.3 / 7.3
add_subdirectory( fortran )
