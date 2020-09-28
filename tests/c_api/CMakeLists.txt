
list( APPEND _api_odc_tests
    read
    encode
    errors
)

foreach( _test ${_api_odc_tests} )
    ecbuild_add_test(
        TARGET       odc_capi_${_test}
        SOURCES      ${_test}.cc
        ENVIRONMENT  ${test_environment}
        TEST_DEPENDS odc_get_test_data
        LIBS         eckit odccore )
endforeach()
