
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
    # test_table
)

foreach( _test ${_core_odc_tests} )
    ecbuild_add_test(
        TARGET       odc_${_test}
        SOURCES      ${_test}.cc ../TemporaryFiles.h
        TEST_DEPENDS odc_get_test_data
        ENVIRONMENT  ${test_environment}
        LIBS         eckit odccore )
endforeach()

