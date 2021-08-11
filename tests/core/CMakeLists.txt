
list( APPEND core_data_files
    odb_533_1.odb
    odb_533_2.odb
)

ecbuild_get_test_multidata( TARGET odc_get_core_data
                            NAMES ${core_data_files}
                            NOCHECK )


list( APPEND _core_odc_tests
    test_encode_odb
    test_decode_odb
    test_codecs_write
    test_codecs_read
    test_codecs_end_to_end
    test_codecs_end_to_end2
    test_reencode_string_table
    test_concatenated_odbs
    test_minmax
    test_metadata

    test_select_iterator
    test_text_reader
    test_table_iterator
    test_initial_missing
)

foreach( _test ${_core_odc_tests} )
    ecbuild_add_test(
        TARGET       odc_${_test}
        SOURCES      ${_test}.cc ../TemporaryFiles.h
        TEST_DEPENDS odc_get_test_data odc_get_core_data
        ENVIRONMENT  ${test_environment}
        LIBS         eckit odccore )
endforeach()

