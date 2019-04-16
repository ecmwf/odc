
list( APPEND _api_fodc_tests
    general
)

foreach( _test ${_api_fodc_tests} )
    ecbuild_add_test(
        TARGET       odc_fapi_${_test}
        SOURCES      ${_test}.f90
        ENVIRONMENT  ${test_environment}
        TEST_DEPENDS get_odc_test_data
        LIBS         fodc )
endforeach()
