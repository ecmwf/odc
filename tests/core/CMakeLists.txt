
list( APPEND _core_odb_tests
    test_encode_odb
    test_decode_odb
    test_codecs_write
    test_codecs_read
    test_codecs_end_to_end
    test_reencode_string_table
    test_concatenated_odbs
    test_minmax

    test_select_iterator
    test_text_reader
)

foreach( _test ${_core_odb_tests} )
    ecbuild_add_test(
        TARGET       odc_${_test}
        SOURCES      ${_test}.cc ../TemporaryFiles.h ../MockDataHandles.h
        TEST_DEPENDS get_odc_test_data
        INCLUDES     ${ECKIT_INCLUDE_DIRS} ${odc_INCLUDE_DIRS}
        ENVIRONMENT  ${test_environment}
        LIBS         eckit Odb )
endforeach()

