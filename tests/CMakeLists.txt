### test data files

list( APPEND test_migrator_data_files
2000010106.old.ECMA.tar.gz )

ecbuild_get_test_multidata( TARGET get_migrator_test_data 
                            #DIRNAME odb_api/tests 
                            NAMES ${test_migrator_data_files} 
                            NOCHECK )

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
TestAggregateFunctions3.odb )

ecbuild_get_test_multidata( TARGET get_odb_api_test_data
                            #DIRNAME odb_api/tests 
                            NAMES ${test_data_files} 
                            NOCHECK )

### list tests

list( APPEND odb_migrator_tests
#Test_AAAImportODB
#Test_AAAImportODBDispatching
)

list( APPEND odb_api_tests
Test_AggregateFunctions
Test_AggregateFunctions2
Test_AggregateFunctions3
Test_AtTableInTheOutput
Test_Bitfields
Test_CREATE_TABLE_and_SELECT_INTO
Test_CatFiles
Test_ChildTable_DataLoaderReturnsExpectedChildTableRows
Test_ChildTable_DataLoaderReturnsExpectedNumberOfChildTableColumns
Test_ChildTable_DataLoaderReturnsExpectedNumberOfChildTableRows
Test_Codec
Test_CodecOptimization
Test_CommandLineParsing
Test_ConstCodec
Test_ConstIntegerCodec
Test_DataRowCanSetDoubles
Test_DataRowCanSetIntegers
Test_DataRowCanSetStrings
Test_DataRowInitializedFromColumnsHasExpectedFlags
Test_DataRowInitializedFromColumnsHasExpectedSize
Test_DataRowInitializedFromColumnsHasExpectedValues
Test_Decoding
Test_DispatchingWriter
Test_Distinct
Test_EmptyPageWithFillMark_PushBackRowsToEmptyPageWithFillMark
Test_EmptyPageWithFillMark_ResizeEmptyPageWithFillMark
Test_EmptyPage_CanIncreasePageSizeAndInitializeValues
Test_EmptyPage_CannotIncreasePageSizeBeyondItsCapacity
Test_EmptyPage_DecreadingSizeOfEmptyPageToZero
Test_EmptyPage_EmptyPageCanBeCleared
Test_EmptyPage_EmptyPageHasZeroSize
Test_EmptyPage_EmptyPageIsEmpty
Test_EmptyPage_EmptyPageIsNotFull
Test_EmptyPage_HasTheCorrectCapacity
Test_EmptyPage_IncreadingSizeOfEmptyPageToHalfTheCapacity
Test_EmptyPage_IteratorReturnsExpectedNumberOfRows
Test_EmptyPage_PushingBackRowsToAnEmptyPage
Test_EmptyTable_CanBeCleared
Test_EmptyTable_CapacityCanBeIncreased
Test_EmptyTable_HasExpectedCapacity
Test_EmptyTable_HasZeroSize
Test_EmptyTable_IsEmpty
Test_EmptyTable_SizeCanBeIncreased
Test_FastODA2Request
Test_FastODA2Request2
Test_FastODA2Request3
Test_FilledDataSet_DataLoaderReturnsExpectedNumberOfTables
Test_FilledDataSet_DataLoaderReturnsExpectedTables
Test_FilledDataSet_DataSaverOutputContainsExpectedRows
Test_FilledLink_DataLinkHasExpectedSize
Test_FilledLink_DataLinkIteratorReturnsExpectedNumberOfChildRows
Test_FilledLink_DataLinkIteratorReturnsExpectedNumberOfParentRows
Test_FilledLink_DataLinkIteratorReturnsExpectedRows
Test_FilledLink_DataLinkReturnsExpectedChildTable
Test_FilledLink_DataLinkReturnsExpectedParentTable
Test_FilledLink_InsertRowAtTheBeginningOfTheFirstRange
Test_FilledLink_InsertRowAtTheEndOfTheLastRange
Test_FilledTable_AdvanceIteratorBackward
Test_FilledTable_AdvanceIteratorByHalf
Test_FilledTable_AdvanceIteratorByZero
Test_FilledTable_AdvanceIteratorForewardByOne
Test_FilledTable_AdvanceIteratorToTheEnd
Test_FilledTable_CanBeCleared
Test_FilledTable_CanIterateBackward
Test_FilledTable_CanIterateForeward
Test_FilledTable_CapacityCanBeIncreased
Test_FilledTable_DistanceBetweenBeginAndEndIteratorIsAsExpected
Test_FilledTable_DistanceBetweenTheSameIteratorsIsZero
Test_FilledTable_GetRowAtTheBack
Test_FilledTable_HasExpectedCapacity
Test_FilledTable_HasExpectedNonZeroSize
Test_FilledTable_InsertRowAtTheBeginning
Test_FilledTable_InsertRowAtTheEnd
Test_FilledTable_IsNotEmpty
Test_FilledTable_SizeCanBeDecreased
Test_FilledTable_SizeCanBeIncreased
Test_Fixture_DataLoaderWithTableMappings
Test_Fixture_InsertingTablesIncreasesSize
Test_Fixture_LoadDataSetFromSqlQueries
Test_FullPage_CanDecreasePageSize
Test_FullPage_CanNotInsertRowToAFullPage
Test_FullPage_DecreadingSizeOfFullPageByHalf
Test_FullPage_DecreadingSizeOfFullPageToZero
Test_FullPage_DecreasingSizeOfFullPageToHalfTheCapacity
Test_FullPage_FullPageCanBeCleared
Test_FullPage_FullPageHasSizeOfItsCapacity
Test_FullPage_FullPageIsFull
Test_FullPage_FullPageIsNotEmpty
Test_FullPage_IncreadingSizeOfFullPageBeyondItsCapacity
Test_FullPage_IteratorReturnsExpectedNumberOfRows
Test_FullPage_IteratorReturnsExpectedRows
Test_FullPage_PushingBackRowsToTheFullPage
Test_FullPage_SplitFullPage
Test_FunctionCircle
Test_FunctionDateAndTime
Test_FunctionDistance
Test_FunctionDotp
Test_FunctionEqBox
Test_FunctionNorm
Test_FunctionRggBox
Test_FunctionTdiff
Test_FunctionThin
Test_FunctionTypeConversion
Test_FunctionsForAngleConversion
Test_FunctionsForTemperatureConversion
Test_HalfEmptyPage_InsertRowAtTheBeginning
Test_HalfEmptyPage_InsertRowAtTheEnd
Test_HashTable_clone
Test_InMemoryDataHandle
Test_InitializedDataRowHasExpectedFlags
Test_InitializedDataRowHasExpectedSize
Test_InitializedDataRowHasExpectedValues
Test_InnerJoin_InnerJoinHasExpectedColumns
Test_InnerJoin_InnerJoinHasExpectedNumberOfColumns
Test_InnerJoin_InnerJoinReturnsExpectedNumberOfResults
Test_InnerJoin_InnerJoinReturnsExpectedValues
Test_Int16_MissingCodec
Test_IntegerValues
Test_JULIAN_SECONDS
Test_LinkedTables_DataJoinCanBeUsedWithStlAlgorithms
Test_LinkedTables_DataJoinHasExpectedColumns
Test_LinkedTables_DataJoinHasExpectedNumberOfColumns
Test_LinkedTables_DataJoinReturnsExpectedNumberOfResults
Test_LinkedTables_DataJoinReturnsExpectedValues
Test_MetaData
Test_MetaDataReader
Test_MinMax
Test_MissingValue
Test_OrderBy
Test_Ordinals_CopiedDataRowHasExpectedFlags
Test_Ordinals_CopiedDataRowHasExpectedValues
Test_Ordinals_DataRowCopyHasExpectedFlags
Test_Ordinals_DataRowCopyHasExpectedSize
Test_Ordinals_DataRowCopyHasExpectedValues
Test_ParentTable_DataLoaderReturnsExpectedNumberOfParentTableColumns
Test_ParentTable_DataLoaderReturnsExpectedNumberOfParentTableRows
Test_ParentTable_DataLoaderReturnsExpectedParentTableRows
Test_SQLFunctionsInfo
Test_SelectAggregate_ReturnsExpectedColumns
Test_SelectAggregate_ReturnsExpectedNumberOfColumns
Test_SelectAggregate_ReturnsExpectedNumberOfResults
Test_SelectAggregate_ReturnsExpectedResults
Test_SelectAll_CanUseCopyToAppendResults
Test_SelectAll_ReturnsExpectedColumns
Test_SelectAll_ReturnsExpectedNumberOfColumns
Test_SelectAll_ReturnsExpectedNumberOfResults
Test_SelectAll_ReturnsExpectedResults
Test_SelectDataHandle
Test_SelectIterator
Test_SelectIterator2
Test_SelectIterator3
Test_SelectStarAt
Test_SelectTwoFiles
Test_SelectWhere_ReturnsExpectedColumns
Test_SelectWhere_ReturnsExpectedNumberOfColumns
Test_SelectWhere_ReturnsExpectedNumberOfResults
Test_SelectWhere_ReturnsExpectedResults
Test_Setvbuffer
Test_Star
Test_TEMPLATE
Test_TextReaderIterator_parseBitfields
Test_TextReaderIterator_parseBitfields_32bits_limit
Test_TextSelect
Test_TextSelect2
Test_ThreeColumnTable_LineByLineInitialization
Test_ThreeColumnTable_MultiLineInitialization
Test_UnInitializedDataRowHasExpectedFlags
Test_UnInitializedDataRowHasExpectedSize
Test_WriteCatFiles
Test_bitfieldsLength
Test_bitfields_hash_operator
Test_blocksSizes
Test_dateTime
Test_hash_operator_in_where
Test_hash_operator_on_select_list
Test_include
Test_log_error
Test_meta_data_reader_checks_if_file_truncated
Test_meta_data_reader_fails_scanning_corrupted_file
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
Test_IndexTool
Test_BufferedHandle_ODB80

#Examples.cc
Test_example_select_data_read_results
Test_example_read_data
Test_example_write_data
Test_example_sql_select_callback
Test_example_sql_select_callback_invoked_as_a_request
Test_example_sql_select_and_a_mars_verb_as_a_callback

#CAPIExamples.cc
Test_c_api_example_select_data_read_results
Test_c_api_example_read_data
Test_c_api_example_write_data
)

### get location of odb executable

get_target_property( odb_bin odb LOCATION )

set( test_environment
  ODB_API_CODES=${PROJECT_SOURCE_DIR}/etc
  ODB_API_HOME=${PROJECT_SOURCE_DIR}
  ODB_API_TEST_DATA_PATH=${CMAKE_CURRENT_BINARY_DIR}
  PATH=${CMAKE_BINARY_DIR}/bin:$ENV{PATH}
  ODB_RTABLE_PATH=${PROJECT_SOURCE_DIR}/etc 

  TEST_DHSHOME=${CMAKE_CURRENT_SOURCE_DIR}/dhshome/
  TEST_DATA_DIRECTORY=${CMAKE_CURRENT_BINARY_DIR}
  )

### migrator tests

set( _prev_test get_migrator_test_data )
foreach( _test ${odb_migrator_tests} )
  ecbuild_add_test( TARGET     ${_test}
                    COMMAND    ${odb_bin}
                    ARGS       ${_test}
                    CONDITION  HAVE_MIGRATOR
                    ENVIRONMENT ${test_environment} ${ODB_ENVIRONMENT}
                    TEST_DEPENDS ${_prev_test} )
    set( _prev_test ${_test} )
endforeach()

### odb2_to_odb1 tests

ecbuild_get_test_multidata( TARGET get_odb2_to_odb1_data NAMES odb2_to_odb1.ECMA.conv.tar.gz NOCHECK )

ecbuild_add_test( TARGET test_odb2_to_odb1
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test_odb2_to_odb1.sh
    ENVIRONMENT ${test_environment} ${ODB_ENVIRONMENT}
        PATH=${CMAKE_BINARY_DIR}/bin:${odb_BASE_DIR}/bin:$ENV{PATH}
        ODB_IO_METHOD=4
        ODB_IO_GRPSIZE=160
    TEST_DEPENDS get_odb2_to_odb1_data
    CONDITION HAVE_FORTRAN AND ODB_FOUND AND ODB_HAVE_ECMA)

### odb2netcdf tests

ecbuild_get_test_multidata( TARGET get_odb2netcdf_data
    NAMES odb2netcdf_1d.odb odb2netcdf_1d.cdl
          odb2netcdf_2d_hdr.odb odb2netcdf_2d_body.odb odb2netcdf_2d.cdl
    NOCHECK )

ecbuild_add_test( TARGET test_odb2netcdf_1d
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test_odb2netcdf_1d.sh
    ENVIRONMENT ${test_environment}
    TEST_DEPENDS get_odb2netcdf_data
    CONDITION HAVE_NETCDF )

ecbuild_add_test( TARGET test_odb2netcdf_2d
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test_odb2netcdf_2d.sh
    ENVIRONMENT ${test_environment}
    TEST_DEPENDS get_odb2netcdf_data
    CONDITION HAVE_NETCDF )

### odb_api tests

# remove dependency on migrator tests if migrator not enabled
if( NOT HAVE_MIGRATOR )
  unset( odb_migrator_tests )
endif()

if( HAVE_MIGRATOR )
    set( _prev_test get_migrator_test_data  )
else()
    set( _prev_test get_odb_api_test_data  )
endif()

foreach( _test ${odb_api_tests} )
    if( HAVE_MIGRATOR )
        set( _dependencies ${odb_migrator_tests} ${_prev_test} )
    else()
        set( _dependencies ${_prev_test} )
    endif()
    ecbuild_add_test( TARGET       ${_test}
                      COMMAND      ${odb_bin}
                      ARGS         ${_test}
                      ENVIRONMENT  ${test_environment}
                      TEST_DEPENDS ${_dependencies})
    set( _prev_test ${_test} )
endforeach()

#### ECML ODB Module verbs tests

#list( APPEND ecml_test_data_files  2000010106.odb)
#ecbuild_get_test_multidata( TARGET ecml_get_odb_api_test_data DIRNAME odb_api/tests NAMES ${ecml_test_data_files} NOCHECK )

list( APPEND odb_module_tests
    #test_odb_governance.ecml
    test_sql_splitting.ecml
    test_chunk.ecml
    test_chunk2.ecml
    test_embedded_ecml_in_from_clause.ecml
    test_sql_like.ecml
    test_sql_match_in.ecml
    test_create_partitions.ecml

# Disabled till ODB-215 fixed as it fails in Bamboo
    test_server_side_processing.ecml
    test_stage.ecml
)

# tests moved from ODB Server that populate ODB Server root - that data is used by other tests
ecbuild_add_test ( TARGET       test_ec_archiving.ecml
                   COMMAND      ${odb_bin}
                   ARGS         ecml ${PROJECT_SOURCE_DIR}/src/odb_api/ecml_verbs/tests/test_ec_archiving.ecml
                   ENVIRONMENT  ${test_environment} 
                   TEST_DEPENDS get_odb_api_test_data 
                                get_mars_client_test_data_ec )


ecbuild_add_test ( TARGET       test_mo_archiving.ecml
                   COMMAND      ${odb_bin}
                   ARGS         ecml ${PROJECT_SOURCE_DIR}/src/odb_api/ecml_verbs/tests/test_mo_archiving.ecml
                   ENVIRONMENT  ${test_environment} 
                   TEST_DEPENDS get_odb_api_test_data 
                                get_mars_client_test_data_mo)

foreach( _test ${odb_module_tests} )
    ecbuild_add_test ( TARGET       ${_test}
                       COMMAND      ${odb_bin}
                       ARGS         ecml ${PROJECT_SOURCE_DIR}/src/odb_api/ecml_verbs/tests/${_test}
                       ENVIRONMENT  ${test_environment} 

                       TEST_DEPENDS get_odb_api_test_data 
                                    get_mars_client_test_data_mo
                                    get_mars_client_test_data_ec
                                    test_ec_archiving.ecml
                                    test_mo_archiving.ecml
                       )
endforeach()


ecbuild_get_test_multidata( TARGET  get_mars_client_test_data_mo
                            DIRNAME odb_api/tests
                            NAMES   20150218_glu_surface_odb2 )

ecbuild_get_test_multidata( TARGET  get_mars_client_test_data_ec
                            DIRNAME odb_api/tests
                            NAMES   conv_mfb_20151108_12.odb )

ecbuild_add_executable( TARGET      test_client_lib_fortran
                        CONDITION   HAVE_FORTRAN AND HAVE_ODB
                        SOURCES     test_client_lib_fortran.f90
                        LIBS        Odb_fortran ${ODB_LIBS} marskit
                        LINKER_LANGUAGE Fortran)

ecbuild_add_executable( TARGET      test_client_lib_fortran_server_side_ecml
                        CONDITION   HAVE_FORTRAN AND HAVE_ODB
                        SOURCES     test_client_lib_fortran_server_side_ecml.f90
                        LIBS        Odb_fortran ${ODB_LIBS} marskit
                        LINKER_LANGUAGE Fortran)

ecbuild_add_executable( TARGET      test_client_lib_cpp
                        CONDITION   HAVE_ODB
                        SOURCES     test_client_lib_cpp.cc
                        INCLUDES    ${ODB_API_INCLUDE_DIRS}
                        LIBS        Odb odbtools marskit )
