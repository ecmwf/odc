
#list( APPEND _odc_server_tests
#    test_partition )

foreach( _test ${_odc_server_tests} )
    ecbuild_add_test(
        TARGET       odc_${_test}
        SOURCES      ${_test}.cc
        TEST_DEPENDS odc_get_test_data
        ENVIRONMENT  ${test_environment}
        LIBS         eckit odccore
    )
endforeach()

