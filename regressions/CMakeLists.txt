
list( APPEND regressions_data_files
    2000010106.odb
)

ecbuild_get_test_multidata( TARGET get_odb_api_regressions_data
                            #DIRNAME odb_api/tests
                            NAMES ${regressions_data_files}
                            NOCHECK )

ecbuild_add_test(
    TYPE script
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/ODB-374.sh
    ENVIRONMENT PATH=${CMAKE_BINARY_DIR}/bin:$ENV{PATH}
    TEST_DEPENDS get_odb_api_regressions_data
)
