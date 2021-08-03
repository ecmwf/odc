### test data files


list( APPEND test_data_files
2000010106-reduced.odb
)

ecbuild_get_test_multidata( TARGET odc_get_test_data
                            #DIRNAME odc/tests
                            NAMES ${test_data_files}
                            NOCHECK )

### list tests

list( APPEND odc_tests
Test_AggregateFunctions
Test_AggregateFunctions2
Test_AggregateFunctions3
Test_AtTableInTheOutput
Test_Bitfields
Test_CREATE_TABLE_and_SELECT_INTO
Test_CommandLineParsing
Test_DispatchingWriter
Test_Distinct
Test_FunctionDateAndTime
Test_FunctionTdiff
Test_FunctionThin
Test_FunctionTypeConversion
Test_FunctionsForAngleConversion
Test_FunctionsForTemperatureConversion
Test_Int16_MissingCodec
Test_IntegerValues
Test_JULIAN_SECONDS
Test_MetaData
Test_MissingValue
Test_OrderBy
Test_SelectDataHandle
Test_SelectStarAt
Test_SelectTwoFiles
Test_Setvbuffer
Test_Star
Test_bitfieldsLength
Test_bitfields_hash_operator
Test_hash_operator_in_where
Test_hash_operator_on_select_list
Test_include
Test_log_error
Test_meta_data_reader_checks_if_file_truncated
Test_operator_ge
Test_rownumber1
Test_selectAggregatedAndNonAggregated
Test_selectAggregatedAndNonAggregated2
Test_selectAggregatedAndNonAggregated3
Test_selectAggregatedAndNonAggregatedNULL
Test_select_constant_value
Test_sqlOutputFormatting
Test_stringInWhere
Test_windSpeedWindDirection
Test_QuestionMarkHandlingWhenSplittingByStringColumn_ODB235

Test_example_select_data_read_results
Test_example_read_data
Test_example_write_data

Test_c_api_example_write_data
)


set( test_environment
  odc_CODES=${PROJECT_SOURCE_DIR}/etc
  odc_HOME=${PROJECT_SOURCE_DIR}
  odc_TEST_DATA_PATH=${CMAKE_CURRENT_BINARY_DIR}
  PATH=${CMAKE_BINARY_DIR}/bin:$ENV{PATH}

  TEST_DHSHOME=${CMAKE_CURRENT_BINARY_DIR}/dhshome/
  TEST_DATA_DIRECTORY=${CMAKE_CURRENT_BINARY_DIR}
  )

### odc tests

set( _prev_test odc_get_test_data  )

foreach( _test ${odc_tests} )
    set( _dependencies ${_prev_test} )
    ecbuild_add_test( TARGET       ${_test}
                      COMMAND      $<TARGET_FILE:odc>
                      ARGS         ${_test}
                      ENVIRONMENT  ${test_environment}
                      LABELS       odc
                      TEST_DEPENDS ${_dependencies})
    set( _prev_test ${_test} )
endforeach()

ecbuild_add_executable( TARGET      test_client_lib_fortran
                        CONDITION   HAVE_FORTRAN AND HAVE_ODB
                        SOURCES     test_client_lib_fortran.f90
                        LIBS        odc_fortran
                        LINKER_LANGUAGE Fortran)

ecbuild_add_executable( TARGET      test_client_lib_cpp
                        CONDITION   HAVE_ODB
                        SOURCES     test_client_lib_cpp.cc
                        LIBS        odccore odctools )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

add_subdirectory( core )
add_subdirectory( sql )
add_subdirectory( tools )
add_subdirectory( api )
add_subdirectory( c_api )
add_subdirectory( f_api )
