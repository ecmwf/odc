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
TestImportODB.cc
TestImportODB.h
migrator_api.cc
migrator_api.h
)

debug_var( ODB_API_MIGRATOR )
debug_var( ODB_FOUND )
debug_var( FORTRAN_LIBRARIES)

if( ODB_API_MIGRATOR )
	ecbuild_find_fortranlibs( REQUIRED )
endif()

  ecbuild_add_library ( TARGET Odbmigrator
                        COMPONENT  odb_api
                        SOURCES    ${migrator_srcs}
  		             	CONDITION
				          ODB_API_MIGRATOR AND ODB_FOUND AND DEFINED FORTRAN_LIBRARIES
                        TEMPLATES
                            ImportODBTool.cc TSQLReader.cc
                        INCLUDES   ${ODB_INCLUDE_DIRS}
                        LIBS       Odb ${ODB_LIBRARIES} ${FORTRAN_LIBRARIES} ${DL_LIBRARIES} )
                        

  ecbuild_add_executable( TARGET     odb_migrator
						CONDITION
							ODB_API_MIGRATOR AND ODB_FOUND AND DEFINED FORTRAN_LIBRARIES
                        SOURCES    odb2oda.cc
                        INCLUDES   ${ODB_INCLUDE_DIRS}
                        LIBS       Odbmigrator ) #${ODB_LIBRARIES} ${FORTRAN_LIBRARIES} ${DL_LIBRARIES} )

    if( ODB_API_PYTHON AND SWIG_FOUND AND PYTHONLIBS_FOUND )
        set(CMAKE_SWIG_FLAGS "")
        include_directories( "." ${PYTHON_INCLUDE_PATH} )
        set_source_files_properties( pyodbdump.i PROPERTIES CPLUSPLUS ON )
#    set_source_files_properties( pyodbapi.i PROPERTIES SWIG_FLAGS "-includeall")
        swig_add_module( pyodbdump python pyodbdump.i )
        swig_link_libraries( pyodbdump Odbmigrator Odb ${ODB_LIBRARIES} ${FORTRAN_LIBRARIES} ${DL_LIBRARIES} ${PYTHON_LIBRARIES} )
        set(PYTHON_DEST "lib/python${PYTHON_VERSION}/site-packages" )
        install(TARGETS _pyodbdump DESTINATION ${PYTHON_DEST} )
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pyodbdump.py  DESTINATION ${PYTHON_DEST})
    endif()