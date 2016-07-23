
list( APPEND odb_c_api_src_files
odbql.h
odbql.cc  
)

ecbuild_add_library( TARGET             OdbCAPI
                     INSTALL_HEADERS    LISTED
                     HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/odb_api 
                     SOURCES            ${odb_c_api_src_files} 
                     LIBS               Odb
                     )

ecbuild_add_executable( TARGET    odbql_example
                        SOURCES   example.cc  
                        LIBS      OdbCAPI odbtools odbtest )
