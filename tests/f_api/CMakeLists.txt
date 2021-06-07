
list( APPEND _api_fodc_tests
    general
    read
    encode
    usage_examples
)

foreach( _test ${_api_fodc_tests} )
    ecbuild_add_test(
        TARGET       odc_fapi_${_test}
        SOURCES      ${_test}.f90
        ENVIRONMENT  ${test_environment}
        TEST_DEPENDS odc_get_test_data
        CONDITION    HAVE_FORTRAN
        LIBS         fodc )
endforeach()

list( APPEND _api_fodc_test_extras
    odc-fortran-header.1
    odc-fortran-header.2
    odc-fortran-encode-custom.1
    odc-fortran-encode-custom.2
    odc-fortran-encode-row-major.1
    odc-fortran-encode-row-major.2
    odc-fortran-ls.1
    odc-fortran-ls.2
)

foreach( _extra ${_api_fodc_test_extras} )
    file(
        COPY        ${_extra}
        DESTINATION "${CMAKE_BINARY_DIR}/tests/f_api")
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
                            NOINSTALL )
endforeach()
