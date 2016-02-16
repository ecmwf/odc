###############################################################################
# swig python interface

if( HAVE_PYTHON AND SWIG_FOUND AND PYTHONLIBS_FOUND )
    list( APPEND python_odb_api_data_files 2000010106.odb )
    ecbuild_get_test_multidata( TARGET python_odb_api_get_test_data
                                NAMES ${python_odb_api_data_files}
                                NOCHECK )


    message("Configured for building Python bindings.")
    set(CMAKE_SWIG_FLAGS "")
    include_directories( ${PYTHON_INCLUDE_PATH} )

    set_source_files_properties( pyodbapi.i PROPERTIES CPLUSPLUS ON )
    # set_source_files_properties( pyodbapi.i PROPERTIES SWIG_FLAGS "-includeall")

    file( RELATIVE_PATH relative_rpath "${CMAKE_INSTALL_PREFIX}/${PYTHON_DEST}" "${${PNAME}_FULL_INSTALL_LIB_DIR}" )
    ecbuild_append_to_rpath( ${relative_rpath} )
    ecbuild_append_to_rpath( "../../../lib" )        # for ctest to find the libs

    swig_add_module( pyodbapi python pyodbapi.i )
    set( EXTRA_LIBS eckit )
    swig_link_libraries( pyodbapi Odb ${PYTHON_LIBRARIES} eckit )

    #swig_link_libraries( pyodbapi Odb ${PYTHON_LIBRARIES} )

    #set(PYTHON_DEST "lib/python${PYTHON_VERSION}/site-packages" )
    set(PYTHON_SITE "${INSTALL_LIB_DIR}/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages" )
    set(PYTHON_DEST "${PYTHON_SITE}" )

    include_directories( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${PYTHON_INCLUDE_DIRS} )

    set( _swig_py pyodbapi.py )
    add_custom_command(
        OUTPUT ${_swig_py}
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${_swig_py_wrapper} ${CMAKE_CURRENT_BINARY_DIR}/${_swig_py}
        #DEPENDS _pyodbapi
    )

#    ecbuild_add_test ( TARGET       test_python_odb_api.py
#                       TYPE         PYTHON
#                       COMMAND      ${CMAKE_CURRENT_SOURCE_DIR}/test_python_odb_api.py
#                       CONDITION    HAVE_PYTHON
#                       ENVIRONMENT 
#                                    PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}:$ENV{PYTHONPATH}
#                                    LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}:${CMAKE_BINARY_DIR}/lib:$ENV{LD_LIBRARY_PATH}
#                                    PYTHON=${PYTHON_EXECUTABLE} 
#                       TEST_DEPENDS python_odb_api_get_test_data
#                     )

    install(TARGETS _pyodbapi DESTINATION ${PYTHON_DEST} )
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pyodbapi.py  DESTINATION ${PYTHON_DEST})


endif()

