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

    ecbuild_add_library( TARGET Odbmigrator
                         SOURCES    ${migrator_srcs}
                         CONDITION
                            HAVE_MIGRATOR AND ODB_FOUND AND DEFINED FORTRAN_LIBRARIES
                         TEMPLATES
                            ImportODBTool.cc TSQLReader.cc
                         INCLUDES   ${ODB_INCLUDE_DIRS}
                         INSTALL_HEADERS LISTED
                         HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/odb_api/migrator
                         LIBS       Odb ${ODB_LIBRARIES} ${FORTRAN_LIBRARIES} ${DL_LIBRARIES} ${EXTRA_ODB_MIGRATOR_LIBS} ) # pass gomp in EXTRA_ODB_MIGRATOR_LIBS for gfortran if needed
                        
    ecbuild_add_executable( TARGET     odb_migrator
                        CONDITION
                            HAVE_MIGRATOR AND ODB_FOUND AND DEFINED FORTRAN_LIBRARIES
                        SOURCES    odb2oda.cc
                        INCLUDES   ${ODB_INCLUDE_DIRS}
                        # LINKER_LANGUAGE Fortran
                        LIBS       Odbmigrator odbtools ${ODB_LIBRARIES} ${FORTRAN_LIBRARIES} ${DL_LIBRARIES} )


  if( HAVE_MIGRATOR AND HAVE_PYTHON AND SWIG_FOUND AND PYTHONLIBS_FOUND )
        set(CMAKE_SWIG_FLAGS "")
        include_directories( ${CMAKE_CURRENT_BINARY_DIR} ${PYTHON_INCLUDE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../odb_api )
        # include_directories( ${CMAKE_CURRENT_BINARY_DIR} ${PYTHON_INCLUDE_PATH} )
        set_source_files_properties( pyodbdump.i PROPERTIES CPLUSPLUS ON )
        # set_source_files_properties( pyodbapi.i PROPERTIES SWIG_FLAGS "-includeall")
        swig_add_module( pyodbdump python pyodbdump.i )
        swig_link_libraries( pyodbdump Odbmigrator Odb ${ODB_LIBRARIES} ${FORTRAN_LIBRARIES} ${DL_LIBRARIES} ${PYTHON_LIBRARIES} )
        
        set(PYTHON_SITE "${INSTALL_LIB_DIR}/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages" )
        set(PYTHON_DEST "${PYTHON_SITE}" )

        install(TARGETS _pyodbdump                              DESTINATION ${PYTHON_DEST} )
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pyodbdump.py  DESTINATION ${PYTHON_DEST})
    endif()
