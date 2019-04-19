
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


    character(14), parameter :: column_10_bitfield_names(*) = [ character(14) :: &
        "lat_humon", "lat_qcsub", "lat_override", "lat_flag", "lat_hqc_flag", "lon_humon", "lon_qcsub", &
        "lon_override", "lon_flag", "lon_hqc_flag", "date_humon", "date_qcsub", "date_override", &
        "date_flag", "date_hqc_flag", "time_humon", "time_qcsub", "time_override", "time_flag", &
        "time_hqc_flag", "stalt_humon", "stalt_qcsub", "stalt_override", "stalt_flag", "stalt_hqc_flag" &
    ]

    integer, parameter :: column_10_bitfield_sizes(*) = [ &
        1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1]

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
        character(:), allocatable :: column_name, field_name
        integer :: ncols, col, column_type, field, field_size, expected_offset, field_offset
        integer :: element_size, element_size_doubles
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
                element_size = frame%column_data_size(col)
                element_size_doubles = frame%column_data_size_doubles(col)

                if (column_name /= trim(example_column_names(col))) then
                    write(error_unit,'(3a,i2,3a)') 'Unexpected column name ', column_name, &
                               ' for column ', col, ' (expected ', trim(example_column_names(col)), ')'
                    success = .false.
                end if

                if (column_type /= example_column_types(col)) then
                    write(error_unit, '(a,i1,a,i2,a,i1,a)') 'Unexpected column type ', column_type, &
                            ' for column ', col, ' (expected ', example_column_types(col), ')'
                    success = .false.
                end if

                if (element_size /= 8) then
                    write(error_unit, '(a,i1,a,i2,a)') 'Unexpected column data size ', element_size, &
                            ' for column ', col, ' (expected 8)'
                    success = .false.
                end if

                if (element_size_doubles /= 1) then
                    write(error_unit, '(a,i1,a,i2,a)') 'Unexpected column doubles data size ', &
                            element_size_doubles, ' for column ', col, ' (expected 1)'
                    success = .false.
                end if

                if (column_type == ODC_BITFIELD) then
                    if (frame%column_bitfield_count(col) <= 0) then
                        write(error_unit, *) "Bitfields expected for bitfield column"
                        success = .false.
                    end if
                else
                    if (frame%column_bitfield_count(col) /= 0) then
                        write(error_unit, *) "Unexpected bitfields for non-bitfield column"
                        success = .false.
                    end if
                end if

            end do

            ! Test bitfields for column 10

            if (frame%column_bitfield_count(10) /= 25) then
                write(error_unit, *) "Expected 25 bitfield fields for column 10. Got ", &
                                     frame%column_bitfield_count(10)
                success = .false.
            end if

            expected_offset = 0
            do field = 1, 25

                col = 10
                field_name = frame%column_bits_name(col, field)
                field_size = frame%column_bits_size(col, field)
                field_offset = frame%column_bits_offset(col, field)

                if (field_name /= trim(column_10_bitfield_names(field))) then
                    write(error_unit, '(3a,i2,3a)') 'Unexpected field name ', field_name, ' for field ', &
                            field, ' (expected ', trim(column_10_bitfield_names(field)), ')'
                    success = .false.
                end if

                if (field_size /= column_10_bitfield_sizes(field)) then
                    write(error_unit, '(a,i2,a,i2,a,i2,a)') 'Unexpected field size ', field_size, &
                            ' for field ', field, ' (expected ', column_10_bitfield_sizes(field), ')'
                    success = .false.
                end if

                if (field_offset /= expected_offset) then
                    write(error_unit, '(a,i2,a,i2,a,i2,a)') 'Unexpected field offset ', field_offset, &
                            ' for field ', field, ' (expected ', expected_offset, ')'
                    success = .false.
                end if

                expected_offset = expected_offset + field_size
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
