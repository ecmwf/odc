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
    ecbuild_find_fortranlibs( REQUIRED )
endif()

    debug_var( ODB_FOUND )
    debug_var( FORTRAN_LIBRARIES)

    ecbuild_add_library(  TARGET Odbmigrator
                        COMPONENT  odb_api
                        SOURCES    ${migrator_srcs}
  		             	CONDITION
				            ENABLE_MIGRATOR AND ODB_FOUND AND DEFINED FORTRAN_LIBRARIES
                        TEMPLATES
                            ImportODBTool.cc TSQLReader.cc
                            INCLUDES   ${ODB_INCLUDE_DIRS}
                            LIBS       Odb ${ODB_LIBRARIES} ${FORTRAN_LIBRARIES} ${DL_LIBRARIES} )
                        
    ecbuild_add_executable( TARGET     odb_migrator
                        CONDITION
                            ENABLE_MIGRATOR AND ODB_FOUND AND DEFINED FORTRAN_LIBRARIES
                        SOURCES    odb2oda.cc
                        INCLUDES   ${ODB_INCLUDE_DIRS}
                        LIBS       Odbmigrator odbtools ) #${ODB_LIBRARIES} ${FORTRAN_LIBRARIES} ${DL_LIBRARIES} )

    ecbuild_add_executable( TARGET     pmigrator
                            CONDITION
                                ENABLE_PMIGRATOR
                            SOURCES    pmigrator.f90 odb2.f90 odb2_flag_definitions.f90
                            INCLUDES   ${ODB_INCLUDE_DIRS}
                            LINKER_LANGUAGE Fortran
                            LIBS       Odb Odb_fortran /var/tmp/new_migrator/ECMA.conv/_odb_glue.o ${ODB_LIBRARIES} ${FORTRAN_LIBRARIES} ${DL_LIBRARIES} /var/tmp/new_migrator/ECMA.conv/libECMA.a  /usr/local/apps/odb/CY37R3.001/src/odb_CY37R3.001/odb/lib/savelist.o ) #${ODB_LIBRARIES} ${FORTRAN_LIBRARIES} ${DL_LIBRARIES} )

	if( ENABLE_MIGRATOR AND ENABLE_PYTHON AND SWIG_FOUND AND PYTHONLIBS_FOUND )
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
