
list( APPEND _api_fodc_tests
    general
    read
    encode
)

foreach( _test ${_api_fodc_tests} )
    ecbuild_add_test(
        TARGET       odc_fapi_${_test}
        SOURCES      ${_test}.f90
        ENVIRONMENT  ${test_environment}
        TEST_DEPENDS odc_get_test_data
        CONDITION    HAVE_FORTRAN
        LINKER_LANGUAGE Fortran
        LIBS         fodc )
endforeach()

list( APPEND _api_fodc_example_sources
    odc_header
    odc_encode_custom
    odc_encode_row_major
    odc_ls
)

list( APPEND _api_fodc_example_targets
    odc-fortran-header
    odc-fortran-encode-custom
    odc-fortran-encode-row-major
    odc-fortran-ls
)

list( LENGTH _api_fodc_example_sources _count )
math( EXPR _count "${_count}-1" )

foreach( _i RANGE ${_count} )
    list( GET _api_fodc_example_sources ${_i} _sources )
    list( GET _api_fodc_example_targets ${_i} _target )

    ecbuild_add_executable( TARGET     ${_target}
                            SOURCES    ${_sources}.f90
                            LIBS       fodc
                            CONDITION  HAVE_FORTRAN
                            LINKER_LANGUAGE Fortran
                            NOINSTALL )
endforeach()

list( APPEND _fapi_odc_tests_scripts
    usage_examples.sh
)

foreach( _script ${_fapi_odc_tests_scripts} )
    ecbuild_add_test(
        TARGET odc_fapi_${_script}
        TYPE script
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/${_script}
        ENVIRONMENT PATH=${CMAKE_BINARY_DIR}/bin:$ENV{PATH}
        CONDITION   HAVE_FORTRAN
    )
endforeach()
