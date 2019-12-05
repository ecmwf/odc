
module fapi_general_tests

    use odc
    use odc_config
    use, intrinsic :: iso_fortran_env
    implicit none

contains

    function test_odc_version() result(success)

        ! Test that we obtain the expected version number

        logical :: success
        character(:), allocatable :: version_str

        success = .true.

        if (odc_version(version_str) /= ODC_SUCCESS) then
            write(error_unit, *) 'getting version string failed'
            success = .false.
        end if

        if (version_str /= odc_version_str) then
            write(error_unit, *) "Unexpected version: ", version_str
            write(error_unit, *) "Expected: ", odc_version_str
            success = .false.
        endif

    end function

    function test_git_sha1() result(success)

        ! Test that we obtain the expected version number

        logical :: success
        character(:), allocatable :: sha1

        success = .true.

        if (odc_git_sha1(sha1) /= ODC_SUCCESS) then
            write(error_unit, *) 'getting git sha1 string failed'
            success = .false.
        end if

        if (sha1 /= odc_git_sha1_str .and. sha1 /= "not available") then
            write(error_unit, *) "Unexpected git sha1: ", sha1
            write(error_unit, *) "Expected: ", odc_git_sha1_str
            success = .false.
        endif

    end function

    function test_type_names() result(success)
        logical :: success
        integer :: test_types(6) = [ODC_IGNORE, ODC_INTEGER, ODC_REAL, ODC_STRING, ODC_BITFIELD, ODC_DOUBLE]
        character(8) :: names(6) = [character(8) :: "ignore", "integer", "real", "string", "bitfield", "double"]
        character(:), allocatable :: name
        integer :: ntypes, n

        success = .true.

        if (odc_column_type_count(ntypes) /= ODC_SUCCESS) then
            write(error_unit, *) 'Failed to get type count'
            success = .false.
        end if

        if (ntypes /= 6) then
            write(error_unit, *) 'Unexpected number of types found'
            success = .false.
        endif

        do n = 1, ntypes
            if (odc_column_type_name(test_types(n), name) /= ODC_SUCCESS) then
                write(error_unit, *) 'Failed to get type name, type=', test_types(n)
                success = .false.
            end if

            if (name /= trim(names(n))) then
                write(error_unit, *) 'Unexpected type name "', name, '" for type ', n
                success = .false.
            end if
        end do

    end function

    function test_error_handling() result(success)
        logical :: success
        type(odc_reader) :: reader
        integer :: j, err
        success = .true.

        ! Check that an error is correctly reported

        do j = 1, 2

            err = reader%open_path("invalid-path")
            if (err == ODC_SUCCESS) then
                write(error_unit, *) 'open_path succeeded unexpectedly with "invalid-path"'
                success = .false.
            end if

            if (odc_error_string(err) /= "Cannot open invalid-path  (No such file or directory)") then
                write(error_unit, *) 'unexpected error message: ', odc_error_string(err)
                success = .false.
            endif
        end do

    end function

end module


program fapi_general

    use fapi_general_tests
    implicit none

    logical :: success
    success = .true.

    if (odc_initialise_api() /= ODC_SUCCESS) then
        write(error_unit, *) 'Failed to initialise ODC api'
        success = .false.
    end if

    success = success .and. test_odc_version()
    success = success .and. test_git_sha1()
    success = success .and. test_type_names()
    success = success .and. test_error_handling()

    if (.not. success) stop -1

end program
