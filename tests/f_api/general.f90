
module fapi_general_tests

    use odc
    use odc_config
    use, intrinsic :: iso_fortran_env
    implicit none

contains

    function test_odc_version() result(success)

        ! Test that we obtain the expected version number

        logical :: success
        character(20) :: version_str

        success = .true.
        version_str = odc_version()
        if (version_str /= odc_version_str) then
            write(error_unit, *) "Unexpected version: ", version_str
            write(error_unit, *) "Expected: ", odc_version_str
            success = .false.
        endif

    end function

    function test_git_sha1() result(success)

        ! Test that we obtain the expected version number

        logical :: success
        character(40) :: sha1

        success = .true.
        sha1 = odc_git_sha1()
        if (sha1 /= odc_git_sha1_str .and. sha1 /= "not available") then
            write(error_unit, *) "Unexpected git sha1: ", sha1
            write(error_unit, *) "Expected: ", odc_git_sha1_str
            success = .false.
        endif

    end function

    function test_type_names() result(success)
        logical :: success
        success = .true.

        if (odc_type_count() /= 6) then
            write(error_unit, *) 'Unexpected number of types found'
            success = .false.
        endif

        if (odc_type_name(ODC_IGNORE) /= "ignore" .or. &
            odc_type_name(ODC_INTEGER) /= "integer" .or. &
            odc_type_name(ODC_REAL) /= "real" .or. &
            odc_type_name(ODC_STRING) /= "string" .or. &
            odc_type_name(ODC_BITFIELD) /= "bitfield" .or. &
            odc_type_name(ODC_DOUBLE) /= "double") then
            write(error_unit, *) 'Unexpected type name'
            success = .false.
        endif

    end function

    function test_error_handling() result(success)
        logical :: success
        type(odc_reader) :: reader
        integer :: i, j
        success = .true.

        do i = 1, 2

            if (i == 1) then
                call odc_error_handling(ODC_ERRORS_CHECKED)
            else
                call odc_error_handling(ODC_ERRORS_REPORT)
            end if

            ! Check that an error is correctly reported

            do j = 1, 2

                call reader%open_path("invalid-path")

                if (odc_success()) then
                    write(error_unit, *) 'open_path succeeded unexpectedly with "invalid-path"'
                    success = .false.
                else if (odc_error_string() /= "Cannot open invalid-path  (No such file or directory)") then
                    write(error_unit, *) 'unexpected error message: ', odc_error_string()
                    success = .false.
                endif

                ! Skip resetting errors after first case of ODC_ERRORS_REPORT (i.e. the error is reported
                ! so we should continue)
                if (.not. (i == 2 .and. j == 1)) then

                    ! Check that errors are correctly reset
                    call odc_reset_error()

                    if (.not. odc_success()) then
                        write(error_unit, *) 'Reset of error state failed'
                        success = .false.
                    end if
                end if
            end do
        end do

    end function

end module


program fapi_general

    use fapi_general_tests
    implicit none

    logical :: success

    call odc_initialise_api()

    success = .true.
    success = success .and. test_odc_version()
    success = success .and. test_git_sha1()
    success = success .and. test_type_names()
    success = success .and. test_error_handling()

    if (.not. success) stop -1

end program
