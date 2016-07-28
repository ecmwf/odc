
# odbql, C/C++ API library.
list( APPEND odb_c_api_src_files
    odbql.h
    odbql.cc  
)

#ecbuild_add_library( TARGET             odbql
#                     INSTALL_HEADERS    LISTED
#                     HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/odb_api 
#                     SOURCES            ${odb_c_api_src_files} 
#                     LIBS               Odb 
#                     )

# Fortran bindings & wrappers.
#ecbuild_add_library(TARGET		odbql_fortran
#					CONDITION	HAVE_FORTRAN
#					SOURCES	    odbql_wrappers.f90 odbql_binding.f90
#					LIBS		Odb odbql )

# Examples / tests of C API.
#ecbuild_add_executable( TARGET    odbql_c_example
#                        SOURCES   example.c test.cc 
#                        LIBS      Odb odbql odbtools odbtest )


# Examples / tests of Fortran API.
#ecbuild_add_executable( TARGET   odbql_fortran_examples 
#                      CONDITION  HAVE_FORTRAN
#                      SOURCES    odbql_fortran_examples.f90
#                      LIBS       odbql_fortran
#                      )

#if( HAVE_FORTRAN )
#  install( FILES ${CMAKE_Fortran_MODULE_DIRECTORY}/${CMAKE_CFG_INTDIR}/odbql_binding.mod DESTINATION ${INSTALL_INCLUDE_DIR} )
#endif()
