
list( APPEND _api_odc_tests
    read
    import
    usage_examples
)

foreach( _test ${_api_odc_tests} )
    ecbuild_add_test(
        TARGET       odc_api_${_test}
        SOURCES      ${_test}.cc
        ENVIRONMENT  ${test_environment}
        TEST_DEPENDS odc_get_test_data
        LIBS         eckit odccore odctools )
endforeach()

list( APPEND _api_odc_test_extras
    odc-cpp-encode-custom.1
    odc-cpp-encode-custom.2
    odc-cpp-header.1
    odc-cpp-header.2
    odc-cpp-index.1
    odc-cpp-index.2
    odc-cpp-ls.1
    odc-cpp-ls.2
    span.csv
)

foreach( _extra ${_api_odc_test_extras} )
    file(
        COPY        ${_extra}
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

list( APPEND _api_odc_example_sources
    odc_encode_custom
    odc_header
    odc_index
    odc_ls
)

list( APPEND _api_odc_example_targets
    odc-cpp-encode-custom
    odc-cpp-header
    odc-cpp-index
    odc-cpp-ls
)

list( LENGTH _api_odc_example_sources _count )
math( EXPR _count "${_count}-1" )

foreach( _i RANGE ${_count} )
    list( GET _api_odc_example_sources ${_i} _sources )
    list( GET _api_odc_example_targets ${_i} _target )

    ecbuild_add_executable( TARGET     ${_target}
                            SOURCES    ${_sources}.cc
                            LIBS       eckit odccore
                            NOINSTALL )
endforeach()
