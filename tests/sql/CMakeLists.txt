
list( APPEND _sql_odb_tests
    test_functions
    test_text_reader_select
)

foreach( _test ${_sql_odb_tests} )
    ecbuild_add_test(
        TARGET       odc_${_test}
        SOURCES      ${_test}.cc ../TemporaryFiles.h ../MockDataHandles.h
        INCLUDES     ${ECKIT_INCLUDE_DIRS} ${odc_INCLUDE_DIRS}
        ENVIRONMENT  ${test_environment}
        TEST_DEPENDS get_odc_test_data
        LIBS         eckit Odb )
endforeach()



# TODO: Pure tests for the pure-sql stuff. The functions clearly invoke odb-specific behaviour
