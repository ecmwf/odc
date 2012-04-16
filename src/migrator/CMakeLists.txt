list( APPEND migrator_srcs
FakeODBIterator.cc
FakeODBIterator.h
ImportODBTool.cc
ImportODBTool.h
ODB2ODATool.cc
ODB2ODATool.h
ODBIterator.cc
ODBIterator.h
OldODBReader.h
ReptypeGenIterator.cc
ReptypeGenIterator.h
TSQLReader.cc
TSQLReader.h
TestImportODB.cc
TestImportODB.h
odb2oda.cc
)

debug_var( ODB_API_MIGRATOR )
debug_var( ODB_FOUND )
debug_var( FORTRAN_LIBRARIES)

if( ODB_API_MIGRATOR AND ODB_FOUND AND DEFINED FORTRAN_LIBRARIES )

  ecmwf_add_executable( TARGET     odb_migrator
                        COMPONENT odb_api
                        SOURCES    ${migrator_srcs}
                        TEMPLATES  
                          ImportODBTool.cc TSQLReader.cc
                        INCLUDES   ${ODB_INCLUDE_DIRS}
                        LIBS       Odb ${ODB_LIBRARIES} ${FORTRAN_LIBRARIES} ${DL_LIBRARIES} )

else()
   ecmwf_declare_project_files( ${migrator_srcs} )
endif()
