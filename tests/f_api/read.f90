
module fapi_read_tests

    use odc
    use odc_config
    use, intrinsic :: iso_fortran_env
    implicit none

contains

    function test_count_lines() result(success)

        ! Test that we obtain the expected version number

        type(odc_reader) :: reader
        type(odc_frame) :: frame
        integer(8) :: frame_count, row_count
        logical :: success

        success = .true.
        call reader%open_path("../2000010106.odb")

        frame_count = 0
        row_count = 0

        do while (reader%next_frame(frame, .false.))

            frame_count = frame_count + 1
            row_count = row_count + frame%row_count()

            if (frame%column_count() /= 51) then
                write(error_unit, *) 'Unexpected column count: ', frame%column_count(), ' /= 51'
                success = .false.
            endif

        end do

        if (frame_count /= 333) then
            write(error_unit, *) 'Unexpected frame count: ', frame_count, ' /= 333'
            success = .false.
        endif

        if (row_count /= 3321753) then
            write(error_unit, *) 'Unexpected row count: ', row_count, ' /= 3321753'
            success = .false.
        endif

        call reader%close()

    end function

    ! function test_separate_free_calls

end module


program fapi_general

    use fapi_read_tests
    implicit none

    logical :: success

    call odc_initialise_api()

    success = .true.
    success = success .and. test_count_lines()

    if (.not. success) stop -1

end program
