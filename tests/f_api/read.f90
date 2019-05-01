
module fapi_read_tests

    use odc
    use odc_config
    use, intrinsic :: iso_fortran_env
    implicit none

contains

    subroutine check_call(err, msg, success)
        integer, intent(in) :: err
        character(*), intent(in) :: msg
        logical, intent(inout) :: success

        if (err /= ODC_SUCCESS) then
            write(error_unit, *) 'Failed API call: ', msg
            write(error_unit, *) 'Error: ', odc_error_string(err)
            success = .false.
        end if
    end subroutine


    function test_count_lines() result(success)

        ! Test that we obtain the expected version number

        type(odc_reader) :: reader
        type(odc_frame) :: frame
        integer(8) :: frame_count, row_count, tmp_8
        integer :: err, tmp_4
        logical :: success

        success = .true.
        call check_call(reader%open_path("../2000010106.odb"), "open ODB", success)
        call check_call(frame%initialise(reader), "initialise frame", success)

        frame_count = 0
        row_count = 0

        err = frame%next()
        do while (err == ODC_SUCCESS)

            call check_call(frame%row_count(tmp_8), "row count", success)

            frame_count = frame_count + 1
            row_count = row_count + tmp_8

            call check_call(frame%column_count(tmp_4), "column count", success)
            if (tmp_4 /= 51) then
                write(error_unit, *) 'Unexpected column count: ', tmp_4, ' /= 51'
                success = .false.
            endif

            err = frame%next()
        end do

        if (err /= ODC_ITERATION_COMPLETE) call check_call(err, "next frame", success)

        if (frame_count /= 333) then
            write(error_unit, *) 'Unexpected frame count: ', frame_count, ' /= 333'
            success = .false.
        endif

        if (row_count /= 3321753) then
            write(error_unit, *) 'Unexpected row count: ', row_count, ' /= 3321753'
            success = .false.
        endif

        call check_call(reader%close(), "close reader", success)

    end function

    function test_column_details() result(success)

        type(odc_reader) :: reader
        type(odc_frame) :: frame
        character(:), allocatable :: column_name, field_name
        integer :: ncols, col, column_type, field, field_size, expected_offset, field_offset
        integer :: element_size, element_size_doubles, bitfield_count
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
        call check_call(reader%open_path("../2000010106.odb"), "open reader", success)
        call check_call(frame%initialise(reader), "initialise frame", success)

        call check_call(frame%next(), "get the first frame", success)

        call check_call(frame%column_count(ncols), "column count", success)
        if (ncols /= 51) then
            write(error_unit, *) 'Expected 51 columns'
            success = .false.
        endif

        ! n.b. -- 1-based indexing!
        do col = 1, ncols

            call check_call(frame%column_attrs(col, &
                                               name=column_name, &
                                               type=column_type, &
                                               element_size=element_size, &
                                               element_size_doubles=element_size_doubles, &
                                               bitfield_count=bitfield_count), "column attrs", success)

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
                if (bitfield_count <= 0) then
                    write(error_unit, *) "Bitfields expected for bitfield column"
                    success = .false.
                end if
            else
                if (bitfield_count /= 0) then
                    write(error_unit, *) "Unexpected bitfields for non-bitfield column"
                    success = .false.
                end if
            end if

        end do

        ! Test bitfields for column 10

        call check_call(frame%column_attrs(10, bitfield_count=bitfield_count), "bitfield count", success)
        if (bitfield_count /= 25) then
            write(error_unit, *) "Expected 25 bitfield fields for column 10. Got ", bitfield_count
            success = .false.
        end if

        expected_offset = 0
        do field = 1, 25

            ! Look at column 10
            call check_call(frame%bitfield_attrs(10, field, &
                                                 name=field_name, &
                                                 offset=field_offset, &
                                                 size=field_size), "bitfield attrs", success)

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

        call check_call(frame%free(), "free frame", success)
        call check_call(reader%close(), "close reader", success)
    end function

    function check_frame_2_values(array_data) result(success)

        real(8) :: array_data(:,:)
        logical :: success

        integer :: row, i
        integer, parameter :: expected_seqno(*) = [6106691, 6002945, 6003233, 6105819]
        integer, parameter :: expected_obschar(*) = [537918674, 135265490, 135265490, 537918674]
        integer(8) :: missing_integer
        real(8) :: missing_double

        success = .true.

        call check_call(odc_missing_integer(missing_integer), "missing integer", success)
        call check_call(odc_missing_double(missing_double), "missing double", success)

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
            if (int(array_data(row, 14)) /= missing_integer) then
                write(error_unit, *) 'Expected value with set missing value. Got ', int(array_data(row, 14)), ', &
                                     expected ', missing_integer
                success = .false.
            end if

            ! repres_error (REAL, missing)
            if (array_data(row, 49) /= missing_double) then
                write(error_unit, *) 'Expected value with set missing value. Got ', array_data(row, 49), ', &
                                     expected ', missing_double
                success = .false.
            end if
        end do

    end function

    function test_decode_columns_allocate() result(success)

        type(odc_reader) :: reader
        type(odc_frame) :: frame
        type(odc_decoder) :: decoder
        integer(8) :: nrows, nrows2
        integer :: ncols
        logical :: success, column_major
        real(8), pointer :: array_data(:,:)

        success =.true.
        call check_call(reader%open_path("../2000010106.odb"), "open reader", success)
        call check_call(frame%initialise(reader), "initialise frame", success)

        ! Read the second frame, because why not.
        call check_call(frame%next(), "get first frame", success)
        call check_call(frame%next(), "get second frame", success)

        call check_call(decoder%initialise(), "initialise decoder", success)
        call check_call(decoder%defaults_from_frame(frame), "decoder from frame", success)
        call check_call(decoder%decode(frame, nrows), "do decode", success)

        if (nrows /= 10000) then
            write(error_unit, *) 'Unexpected number of rows decoded'
            success = .false.
        end if

        call check_call(decoder%row_count(nrows2), "decoder row count", success)
        if (nrows2 /= 10000) then
            write(error_unit, *) 'Got row count ', nrows, ' not 10000'
            success = .false.
        end if

        call check_call(decoder%column_count(ncols), "decoder column count", success)
        if (ncols /= 51) then
            write(error_unit, *) 'Got column count ', ncols, ' not 51'
            success = .false.
        end if

        call check_call(decoder%data(array_data, column_major), "get decoded data", success)

        if (any(shape(array_data) /= [10000, 51])) then
            write(error_unit, *) 'Unexpected data dimensions'
            success = .false.
        end if

        if (.not. column_major) then
            write(error_unit, *) 'Expected column major by default'
            success = .false.
        end if

        success = success .and. check_frame_2_values(array_data)

        call check_call(decoder%free(), "free decoder", success)
        call check_call(reader%close(), "free reader", success)

    end function

    function test_decode_array_reuse() result(success)

        use, intrinsic :: iso_c_binding

        type(odc_reader) :: reader
        type(odc_frame) :: frame
        type(odc_decoder) :: decoder
        integer(8) :: rows_decoded, nrows
        integer :: ncols
        logical :: success
        real(8), target :: array_data(11000, 51)

        success = .true.
        call check_call(reader%open_path("../2000010106.odb"), "open reader", success)
        call check_call(frame%initialise(reader), "initialise frame", success)

        call check_call(frame%next(), "get first frame", success)

        call check_call(decoder%initialise(), "initialise decoder", success)
        call check_call(decoder%defaults_from_frame(frame), "decoder frame defaults", success)
        call check_call(decoder%set_data(array_data), "set array data", success)
        call check_call(decoder%decode(frame, rows_decoded), "decode first frame", success)

        if (rows_decoded /= 10000) then
            write(error_unit, *) 'Unexpected number of rows decoded'
            success = .false.
        end if

        call check_call(frame%next(), "get second frame", success)

        call check_call(decoder%decode(frame, rows_decoded), "decode second frame", success)

        if (rows_decoded /= 10000) then
            write(error_unit, *) 'Unexpected number of rows decoded'
            success = .false.
        end if

        call check_call(decoder%row_count(nrows), "decoder row count", success)
        if (nrows /= 11000) then
            write(error_unit, *) 'Got row count ', nrows, ' not 11000'
            write(error_unit, *) 'Row count should be related to the size of the decode target, not the decode data'
            success = .false.
        end if

        call check_call(decoder%column_count(ncols), "decoder column count", success)
        if (ncols /= 51) then
            write(error_unit, *) 'Got column count ', ncols, ' not 51'
            success = .false.
        end if

        success = success .and. check_frame_2_values(array_data)

        call check_call(decoder%free(), "free decoder", success)
        call check_call(reader%close(), "close reader", success)

    end function

    function test_decode_aggregate() result(success)

        type(odc_reader) :: reader
        type(odc_frame) :: frame
        type(odc_decoder) :: decoder
        integer(8) :: rows_decoded, nrows
        integer :: ncols
        logical :: success
        real(8), pointer :: array_data(:,:)

        success = .true.
        call check_call(reader%open_path("../2000010106.odb"), "open reader", success)
        call check_call(frame%initialise(reader), "initialise frame", success)

        call check_call(frame%next(maximum_rows=99999_8), "get first (aggregate) frame", success)

        call check_call(decoder%initialise(), "initialise decoder", success)
        call check_call(decoder%defaults_from_frame(frame), "decoder frame defaults", success)
        call check_call(decoder%decode(frame, rows_decoded, nthreads=4), "decode threaded", success)

        if (rows_decoded /= 90000) then
            write(error_unit, *) 'Unexpected number of rows decoded'
            success = .false.
        end if

        call check_call(decoder%row_count(nrows), "decoder row count", success)
        if (nrows /= 90000) then
            write(error_unit, *) 'Got row count ', nrows, ' not 90000'
            success = .false.
        end if

        call check_call(decoder%column_count(ncols), "decoder column count", success)
        if (ncols /= 51) then
            write(error_unit, *) 'Got column count ', ncols, ' not 51'
            success = .false.
        end if

        call check_call(decoder%data(array_data), "get array data", success)

        if (any(shape(array_data) /= [90000, 51])) then
            write(error_unit, *) 'Unexpected data dimensions'
            success = .false.
        end if

        call check_call(decoder%free(), "free decoder", success)
        call check_call(reader%close(), "close reader", success)

    end function

end module


program fapi_general

    use fapi_read_tests
    implicit none

    logical :: success

    success = .true.
    call check_call(odc_initialise_api(), "initialise api", success)

    success = test_count_lines() .and. success
    success = test_column_details() .and. success
    success = test_decode_columns_allocate() .and. success
    success = test_decode_array_reuse() .and. success
    success = test_decode_aggregate() .and. success

    if (.not. success) stop -1

end program
