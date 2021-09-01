
list( APPEND _capi_odc_tests
    read
    encode
)

foreach( _test ${_capi_odc_tests} )
    ecbuild_add_test(
        TARGET       odc_capi_${_test}
        SOURCES      ${_test}.cc
        ENVIRONMENT  ${test_environment}
        TEST_DEPENDS odc_get_test_data
        LIBS         eckit odccore )
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

list( APPEND _capi_odc_tests_scripts
    usage_examples.sh
)

foreach( _script ${_capi_odc_tests_scripts} )
    ecbuild_add_test(
        TARGET odc_capi_${_script}
        TYPE script
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/${_script}
        ENVIRONMENT PATH=${CMAKE_BINARY_DIR}/bin:$ENV{PATH}
    )
endforeach()
