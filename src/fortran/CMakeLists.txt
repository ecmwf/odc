ecbuild_add_library(TARGET		Odb_fortran
					CONDITION	HAVE_FORTRAN
					SOURCES		odb_c_binding.f90
					LIBS		Odb )

ecbuild_add_test( TARGET     test_fortran_api
                  CONDITION  HAVE_FORTRAN
                  SOURCES    test_fortran_api.f90
                  LIBS       Odb_fortran ${ODB_LIBS} #${ODB_LIBPATH}
                  LINKER_LANGUAGE Fortran )

ecbuild_add_test( TARGET     test_client_lib_fortran_local.f90
                  CONDITION  HAVE_FORTRAN
                  SOURCES    test_client_lib_fortran_local.f90
                  LIBS       Odb_fortran ${ODB_LIBS} #${ODB_LIBPATH}
                  LINKER_LANGUAGE Fortran )


if( HAVE_FORTRAN )
  install( FILES ${CMAKE_Fortran_MODULE_DIRECTORY}/${CMAKE_CFG_INTDIR}/odb_c_binding.mod DESTINATION ${INSTALL_INCLUDE_DIR} )
endif()
