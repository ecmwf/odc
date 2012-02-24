mars_add_library( TARGET     Odb_fortran
                  COMPONENT  odb_api
                  SOURCES    odb_c_binding.f90
                  LIBS       Oda )

mars_add_test( TARGET     test_fortran_api
               SOURCES    test_fortran_api.f90
               LIBS       Odb_fortran )
