if( BUILD_ODB_FORTRAN )

  ecmwf_add_library(TARGET     Odb_fortran
                    COMPONENT  odb_api
                    SOURCES    odb_c_binding.f90
                    LIBS       Odb )

  ecmwf_add_test(TARGET     test_fortran_api
                 SOURCES    test_fortran_api.f90
                 LIBS       Odb_fortran )
else()
  ecmwf_declare_project_files( odb_c_binding.f90 test_fortran_api.f90 )
endif()
