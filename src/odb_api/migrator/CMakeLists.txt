list( APPEND migrator_srcs
FakeODBIterator.cc
FakeODBIterator.h
ImportODBTool.cc
ImportODBTool.h
ODB2ODATool.cc
ODB2ODATool.h
MigratorTool.h
MigratorTool.cc
ODBIterator.cc
ODBIterator.h
OldODBReader.h
ReptypeGenIterator.cc
ReptypeGenIterator.h
TSQLReader.cc
TSQLReader.h
migrator_api.cc
migrator_api.h
)

if( ENABLE_MIGRATOR )
    enable_language (Fortran)
    ecbuild_enable_fortran( REQUIRED )
    ecbuild_find_fortranlibs( REQUIRED )
    #if(CMAKE_Fortran_COMPILER_ID STREQUAL "PGI")
    #    cmake_add_fortran_flags("-r8")
    #    elseif(CMAKE_Fortran_COMPILER_ID STREQUAL "GNU")
    #    # NOTE that if we add -fdefault-real-8 then we NEED -fdefault-double-8 to avoid quadmath
    #    cmake_add_fortran_flags("-fdefault-real-8 -fdefault-double-8")
    #endif()
	message( "Fortran libraries: [${FORTRAN_LIBRARIES}]" )
endif()

    ecbuild_add_library( TARGET Odbmigrator
                         SOURCES    ${migrator_srcs}
  		             	 CONDITION
				            ENABLE_MIGRATOR AND ODB_FOUND AND DEFINED FORTRAN_LIBRARIES
                         TEMPLATES
                            ImportODBTool.cc TSQLReader.cc
                         INCLUDES   ${ODB_INCLUDE_DIRS}
                         INSTALL_HEADERS LISTED
                         HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/odb_api/migrator
                         LIBS       Odb ${ODB_LIBRARIES} ${FORTRAN_LIBRARIES} ${DL_LIBRARIES} ) #FIXME: add gomp for gfortran 
                        
    ecbuild_add_executable( TARGET     odb_migrator
                        CONDITION
                            ENABLE_MIGRATOR AND ODB_FOUND AND DEFINED FORTRAN_LIBRARIES
                        SOURCES    odb2oda.cc
                        INCLUDES   ${ODB_INCLUDE_DIRS}
                        LINKER_LANGUAGE ${LINKER_LANGUAGE}
                        LIBS       Odbmigrator odbtools ${ODB_LIBS} ) #${ODB_LIBRARIES} ${FORTRAN_LIBRARIES} ${DL_LIBRARIES} )


	if( ENABLE_MIGRATOR AND ENABLE_PYTHON AND SWIG_FOUND AND PYTHONLIBS_FOUND )
        set(CMAKE_SWIG_FLAGS "")
        include_directories( ${CMAKE_CURRENT_BINARY_DIR} ${PYTHON_INCLUDE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../odb_api )
        #include_directories( ${CMAKE_CURRENT_BINARY_DIR} ${PYTHON_INCLUDE_PATH} )
        set_source_files_properties( pyodbdump.i PROPERTIES CPLUSPLUS ON )
#    set_source_files_properties( pyodbapi.i PROPERTIES SWIG_FLAGS "-includeall")
        swig_add_module( pyodbdump python pyodbdump.i )
        swig_link_libraries( pyodbdump Odbmigrator Odb ${ODB_LIBRARIES} ${FORTRAN_LIBRARIES} ${DL_LIBRARIES} ${PYTHON_LIBRARIES} )
        set(PYTHON_DEST "lib/python${PYTHON_VERSION}/site-packages" )
        install(TARGETS _pyodbdump DESTINATION ${PYTHON_DEST} )
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pyodbdump.py  DESTINATION ${PYTHON_DEST})
    endif()
