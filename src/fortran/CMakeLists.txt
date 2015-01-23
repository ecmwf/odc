if( ENABLE_FORTRAN )
    ecbuild_enable_fortran( REQUIRED )
endif()
  
ecbuild_add_library(TARGET		Odb_fortran
					CONDITION	ENABLE_FORTRAN
					SOURCES		odb_c_binding.f90
					LIBS		Odb )

ecbuild_add_test( TARGET     test_fortran_api
                  CONDITION  ENABLE_FORTRAN
                  SOURCES    test_fortran_api.f90
                  LIBS       Odb_fortran ${ODB_LIBS} #${ODB_LIBPATH}
                  LINKER_LANGUAGE Fortran ) #${LINKER_LANGUAGE} )
