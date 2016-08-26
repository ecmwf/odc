###############################################################################
# swig python interface

if( HAVE_PYTHON AND SWIG_FOUND )
  join( ECKIT_SELF_INCLUDE_DIRS "', '" _ECKIT_INCLUDES )
  configure_file( setup.py.in setup.py )
  file( COPY odb test_python_odb_api.py odb_api_python_examples.py DESTINATION . )

  set( _odbapi_swig "_pyodbapi${CMAKE_SHARED_LIBRARY_SUFFIX}" )
  # Build the extension module for use in build tree with RPATH pointing to the build tree
  add_custom_command( OUTPUT ${_odbapi_swig}
                      COMMAND ${PYTHON_EXECUTABLE} setup.py build_ext --inplace --force -R ${CMAKE_BINARY_DIR}/lib
                      DEPENDS odb/pyodbapi.i setup.py.in Odb )
  add_custom_target(build_swig_wrapper ALL DEPENDS ${_odbapi_swig})

  # Call distutils for installation
  install(CODE "execute_process(COMMAND ${PYTHON_EXECUTABLE} setup.py install --prefix $ENV{DESTDIR}${CMAKE_INSTALL_PREFIX} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})")

  list( APPEND python_odb_api_data_files 2000010106.odb )
  ecbuild_get_test_multidata( TARGET python_odb_api_get_test_data
                              NAMES ${python_odb_api_data_files}
                              NOCHECK )

  ecbuild_add_test ( TARGET       test_python_odb_api.py
                     TYPE         PYTHON
                     COMMAND      ${CMAKE_CURRENT_BINARY_DIR}/test_python_odb_api.py
                     CONDITION    HAVE_PYTHON
                     TEST_DEPENDS python_odb_api_get_test_data
                   )

endif()

