
#list( APPEND _odb_server_tests
#    test_partition )

foreach( _test ${_odb_server_tests} )
    ecbuild_add_test(
        TARGET       odb_api_${_test}
        SOURCES      ${_test}.cc
        TEST_DEPENDS get_odb_api_test_data
        INCLUDES     ${ECKIT_INCLUDE_DIRS} ${ODB_API_INCLUDE_DIRS}
        ENVIRONMENT  ${test_environment}
        LIBS         eckit Odb
    )
endforeach()

