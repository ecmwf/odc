list( APPEND migrator_srcs
ODB2ODATool.cc
ODBIterator.cc
FakeODBIterator.cc
ReptypeGenIterator.cc
ksh.cc
TestImportODB.cc
ImportODBTool.cc
)

if( ODB_API_MIGRATOR AND ODB_FOUND AND DEFINED FORTRAN_LIBRARIES )

  ecmwf_add_executable( TARGET     odb_migrator
                        COMPONENT odb_api
                        SOURCES    ${migrator_srcs}
                        TEMPLATES  
                          ImportODBTool.cc
                        INCLUDES   ${ODB_INCLUDE_DIRS}
                        LIBS       Odb ${ODB_LIBRARIES} ${FORTRAN_LIBRARIES} )

else()
   ecmwf_declare_project_files( ${migrator_srcs} )
endif()
