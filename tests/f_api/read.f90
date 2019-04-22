
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

        do while (reader%next_frame(frame))

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

        success = .true.
        call reader%open_path("../2000010106.odb")

        if (reader%next_frame(frame)) then

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

    function check_frame_2_values(array_data) result(success)

        real(8) :: array_data(:,:)
        logical :: success

        integer :: row, i
        integer, parameter :: expected_seqno(*) = [6106691, 6002945, 6003233, 6105819]
        integer, parameter :: expected_obschar(*) = [537918674, 135265490, 135265490, 537918674]

        success = .true.

        do i = 1, 4
            row = 1 + ((i-1) * 765)

            ! Expver
            if (trim(transfer(array_data(row, 1), "        ")) /= "0018") then
                write(error_unit, *) 'unexpected expver in row ', row, ' (expected 0018, got ', &
                                     transfer(array_data(row, 1), "        ") ,')'
                success = .false.
            end if

            ! Test seqno (INTEGER)
            if (int(array_data(row, 4)) /= expected_seqno(i)) then
                write(error_unit, *) 'Unexpected seqno value. row=', row, ", expected=", &
                                     expected_seqno(i), ", got=", int(array_data(row, 4))
                success = .false.
            end if

            ! obschar (BITFIELD)
            if (int(array_data(row, 6)) /= expected_obschar(i)) then
                write(error_unit, *) 'Unexpected obschar value. row=', row, ", expected=", &
                                     expected_obschar(i), ", got=", int(array_data(row, 6))
                success = .false.
            end if

            ! Sortbox (INTEGER, missing)
            if (int(array_data(row, 14)) /= odc_missing_integer()) then
                write(error_unit, *) 'Expected value with set missing value. Got ', int(array_data(row, 14)), ', &
                                     expected ', odc_missing_integer()
                success = .false.
            end if

            ! repres_error (REAL, missing)
            if (array_data(row, 49) /= odc_missing_double()) then
                write(error_unit, *) 'Expected value with set missing value. Got ', array_data(row, 49), ', &
                                     expected ', odc_missing_double()
                success = .false.
            end if
        end do

    end function

    function test_decode_columns_allocate() result(success)

        type(odc_reader) :: reader
        type(odc_frame) :: frame
        type(odc_decode_target) :: decode_target
        integer(8) :: nrows
        logical :: success
        real(8), pointer :: array_data(:,:)

        success =.true.
        call reader%open_path("../2000010106.odb")

        if (.not. reader%next_frame(frame)) then
            write(error_unit, *) 'Expected at least one frames'
            success = .false.
            return
        end if

        ! Read the second frame, because why not.
        if (reader%next_frame(frame)) then

            call decode_target%initialise(frame)
            !call decode_target%set_row_count(11000_8)

            nrows = frame%decode(decode_target)

            if (nrows /= 10000) then
                write(error_unit, *) 'Unexpected number of rows decoded'
                success = .false.
            end if

            if (decode_target%row_count() /= 10000) then
                write(error_unit, *) 'Got row count ', decode_target%row_count(), ' not 10000'
                success = .false.
            end if

            if (decode_target%column_count() /= 51) then
                write(error_unit, *) 'Got column count ', decode_target%column_count(), ' not 51'
                success = .false.
            end if

            array_data => decode_target%data()

            if (any(shape(array_data) /= [10000, 51])) then
                write(error_unit, *) 'Unexpected data dimensions'
                success = .false.
            end if

            success = success .and. check_frame_2_values(array_data)

            call decode_target%free()

        else
            write(error_unit, *) 'Expected at least two frames'
            success = .false.
            return
        end if

        call reader%close()

    end function

    function test_decode_array_reuse() result(success)

        use, intrinsic :: iso_c_binding

        type(odc_reader) :: reader
        type(odc_frame) :: frame
        type(odc_decode_target) :: decode_target
        integer(8) :: nrows
        logical :: success
        real(8), target :: array_data(11000, 51)

        success = .true.
        call reader%open_path("../2000010106.odb")

        if (.not. reader%next_frame(frame)) then
            write(error_unit, *) 'Expected at least one frames'
            success = .false.
            return
        end if

        call decode_target%initialise(frame)
        call decode_target%set_data_array(array_data)
        if (frame%decode(decode_target) /= 10000) then
            write(error_unit, *) 'Unexpected number of rows decoded'
            success = .false.
        end if

        ! Read the second frame, because why not. Reuse the decode target!
        if (reader%next_frame(frame)) then

            nrows = frame%decode(decode_target)

            if (nrows /= 10000) then
                write(error_unit, *) 'Unexpected number of rows decoded'
                success = .false.
            end if

            if (decode_target%row_count() /= 11000) then
                write(error_unit, *) 'Got row count ', decode_target%row_count(), ' not 11000'
                write(error_unit, *) 'Row count should related to the size of the decode target, not the decode data'
                success = .false.
            end if

            if (decode_target%column_count() /= 51) then
                write(error_unit, *) 'Got column count ', decode_target%column_count(), ' not 51'
                success = .false.
            end if

            success = success .and. check_frame_2_values(array_data)

            call decode_target%free()

        else
            write(error_unit, *) 'Expected at least two frames'
            success = .false.
            return
        end if

        call reader%close()

    end function

    function test_decode_aggregate() result(success)

        type(odc_reader) :: reader
        type(odc_frame) :: frame
        type(odc_decode_target) :: decode_target
        integer(8) :: nrows
        logical :: success
        real(8), pointer :: array_data(:,:)

        success = .true.
        call reader%open_path("../2000010106.odb")

        if (reader%next_frame(frame, maximum_rows=99999)) then

            call decode_target%initialise(frame)
            nrows = frame%decode(decode_target, nthreads=4)

            if (nrows /= 90000) then
                write(error_unit, *) 'Unexpected number of rows decoded'
                success = .false.
            end if

            if (decode_target%row_count() /= 90000) then
                write(error_unit, *) 'Got row count ', decode_target%row_count(), ' not 90000'
                success = .false.
            end if

            if (decode_target%column_count() /= 51) then
                write(error_unit, *) 'Got column count ', decode_target%column_count(), ' not 51'
                success = .false.
            end if

            array_data => decode_target%data()

            if (any(shape(array_data) /= [90000, 51])) then
                write(error_unit, *) 'Unexpected data dimensions'
                success = .false.
            end if

            call decode_target%free()

        else
            write(error_unit, *) 'Expected at least one frame'
            success = .false.
            return
        end if

        call reader%close()

    end function

end module


program fapi_general

    use fapi_read_tests
    implicit none

    logical :: success

    call odc_initialise_api()

    success = .true.
    success = test_count_lines() .and. success
    success = test_column_details() .and. success
    success = test_decode_columns_allocate() .and. success
    success = test_decode_array_reuse() .and. success
    success = test_decode_aggregate() .and. success

    if (.not. success) stop -1

end program
