
list( APPEND tools_data_files
    2000010106.odb
)

ecbuild_get_test_multidata( TARGET get_odc_tools_data
                            NAMES ${tools_data_files}
                            NOCHECK )


list( APPEND tools_tests_scripts
    test_odb_split.sh
    test_odb_sql_split.sh
    test_odb_sql_variables.sh
#   test_odb_sql_match_in.sh    # Disabled since ODB-431. Deprecated.
    test_odb_sql_like.sh
    test_odb_import.sh )


foreach( _script ${tools_tests_scripts} )
    ecbuild_add_test(
        TARGET odc_${_script}
        TYPE script
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/${_script}
        ENVIRONMENT PATH=${CMAKE_BINARY_DIR}/bin:$ENV{PATH}
        TEST_DEPENDS get_odc_tools_data
    )
endforeach()

# Add a custom target to make these files visible in qtcreator

add_custom_target(
    test_tools_file_visibility
    SOURCES ${tools_tests_scripts}
)
