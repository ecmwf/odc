if( ODB_API_FORTRAN )
    ecbuild_enable_fortran( REQUIRED )
endif()
  
ecbuild_add_library( TARGET     Odb_fortran
                     COMPONENT  odb_api
					 CONDITION  ODB_API_FORTRAN
                     SOURCES    odb_c_binding.f90
                     LIBS       Odb )

ecbuild_add_test( TARGET     test_fortran_api
                  CONDITION  ODB_API_FORTRAN
                  SOURCES    test_fortran_api.f90
                  LIBS       Odb_fortran 
                  LINKER_LANGUAGE ${LINKER_LANGUAGE} )
