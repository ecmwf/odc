
ecbuild_add_library(TARGET		odc_fortran

                    CONDITION   HAVE_FORTRAN

                    SOURCES
                                # Legacy:

                                odc_c_binding.f90

                    LIBS		odccore )

set( test_environment_fortran
     TEST_DHSHOME=${PROJECT_SOURCE_DIR}/tests/dhshome/
     odc_SCHEMA_PATH=${CMAKE_CURRENT_BINARY_DIR}/../../tests/cma.hh
)

if( HAVE_FORTRAN )

    list( APPEND fortran_tests

        # Legacy API examples
        legacy_fortran_api_examples
        legacy_test_fortran_api_open_non_existing_file
    )

    foreach( _test ${fortran_tests} )
        ecbuild_add_test( TARGET     odc_${_test}
                          CONDITION  HAVE_FORTRAN
                          SOURCES    ${_test}.f90
                          LIBS       odc_fortran ${ODB_LIBS}
                          ENVIRONMENT  ${test_environment_fortran}

                          LABELS     odc odc_fortran

                          TEST_DEPENDS odc_get_test_data
                                       odc_get_mars_client_test_data_mo
                                       odc_get_mars_client_test_data_ec

                          LINKER_LANGUAGE Fortran )
    endforeach()

# Legacy Fortran bindings:
    install( FILES ${CMAKE_Fortran_MODULE_DIRECTORY}/${CMAKE_CFG_INTDIR}/odc_c_binding.mod DESTINATION ${INSTALL_INCLUDE_DIR} )
endif()
