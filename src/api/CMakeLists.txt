
# Examples / tests of C API.
ecbuild_add_executable( TARGET    odbql_c_example
                        SOURCES   odbql_c_example.c odbql_c_test.c
                        LIBS      Odb 
                        LINKER_LANGUAGE CXX ) # ODB-325


# Examples / tests of Fortran API.
ecbuild_add_executable( TARGET   odbql_fortran_example 

                        CONDITION  HAVE_FORTRAN

                        SOURCES    odbql_fortran_example.f90
                        LIBS       Odb_fortran )

