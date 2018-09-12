
ecbuild_add_library(TARGET		Odb_fortran

                    CONDITION   HAVE_FORTRAN

                    SOURCES		
                                # Legacy:

                                odb_c_binding.f90

                    LIBS		Odb )

set( test_environment_fortran
     TEST_DHSHOME=${PROJECT_SOURCE_DIR}/tests/dhshome/
     ODB_API_SCHEMA_PATH=${CMAKE_CURRENT_BINARY_DIR}/../../tests/cma.hh
)

if( HAVE_FORTRAN )

    list( APPEND fortran_tests

        # Legacy API examples
        legacy_fortran_api_examples
        legacy_test_fortran_api_open_non_existing_file
    )

    foreach( _test ${fortran_tests} )
        ecbuild_add_test( TARGET     ${_test}
                          CONDITION  HAVE_FORTRAN
                          SOURCES    ${_test}.f90
                          LIBS       Odb_fortran ${ODB_LIBS} 
                          ENVIRONMENT  ${test_environment_fortran} 

                          LABELS     odb_api odb_api_fortran

                          TEST_DEPENDS get_odb_api_test_data
                                       get_mars_client_test_data_mo
                                       get_mars_client_test_data_ec
                                       test_ec_archiving.ecml
                                       test_mo_archiving.ecml

                          LINKER_LANGUAGE Fortran )
    endforeach()

# odbql, the new Fortran API:
    install( FILES ${CMAKE_Fortran_MODULE_DIRECTORY}/${CMAKE_CFG_INTDIR}/odbql_binding.mod  DESTINATION ${INSTALL_INCLUDE_DIR} )
    install( FILES ${CMAKE_Fortran_MODULE_DIRECTORY}/${CMAKE_CFG_INTDIR}/odbql_wrappers.mod DESTINATION ${INSTALL_INCLUDE_DIR} )
    install( FILES ${CMAKE_Fortran_MODULE_DIRECTORY}/${CMAKE_CFG_INTDIR}/odbql_constants.mod DESTINATION ${INSTALL_INCLUDE_DIR} )

# Legacy Fortran bindings:
    install( FILES ${CMAKE_Fortran_MODULE_DIRECTORY}/${CMAKE_CFG_INTDIR}/odb_c_binding.mod DESTINATION ${INSTALL_INCLUDE_DIR} )
endif()
