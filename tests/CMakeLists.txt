### test data files


list( APPEND test_data_files
2000010106.odb.gz
2000010106.odb
2000010106.1.0.odb
2000010106.2.0.odb
2000010106.3.0.odb
2000010106.4.0.odb
2000010106.5.0.odb
2000010106.6.0.odb
2000010106.7.0.odb
2000010106.7.1.odb
2000010106.7.2.odb
2000010106.7.3.odb
mondb.1.12.odb
TestIntegerValues.odb
split_crash_on_andate_and_antime.odb
TestMetaDataReader.odb
a1to10twice.odb
TestAggregateFunctions2.odb
dribu.odb
TestAggregateFunctions3.odb

# MetOffice schema for testing ODB-127
ECMA.ddl
ECMATMP.ddl
cma.hh
bitfields.hh
group_id.hh
mdi.hh
ops_obsgroups.hh
ops_subtypes.hh
report_types.hh
sensor.hh
varno.hh
vertco_type.hh
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

#Examples.cc
Test_example_select_data_read_results
Test_example_read_data
Test_example_write_data

#CAPIExamples.cc
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

### odb2netcdf tests

ecbuild_get_test_multidata(
    TARGET odc_get_odb2netcdf_data
    NAMES odb2netcdf_1d.odb odb2netcdf_1d.cdl
          odb2netcdf_2d_hdr.odb odb2netcdf_2d_body.odb odb2netcdf_2d.cdl
    NOCHECK )

ecbuild_add_test( TARGET test_odb2netcdf_1d
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test_odb2netcdf_1d.sh
    ENVIRONMENT ${test_environment}
    LABELS       odc odc_netcdf
    TEST_DEPENDS get_odb2netcdf_data
    CONDITION HAVE_NETCDF )

ecbuild_add_test( TARGET test_odb2netcdf_2d
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test_odb2netcdf_2d.sh
    ENVIRONMENT ${test_environment}
    TEST_DEPENDS get_odb2netcdf_data
    LABELS       odc odc_netcdf
    CONDITION HAVE_NETCDF )

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

# n.b. Old behaviour (c.f. ODB-394) re-enabled with ODB_PATCH_TIME_ENCODE_RELATIVE.
# This is a bit of a hack...

ecbuild_get_test_multidata( TARGET  odc_get_mars_client_test_data_mo
                            DIRNAME odc/tests
                            NAMES   20150218_glu_surface_odb2 )

ecbuild_get_test_multidata( TARGET  odc_get_mars_client_test_data_ec
                            DIRNAME odc/tests
                            NAMES   conv_mfb_20151108_12.odb )

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
add_subdirectory( server )
add_subdirectory( api )
add_subdirectory( c_api )
add_subdirectory( f_api )
