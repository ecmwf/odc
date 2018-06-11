
include( ${SWIG_USE_FILE} )

include_directories( ${PYTHON_INCLUDE_DIRS} )
set_source_files_properties( pyodbapi.i PROPERTIES CPLUSPLUS ON )
swig_add_module( pyodbapi python pyodbapi.i )
swig_link_libraries( pyodbapi Odb ${PYTHON_LIBRARIES} )

install( TARGETS ${SWIG_MODULE_pyodbapi_REAL_NAME}
         DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python2.7/site-packages/odb )

install( DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
         DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python2.7/site-packages
         FILES_MATCHING PATTERN "*.py" )
