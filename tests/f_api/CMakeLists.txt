
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
        LIBS         fodc )
endforeach()

ecbuild_add_executable( TARGET     odc-fortran-ls
                        SOURCES    odc_ls.f90
                        LIBS       fodc
                        CONDITION    HAVE_FORTRAN
                        NOINSTALL)
