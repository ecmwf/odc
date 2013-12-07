list( APPEND odb_src_files
obsdump.h
odb.cc
)

ecbuild_add_executable( TARGET    odb
                      COMPONENT odb_api
                      SOURCES   ${odb_src_files}
                      LIBS      Odb ${ECKIT_LIBRARIES} )
