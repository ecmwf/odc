ecbuild_add_library(TARGET		Odb_fortran
					CONDITION	HAVE_FORTRAN
					SOURCES		odb_c_binding.f90
					LIBS		Odb )

list( APPEND fortran_tests
fortran_api_examples
test_fortran_api_open_non_existing_file
test_client_lib_fortran_local
)

foreach( _test ${fortran_tests} )
    ecbuild_add_test( TARGET     ${_test}
                      CONDITION  HAVE_FORTRAN
                      SOURCES    ${_test}.f90
                      LIBS       Odb_fortran ${ODB_LIBS} 
                      ENVIRONMENT 
                                TEST_DHSHOME=${PROJECT_SOURCE_DIR}/tests/dhshome/
                      LINKER_LANGUAGE Fortran )
endforeach()

if( HAVE_FORTRAN )
  install( FILES ${CMAKE_Fortran_MODULE_DIRECTORY}/${CMAKE_CFG_INTDIR}/odb_c_binding.mod DESTINATION ${INSTALL_INCLUDE_DIR} )
endif()
