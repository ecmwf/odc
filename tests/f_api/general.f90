
module fapi_general_tests

    use odc
    use odc_config
    use, intrinsic :: iso_fortran_env
    implicit none

    integer :: test_error_handler_calls = 0
    integer(8) :: test_error_handler_last_context
    integer :: test_error_handler_last_error

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

        if (odc_vcs_version(sha1) /= ODC_SUCCESS) then
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

    function test_odc_integer_behaviour() result(success)

        ! Test that we can set the integer behaviour as both double and long

        logical :: success

        success = .true.

        if (odc_integer_behaviour(ODC_INTEGERS_AS_DOUBLES) /= ODC_SUCCESS) then
            write(error_unit, *) 'setting integer behaviour to doubles failed'
            success = .false.
        end if

        if (odc_integer_behaviour(ODC_INTEGERS_AS_LONGS) /= ODC_SUCCESS) then
            write(error_unit, *) 'setting integer behaviour to longs failed'
            success = .false.
        endif

    end function

    function test_odc_set_failure_handler() result(success)

        ! Test that we can set failure handler and that it is being called on error with appropriate information

        logical :: success
        integer(8) :: original_context = 123456
        integer(8) :: context, err
        type(odc_reader) :: reader
        character(:), allocatable :: name

        success = .true.
        context = original_context

        ! Set test error handler and its context
        if (odc_set_failure_handler(test_error_handler, context) /= ODC_SUCCESS) then
            write(error_unit, *) 'setting failure handler failed'
            success = .false.
        end if

        ! Trigger an error
        err = reader%open_path("invalid-path")
        if (err == ODC_SUCCESS) then
            write(error_unit, *) 'open_path succeeded unexpectedly with "invalid-path"'
            success = .false.
        end if

        ! Check number of error handler calls
        if (test_error_handler_calls /= 1) then
            write(error_unit, *) 'error handler not called expected number of times:', test_error_handler_calls, '/=', 1
            success = .false.
        end if

        ! Check last received error handler context
        if (test_error_handler_last_context /= context) then
            write(error_unit, *) 'error handler context differs:', test_error_handler_last_context, '/=', context
            success = .false.
        end if

        ! Check last received error code
        if (test_error_handler_last_error /= err) then
            write(error_unit, *) 'error handler error code differs:', test_error_handler_last_error, '/=', err
            success = .false.
        end if

        ! Change context value
        context = 654321

        ! Trigger another error
        err = odc_integer_behaviour(0)
        if (err == ODC_SUCCESS) then
            write(error_unit, *) 'odc_integer_behaviour succeeded unexpectedly with "0"'
            success = .false.
        end if

        ! Check number of error handler calls
        if (test_error_handler_calls /= 2) then
            write(error_unit, *) 'error handler not called expected number of times:', test_error_handler_calls, '/=', 2
            success = .false.
        end if

        ! Check last received error handler context, it should match the original one
        if (test_error_handler_last_context /= original_context) then
            write(error_unit, *) 'error handler context differs:', test_error_handler_last_context, '/=', original_context
            success = .false.
        end if

        ! Check last received error code, it should match the latest one
        if (test_error_handler_last_error /= err) then
            write(error_unit, *) 'error handler error code differs:', test_error_handler_last_error, '/=', err
            success = .false.
        end if

        ! Change context value another time
        context = 0

        ! Trigger yet another error
        err = odc_column_type_name(999999, name)
        if (err == ODC_SUCCESS) then
            write(error_unit, *) 'odc_column_type_name succeeded unexpectedly with "999999"'
            success = .false.
        end if

        ! Check number of error handler calls
        if (test_error_handler_calls /= 3) then
            write(error_unit, *) 'error handler not called expected number of times:', test_error_handler_calls, '/=', 3
            success = .false.
        end if

        ! Check last received error handler context, it should match the original one
        if (test_error_handler_last_context /= original_context) then
            write(error_unit, *) 'error handler context differs:', test_error_handler_last_context, '/=', original_context
            success = .false.
        end if

        ! Check last received error code, it should match the latest one
        if (test_error_handler_last_error /= err) then
            write(error_unit, *) 'error handler error code differs:', test_error_handler_last_error, '/=', err
            success = .false.
        end if
    end function

    subroutine test_error_handler(context, error)
        integer(8), intent(in) :: context
        integer, intent(in) :: error

        test_error_handler_calls = test_error_handler_calls + 1
        test_error_handler_last_context = context
        test_error_handler_last_error = error
    end subroutine

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
    success = success .and. test_odc_integer_behaviour()
    success = success .and. test_odc_set_failure_handler()

    if (.not. success) stop -1

end program
