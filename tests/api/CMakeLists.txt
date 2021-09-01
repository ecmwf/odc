
list( APPEND _api_odc_tests
    read
    import
)

foreach( _test ${_api_odc_tests} )
    ecbuild_add_test(
        TARGET       odc_api_${_test}
        SOURCES      ${_test}.cc
        ENVIRONMENT  ${test_environment}
        TEST_DEPENDS odc_get_test_data
        LIBS         eckit odccore odctools )
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

list( APPEND _api_odc_tests_scripts
    usage_examples.sh
)

foreach( _script ${_api_odc_tests_scripts} )
    ecbuild_add_test(
        TARGET odc_api_${_script}
        TYPE script
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/${_script}
        ENVIRONMENT PATH=${CMAKE_BINARY_DIR}/bin:$ENV{PATH}
    )
endforeach()
