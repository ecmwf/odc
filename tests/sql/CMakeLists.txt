
list( APPEND _sql_odc_tests
    test_functions
    test_text_reader_select
    test_oda_output
)

foreach( _test ${_sql_odc_tests} )
    ecbuild_add_test(
        TARGET       odc_${_test}
        SOURCES      ${_test}.cc ../TemporaryFiles.h
        ENVIRONMENT  ${test_environment}
        TEST_DEPENDS odc_get_test_data
        LIBS         eckit odccore )
endforeach()



# TODO: Pure tests for the pure-sql stuff. The functions clearly invoke odc-specific behaviour
