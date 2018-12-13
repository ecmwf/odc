###############################################################################
# swig python interface

if( HAVE_PYTHON AND SWIG_FOUND )
  join( ECKIT_INCLUDE_DIRS "', '" _ECKIT_INCLUDES )
  configure_file( setup.py.in setup.py )
  file( COPY odb test_python_odc.py legacy_test_python_odc.py legacy_odc_python_examples.py DESTINATION . )

  add_subdirectory("odb")

  list( APPEND python_odc_data_files  2000010106.odb ATOVS.trimmed.odb )
  ecbuild_get_test_multidata( TARGET python_odc_get_test_data
                              NAMES ${python_odc_data_files}
                              NOCHECK )

  set( test_environment_python
         odc_SCHEMA_PATH=${CMAKE_CURRENT_BINARY_DIR}/../../tests/cma.hh)

  ecbuild_add_test ( TARGET       test_python_odc.py
                     TYPE         PYTHON
                     ENVIRONMENT  ${test_environment_python} 
                     COMMAND      ${CMAKE_CURRENT_BINARY_DIR}/test_python_odc.py
                     CONDITION    HAVE_PYTHON
                     LABELS       odc odc_python
                     TEST_DEPENDS python_odc_get_test_data get_odc_test_data
                   )

  ecbuild_add_test ( TARGET       legacy_test_python_odc.py
                     TYPE         PYTHON
                     COMMAND      ${CMAKE_CURRENT_BINARY_DIR}/legacy_test_python_odc.py
                     CONDITION    HAVE_PYTHON
                     LABELS       odc odc_python
                     TEST_DEPENDS python_odc_get_test_data get_odc_test_data
                   )

endif()
