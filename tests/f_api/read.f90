
module fapi_read_tests

    use odc
    use odc_config
    use, intrinsic :: iso_fortran_env
    implicit none

    character(23), parameter :: example_column_names(*) = [ character(23) :: &
        "expver@desc", "andate@desc", "antime@desc", "seqno@hdr", "obstype@hdr", &
        "obschar@hdr", "subtype@hdr", "date@hdr", "time@hdr", "rdbflag@hdr", &
        "status@hdr", "event1@hdr", "blacklist@hdr", "sortbox@hdr", "sitedep@hdr", &
        "statid@hdr", "ident@hdr", "lat@hdr", "lon@hdr", "stalt@hdr", &
        "modoro@hdr", "trlat@hdr", "trlon@hdr", "instspec@hdr", "event2@hdr", &
        "anemoht@hdr", "baroht@hdr", "sensor@hdr", "numlev@hdr", "varno_presence@hdr", &
        "varno@body", "vertco_type@body", "rdbflag@body", "anflag@body", "status@body", &
        "event1@body", "blacklist@body", "entryno@body", "press@body", "press_rl@body", &
        "obsvalue@body", "aux1@body", "event2@body", "ppcode@body", "level@body", &
        "biascorr@body", "final_obs_error@errstat", "obs_error@errstat", "repres_error@errstat", &
        "pers_error@errstat", "fg_error@errstat"]

    integer, parameter :: example_column_types(*) = [ &
        ODC_STRING, ODC_INTEGER, ODC_INTEGER, ODC_INTEGER, ODC_INTEGER, ODC_BITFIELD , &
        ODC_INTEGER, ODC_INTEGER, ODC_INTEGER, ODC_BITFIELD , ODC_BITFIELD , ODC_BITFIELD , &
        ODC_BITFIELD , ODC_INTEGER, ODC_INTEGER, ODC_STRING, ODC_INTEGER, ODC_REAL, &
        ODC_REAL, ODC_REAL, ODC_REAL, ODC_REAL, ODC_REAL, ODC_INTEGER, &
        ODC_INTEGER, ODC_REAL, ODC_REAL, ODC_INTEGER, ODC_INTEGER, ODC_BITFIELD , &
        ODC_INTEGER, ODC_INTEGER, ODC_BITFIELD , ODC_BITFIELD , ODC_BITFIELD , ODC_BITFIELD , &
        ODC_BITFIELD , ODC_INTEGER, ODC_REAL, ODC_REAL, ODC_REAL, ODC_REAL, &
        ODC_INTEGER, ODC_INTEGER, ODC_BITFIELD , ODC_REAL, ODC_REAL, ODC_REAL, &
        ODC_REAL, ODC_REAL, ODC_REAL]


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

    function test_column_details() result(success)

        type(odc_reader) :: reader
        type(odc_frame) :: frame
        character(:), allocatable :: column_name
        integer :: ncols, col, column_type
        logical :: success

        success = .true.
        call reader%open_path("../2000010106.odb")

        if (reader%next_frame(frame, .false.)) then

            ncols = frame%column_count()
            if (ncols /= 51) then
                write(error_unit, *) 'Expected 51 columns'
                success = .false.
            endif

            ! n.b. -- 1-based indexing!
            do col = 1, ncols

                column_name = frame%column_name(col)
                column_type = frame%column_type(col)

                if (column_name /= trim(example_column_names(col))) then
                    write(*,'(3a,i2,3a)') 'Unexpected column name ', column_name, &
                               ' for column ', col, ' (expected ', trim(example_column_names(col)), ')'
                    success = .false.
                end if

                if (column_type /= example_column_types(col)) then
                    write(*, '(a,i1,a,i2,a,i1,a)') 'Unexpected column type ', column_type, ' for column ', &
                                   col, ' (expected ', example_column_types(col), ')'
                    success = .false.
                end if

            end do

            call frame%free()
        else
            write(error_unit, *) 'Expected at least one frame'
            success = .false.
            return
        end if

        call reader%close()
    end function


    ! Test bitfield definition reading
    ! Test read on aggregate tables
    ! Test decoding

end module


program fapi_general

    use fapi_read_tests
    implicit none

    logical :: success

    call odc_initialise_api()

    success = .true.
    success = success .and. test_count_lines()
    success = success .and. test_column_details()

    if (.not. success) stop -1

end program
