
list( APPEND _capi_odc_tests
    read
    encode
    errors
    usage_examples
)

foreach( _test ${_capi_odc_tests} )
    ecbuild_add_test(
        TARGET       odc_capi_${_test}
        SOURCES      ${_test}.cc
        ENVIRONMENT  ${test_environment}
        TEST_DEPENDS odc_get_test_data
        LIBS         eckit odccore )
endforeach()

list( APPEND _api_odc_test_extras
    odc-c-header.1
    odc-c-header.2
    odc-c-encode-custom.1
    odc-c-encode-custom.2
    odc-c-encode-row-major.1
    odc-c-encode-row-major.2
    odc-c-ls.1
    odc-c-ls.2
)

foreach( _extra ${_api_odc_test_extras} )
    file(
        COPY        ${_extra}
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

list( APPEND _capi_odc_example_sources
    odc_header
    odc_encode_custom
    odc_encode_row_major
    odc_ls
)

list( APPEND _capi_odc_example_targets
    odc-c-header
    odc-c-encode-custom
    odc-c-encode-row-major
    odc-c-ls
)

list( LENGTH _capi_odc_example_sources _count )
math( EXPR _count "${_count}-1" )

foreach( _i RANGE ${_count} )
    list( GET _capi_odc_example_sources ${_i} _sources )
    list( GET _capi_odc_example_targets ${_i} _target )

    ecbuild_add_executable( TARGET     ${_target}
                            SOURCES    ${_sources}.c
                            LIBS       odccore
                            NOINSTALL )
endforeach()
