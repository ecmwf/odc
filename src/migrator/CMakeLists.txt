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

#IF(NOT CMAKE_Fortran_COMPILE_OBJECT)
#    SET(CMAKE_Fortran_COMPILE_OBJECT
#        "<CMAKE_Fortran_COMPILER> <DEFINES> <FLAGS> -c <SOURCE> -o <OBJECT>")
#ENDIF(NOT CMAKE_Fortran_COMPILE_OBJECT)

if( ODB_API_MIGRATOR AND ODB_FOUND AND DEFINED FORTRAN_LIBRARIES )

    ecbuild_enable_fortran( REQUIRED )

    ecbuild_add_library( TARGET Odbmigrator
                            COMPONENT  odb_api
                            SOURCES    ${migrator_srcs}
                            TEMPLATES
                            ImportODBTool.cc TSQLReader.cc
                            INCLUDES   ${ODB_INCLUDE_DIRS}
                            LIBS       Odb ${ODB_LIBRARIES} ${FORTRAN_LIBRARIES} ${DL_LIBRARIES} )
                        
    ecbuild_add_executable( TARGET     odb_migrator
                            SOURCES    odb2oda.cc
                            INCLUDES   ${ODB_INCLUDE_DIRS}
                            LIBS       Odbmigrator ) #${ODB_LIBRARIES} ${FORTRAN_LIBRARIES} ${DL_LIBRARIES} )

    ecbuild_add_executable( TARGET     pmigrator
                            SOURCES    pmigrator.f90 odb2.f90 odb2_flag_definitions.f90
                            INCLUDES   ${ODB_INCLUDE_DIRS}
                            LINKER_LANGUAGE Fortran
                            LIBS       Odb Odb_fortran /var/tmp/new_migrator/ECMA.conv/_odb_glue.o ${ODB_LIBRARIES} ${FORTRAN_LIBRARIES} ${DL_LIBRARIES} /var/tmp/new_migrator/ECMA.conv/libECMA.a  /usr/local/apps/odb/CY37R3.001/src/odb_CY37R3.001/odb/lib/savelist.o ) #${ODB_LIBRARIES} ${FORTRAN_LIBRARIES} ${DL_LIBRARIES} )

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

else()
   ecbuild_declare_project_files( ${migrator_srcs} )
endif()
