
list( APPEND _api_odc_tests
    read
    import
)

foreach( _test ${_api_odc_tests} )
    ecbuild_add_test(
        TARGET       odc_api_${_test}
        SOURCES      ${_test}.cc
        INCLUDES     ${eckit_INCLUDE_DIRS} ${odc_INCLUDE_DIRS}
        ENVIRONMENT  ${test_environment}
        TEST_DEPENDS odc_get_test_data
        LIBS         eckit odccore )
endforeach()
