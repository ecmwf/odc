if( ODB_API_FORTRAN )

    set(CMAKE_Fortran_MODULE_DIRECTORY
        ${PROJECT_BINARY_DIR}/include CACHE PATH "directory for all fortran modules."
    )

    install( CODE
        "EXECUTE_PROCESS (COMMAND \"${CMAKE_COMMAND}\" -E copy_directory \"${CMAKE_Fortran_MODULE_DIRECTORY}/\${BUILD_TYPE}\" \"${CMAKE_INSTALL_PREFIX}/include\")"
    ) 

  ecbuild_add_library(TARGET     Odb_fortran
                    COMPONENT  odb_api
                    SOURCES    odb_c_binding.f90
                    LIBS       Odb )

  ecbuild_add_fortran_test(TARGET     test_fortran_api
                 SOURCES    test_fortran_api.f90
                 LIBS       Odb_fortran )
else()
  ecbuild_declare_project_files( odb_c_binding.f90 test_fortran_api.f90 )
endif()
