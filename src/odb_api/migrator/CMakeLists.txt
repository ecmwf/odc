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
ODBMigratorModule.h
ODBMigratorModule.cc
MigrateHandler.h
MigrateHandler.cc
migrator_api.cc
migrator_api.h
)

    ecbuild_add_library( TARGET Odbmigrator
                         SOURCES    ${migrator_srcs}
                         CONDITION
                            HAVE_MIGRATOR AND ODB_FOUND AND DEFINED FORTRAN_LIBRARIES
                         TEMPLATES
                            ImportODBTool.cc TSQLReader.cc
                         PRIVATE_INCLUDES   ${ODB_INCLUDE_DIRS}
                         INSTALL_HEADERS LISTED
                         HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/odb_api/migrator
                         LIBS       Odb odbtools ${ODB_LIBRARIES} ${FORTRAN_LIBRARIES} ${DL_LIBRARIES} ${EXTRA_ODB_MIGRATOR_LIBS}) #FIXME: add gomp for gfortran 
                        
    ecbuild_add_executable( TARGET     odb_migrator
                        CONDITION
                            HAVE_MIGRATOR AND ODB_FOUND AND DEFINED FORTRAN_LIBRARIES
                        SOURCES    odb2oda.cc
                        INCLUDES   ${ODB_INCLUDE_DIRS}
                        # LINKER_LANGUAGE Fortran
                        LIBS       Odbmigrator odbtools ${ODB_LIBRARIES} ${FORTRAN_LIBRARIES} ${DL_LIBRARIES} )

    set( test_environment
      ODB_API_CODES=${PROJECT_SOURCE_DIR}/etc
      ODB_API_HOME=${PROJECT_SOURCE_DIR}
      ODB_API_TEST_DATA_PATH=${CMAKE_CURRENT_BINARY_DIR}
      PATH=${CMAKE_BINARY_DIR}/bin:$ENV{PATH}
      ODB_RTABLE_PATH=${PROJECT_SOURCE_DIR}/etc )

    if( HAVE_MIGRATOR AND ODB_FOUND AND DEFINED FORTRAN_LIBRARIES )

      get_target_property( migrator_bin odb_migrator LOCATION)

      list( APPEND odb_migrator_data_files  2000010106.old.ECMA.tar.gz )

      ecbuild_get_test_multidata( TARGET get_odb_migrator_test_data
                                  DIRNAME odb_api/tests 
                                  NAMES ${odb_migrator_data_files} 
                                  NOCHECK )

      ecbuild_add_test( TARGET     test_migrator.ecml
                      COMMAND      ${migrator_bin} 
                      ARGS         ecml ${CMAKE_CURRENT_SOURCE_DIR}/test_migrator.ecml
                      CONDITION    HAVE_MIGRATOR
                      ENVIRONMENT  ${test_environment}
                      TEST_DEPENDS get_odb_migrator_test_data
                      )
    endif()

  if( HAVE_MIGRATOR AND HAVE_PYTHON AND SWIG_FOUND AND PYTHONLIBS_FOUND )
        set(CMAKE_SWIG_FLAGS "")
        include_directories( ${CMAKE_CURRENT_BINARY_DIR} ${PYTHON_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/../odb_api )
        set_source_files_properties( pyodbdump.i PROPERTIES CPLUSPLUS ON )
        # set_source_files_properties( pyodbapi.i PROPERTIES SWIG_FLAGS "-includeall")
        swig_add_module( pyodbdump python pyodbdump.i )
        swig_link_libraries( pyodbdump Odbmigrator Odb ${ODB_LIBRARIES} ${FORTRAN_LIBRARIES} ${DL_LIBRARIES} ${PYTHON_LIBRARIES} )
        
        set(PYTHON_SITE "${INSTALL_LIB_DIR}/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages" )
        set(PYTHON_DEST "${PYTHON_SITE}" )
        install(TARGETS _pyodbdump DESTINATION ${PYTHON_DEST} )
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pyodbdump.py  DESTINATION ${PYTHON_DEST})
    endif()
