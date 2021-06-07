
module fapi_read_tests

    use odc
    use odc_config
    use, intrinsic :: iso_fortran_env
    use, intrinsic :: iso_c_binding, only: c_loc,c_null_char
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

            call check_call(frame%column_attributes(col, &
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

        call check_call(frame%column_attributes(10, bitfield_count=bitfield_count), "bitfield count", success)
        if (bitfield_count /= 25) then
            write(error_unit, *) "Expected 25 bitfield fields for column 10. Got ", bitfield_count
            success = .false.
        end if

        expected_offset = 0
        do field = 1, 25

            ! Look at column 10
            call check_call(frame%bitfield_attributes(10, field, &
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
                                     &expected ', missing_integer
                success = .false.
            end if

            ! repres_error (REAL, missing)
            if (array_data(row, 49) /= missing_double) then
                write(error_unit, *) 'Expected value with set missing value. Got ', array_data(row, 49), ', &
                                     &expected ', missing_double
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

    function test_frame_properties_1_non_aggregated() result(success)

        ! Where the properties in the two frames are distinct (non-aggregated)

        type(odc_reader) :: reader
        type(odc_frame) :: frame
        integer :: err, nframes = 1, nproperties, idx
        logical :: aggregated = .false.
        character(:), allocatable, target :: version, key, val
        character(255) :: version_str
        logical :: success, exists
        success = .true.

        call test_generate_odb('properties-1.odb', 1, success)

        call check_call(reader%open_path('properties-1.odb'), 'opening path', success)
        call check_call(frame%initialise(reader), 'initialising frame', success)

        call check_call(odc_version(version), 'getting version number', success)
        write(version_str, *) 'odc version ', version
        version_str = trim(adjustl(version_str))

        ! Advance to the first frame in the stream in non-aggregated mode
        err = frame%next(aggregated)

        do while (err == ODC_SUCCESS)
            call check_call(frame%properties_count(nproperties), 'getting properties count', success)

            ! Check properties count
            if (nproperties /= 2) then
                write(error_unit, *) 'unexpected number of properties:', nproperties, '/=', 2
                success = .false.
            end if

            do idx = 1, 2
                call check_call(frame%property_idx(idx, key, val), 'getting property by index', success)

                ! Check getting properties by index
                if ((idx == 1 .and. nframes == 1) .or. (idx == 2 .and. nframes == 2)) then
                    if (key /= 'encoder' .or. val /= version_str) then
                        write(error_unit, *) 'unexpected property: ', key, ' => ', val , ' /= encoder => ', version_str
                        success = .false.
                    end if
                else if (nframes == 1) then
                    if (key /= 'foo' .or. val /= 'bar') then
                        write(error_unit, *) 'unexpected property: ', key, ' => ', val , ' /= foo => bar'
                        success = .false.
                    end if
                else
                    if (key /= 'baz' .or. val /= 'qux') then
                        write(error_unit, *) 'unexpected property: ', key, ' => ', val , ' /= baz => qux'
                        success = .false.
                    end if
                end if

                ! Check getting property values by key
                if (nframes == 1) then
                    call check_call(frame%property('encoder', val), 'getting property by key', success)
                    if (val /= version_str) then
                        write(error_unit, *) 'unexpected property value for encoder: ', val , ' /= ', version_str
                        success = .false.
                    end if

                    call check_call(frame%property('foo', val), 'getting property by key', success)
                    if (val /= 'bar') then
                        write(error_unit, *) 'unexpected property value for foo: ', val , ' /= bar'
                    end if
                else
                    call check_call(frame%property('encoder', val), 'getting property by key', success)
                    if (val /= version_str) then
                        write(error_unit, *) 'unexpected property value for encoder: ', val , ' /= ', version_str
                        success = .false.
                    end if

                    call check_call(frame%property('baz', val), 'getting property by key', success)
                    if (val /= 'qux') then
                        write(error_unit, *) 'unexpected property value for baz: ', val , ' /= qux'
                        success = .false.
                    end if
                end if

                ! Check for reading of non-existent properties
                call check_call(frame%property('non-existent', exists=exists), 'getting property by key', success)
                if (exists) then
                    write(error_unit, *) 'unexpected non-existent property: ', exists, ' /= .false.'
                    success = .false.
                end if
            end do

            nframes = nframes + 1

            ! Advances to the next frame in the stream in non-aggregated mode
            err = frame%next(aggregated)
        end do

        if (err /= ODC_ITERATION_COMPLETE) call check_call(err, 'get next frame', success)

        call check_call(reader%close(), 'closing reader', success)

        ! Check number of frames
        if (nframes - 1 /= 2) then
            write(error_unit, *) 'unexpected number of frames:', nframes - 1, '/=', 2
            success = .false.
        end if

    end function

    function test_frame_properties_1_aggregated() result(success)

        ! Where the properties in the two frames are distinct (aggregated)

        type(odc_reader) :: reader
        type(odc_frame) :: frame
        integer :: err, nframes = 1, nproperties, idx
        logical :: aggregated = .true.
        character(:), allocatable, target :: version, key, val
        character(255) :: version_str
        logical :: success, exists
        success = .true.

        call test_check_file_exists('properties-1.odb', success)

        call check_call(reader%open_path('properties-1.odb'), 'opening path', success)
        call check_call(frame%initialise(reader), 'initialising frame', success)

        call check_call(odc_version(version), 'getting version number', success)
        write(version_str, *) 'odc version ', version
        version_str = trim(adjustl(version_str))

        ! Advance to the first frame in the stream in aggregated mode
        err = frame%next(aggregated)

        do while (err == ODC_SUCCESS)
            call check_call(frame%properties_count(nproperties), 'getting properties count', success)

            ! Check properties count
            if (nproperties /= 3) then
                write(error_unit, *) 'unexpected number of properties:', nproperties, '/=', 3
                success = .false.
            end if

            do idx = 1, 3
                call check_call(frame%property_idx(idx, key, val), 'getting property by index', success)

                ! Check getting properties by index
                if (idx == 1) then
                    if (key /= 'baz' .or. val /= 'qux') then
                        write(error_unit, *) 'unexpected property: ', key, ' => ', val , ' /= baz => qux'
                        success = .false.
                    end if
                else if (idx == 2) then
                    if (key /= 'encoder' .or. val /= version_str) then
                        write(error_unit, *) 'unexpected property: ', key, ' => ', val , ' /= encoder => ', version_str
                        success = .false.
                    end if
                else
                    if (key /= 'foo' .or. val /= 'bar') then
                        write(error_unit, *) 'unexpected property: ', key, ' => ', val , ' /= foo => bar'
                        success = .false.
                    end if
                end if

                ! Check getting property values by key
                call check_call(frame%property('encoder', val), 'getting property by key', success)
                if (val /= version_str) then
                    write(error_unit, *) 'unexpected property value for encoder: ', val , ' /= ', version_str
                    success = .false.
                end if

                call check_call(frame%property('foo', val), 'getting property by key', success)
                if (val /= 'bar') then
                    write(error_unit, *) 'unexpected property value for foo: ', val , ' /= bar'
                    success = .false.
                end if

                call check_call(frame%property('baz', val), 'getting property by key', success)
                if (val /= 'qux') then
                    write(error_unit, *) 'unexpected property value for baz: ', val , ' /= qux'
                    success = .false.
                end if

                ! Check for reading of non-existent properties
                call check_call(frame%property('non-existent', exists=exists), 'getting property by key', success)
                if (exists) then
                    write(error_unit, *) 'unexpected non-existent property: ', exists, ' /= .false.'
                    success = .false.
                end if
            end do

            nframes = nframes + 1

            ! Advances to the next frame in the stream in aggregated mode
            err = frame%next(aggregated)
        end do

        if (err /= ODC_ITERATION_COMPLETE) call check_call(err, 'get next frame', success)

        call check_call(reader%close(), 'closing reader', success)

        ! Check number of frames
        if (nframes - 1 /= 1) then
            write(error_unit, *) 'unexpected number of frames:', nframes - 1, '/=', 1
            success = .false.
        end if

    end function

    function test_frame_properties_2_non_aggregated() result(success)

        ! Where the properties in the two frames overlap with entries that are the same (non-aggregated)

        type(odc_reader) :: reader
        type(odc_frame) :: frame
        integer :: err, nframes = 1, nproperties, idx
        logical :: aggregated = .false.
        character(:), allocatable, target :: version, key, val
        character(255) :: version_str
        logical :: success, exists
        success = .true.

        call test_generate_odb('properties-2.odb', 2, success)

        call check_call(reader%open_path('properties-2.odb'), 'opening path', success)
        call check_call(frame%initialise(reader), 'initialising frame', success)

        call check_call(odc_version(version), 'getting version number', success)
        write(version_str, *) 'odc version ', version
        version_str = trim(adjustl(version_str))

        ! Advance to the first frame in the stream in non-aggregated mode
        err = frame%next(aggregated)

        do while (err == ODC_SUCCESS)
            call check_call(frame%properties_count(nproperties), 'getting properties count', success)

            ! Check properties count
            if (nproperties /= 3) then
                write(error_unit, *) 'unexpected number of properties:', nproperties, '/=', 3
                success = .false.
            end if

            do idx = 1, 3
                call check_call(frame%property_idx(idx, key, val), 'getting property by index', success)

                ! Check getting properties by index
                if (idx == 1) then
                    if (key /= 'baz' .or. val /= 'qux') then
                        write(error_unit, *) 'unexpected property: ', key, ' => ', val , ' /= baz => qux'
                        success = .false.
                    end if
                else if (idx == 2) then
                    if (key /= 'encoder' .or. val /= version_str) then
                        write(error_unit, *) 'unexpected property: ', key, ' => ', val , ' /= encoder => ', version_str
                        success = .false.
                    end if
                else
                    if (key /= 'foo' .or. val /= 'bar') then
                        write(error_unit, *) 'unexpected property: ', key, ' => ', val , ' /= foo => bar'
                        success = .false.
                    end if
                end if

                ! Check getting property values by key
                call check_call(frame%property('encoder', val), 'getting property by key', success)
                if (val /= version_str) then
                    write(error_unit, *) 'unexpected property value for encoder: ', val , ' /= ', version_str
                    success = .false.
                end if

                call check_call(frame%property('foo', val), 'getting property by key', success)
                if (val /= 'bar') then
                    write(error_unit, *) 'unexpected property value for foo: ', val , ' /= bar'
                    success = .false.
                end if

                call check_call(frame%property('baz', val), 'getting property by key', success)
                if (val /= 'qux') then
                    write(error_unit, *) 'unexpected property value for baz: ', val , ' /= qux'
                    success = .false.
                end if

                ! Check for reading of non-existent properties
                call check_call(frame%property('non-existent', exists=exists), 'getting property by key', success)
                if (exists) then
                    write(error_unit, *) 'unexpected non-existent property: ', exists, ' /= .false.'
                    success = .false.
                end if
            end do

            nframes = nframes + 1

            ! Advances to the next frame in the stream in non-aggregated mode
            err = frame%next(aggregated)
        end do

        if (err /= ODC_ITERATION_COMPLETE) call check_call(err, 'get next frame', success)

        call check_call(reader%close(), 'closing reader', success)

        ! Check number of frames
        if (nframes - 1 /= 2) then
            write(error_unit, *) 'unexpected number of frames:', nframes - 1, '/=', 2
            success = .false.
        end if

    end function

    function test_frame_properties_2_aggregated() result(success)

        ! Where the properties in the two frames overlap with entries that are the same (aggregated)

        type(odc_reader) :: reader
        type(odc_frame) :: frame
        integer :: err, nframes = 1, nproperties, idx
        logical :: aggregated = .true.
        character(:), allocatable, target :: version, key, val
        character(255) :: version_str
        logical :: success, exists
        success = .true.

        call test_check_file_exists('properties-2.odb', success)

        call check_call(reader%open_path('properties-2.odb'), 'opening path', success)
        call check_call(frame%initialise(reader), 'initialising frame', success)

        call check_call(odc_version(version), 'getting version number', success)
        write(version_str, *) 'odc version ', version
        version_str = trim(adjustl(version_str))

        ! Advance to the first frame in the stream in aggregated mode
        err = frame%next(aggregated)

        do while (err == ODC_SUCCESS)
            call check_call(frame%properties_count(nproperties), 'getting properties count', success)

            ! Check properties count
            if (nproperties /= 3) then
                write(error_unit, *) 'unexpected number of properties:', nproperties, '/=', 3
                success = .false.
            end if

            do idx = 1, 3
                call check_call(frame%property_idx(idx, key, val), 'getting property by index', success)

                ! Check getting properties by index
                if (idx == 1) then
                    if (key /= 'baz' .or. val /= 'qux') then
                        write(error_unit, *) 'unexpected property: ', key, ' => ', val , ' /= baz => qux'
                        success = .false.
                    end if
                else if (idx == 2) then
                    if (key /= 'encoder' .or. val /= version_str) then
                        write(error_unit, *) 'unexpected property: ', key, ' => ', val , ' /= encoder => ', version_str
                        success = .false.
                    end if
                else
                    if (key /= 'foo' .or. val /= 'bar') then
                        write(error_unit, *) 'unexpected property: ', key, ' => ', val , ' /= foo => bar'
                        success = .false.
                    end if
                end if

                ! Check getting property values by key
                call check_call(frame%property('encoder', val), 'getting property by key', success)
                if (val /= version_str) then
                    write(error_unit, *) 'unexpected property value for encoder: ', val , ' /= ', version_str
                    success = .false.
                end if

                call check_call(frame%property('foo', val), 'getting property by key', success)
                if (val /= 'bar') then
                    write(error_unit, *) 'unexpected property value for foo: ', val , ' /= bar'
                    success = .false.
                end if

                call check_call(frame%property('baz', val), 'getting property by key', success)
                if (val /= 'qux') then
                    write(error_unit, *) 'unexpected property value for baz: ', val , ' /= qux'
                    success = .false.
                end if

                ! Check for reading of non-existent properties
                call check_call(frame%property('non-existent', exists=exists), 'getting property by key', success)
                if (exists) then
                    write(error_unit, *) 'unexpected non-existent property: ', exists, ' /= .false.'
                    success = .false.
                end if
            end do

            nframes = nframes + 1

            ! Advances to the next frame in the stream in aggregated mode
            err = frame%next(aggregated)
        end do

        if (err /= ODC_ITERATION_COMPLETE) call check_call(err, 'get next frame', success)

        call check_call(reader%close(), 'closing reader', success)

        ! Check number of frames
        if (nframes - 1 /= 1) then
            write(error_unit, *) 'unexpected number of frames:', nframes - 1, '/=', 1
            success = .false.
        end if

    end function

    function test_frame_properties_3_non_aggregated() result(success)

        ! Where the properties overlap with entries whose keys are the same, but the values different (non-aggregated)

        type(odc_reader) :: reader
        type(odc_frame) :: frame
        integer :: err, nframes = 1, nproperties, idx
        logical :: aggregated = .false.
        character(:), allocatable, target :: version, key, val
        character(255) :: version_str
        logical :: success, exists
        success = .true.

        call test_generate_odb('properties-3.odb', 3, success)

        call check_call(reader%open_path('properties-3.odb'), 'opening path', success)
        call check_call(frame%initialise(reader), 'initialising frame', success)

        call check_call(odc_version(version), 'getting version number', success)
        write(version_str, *) 'odc version ', version
        version_str = trim(adjustl(version_str))

        ! Advance to the first frame in the stream in non-aggregated mode
        err = frame%next(aggregated)

        do while (err == ODC_SUCCESS)
            call check_call(frame%properties_count(nproperties), 'getting properties count', success)

            ! Check properties count
            if (nproperties /= 2) then
                write(error_unit, *) 'unexpected number of properties:', nproperties, '/=', 2
                success = .false.
            end if

            do idx = 1, 2
                call check_call(frame%property_idx(idx, key, val), 'getting property by index', success)

                ! Check getting properties by index
                if (idx == 1) then
                    if (key /= 'encoder' .or. val /= version_str) then
                        write(error_unit, *) 'unexpected property: ', key, ' => ', val , ' /= encoder => ', version_str
                        success = .false.
                    end if
                else if (nframes == 1) then
                    if (key /= 'foo' .or. val /= 'bar') then
                        write(error_unit, *) 'unexpected property: ', key, ' => ', val , ' /= foo => bar'
                        success = .false.
                    end if
                else
                    if (key /= 'foo' .or. val /= 'baz') then
                        write(error_unit, *) 'unexpected property: ', key, ' => ', val , ' /= foo => baz'
                        success = .false.
                    end if
                end if

                ! Check getting property values by key
                if (nframes == 1) then
                    call check_call(frame%property('encoder', val), 'getting property by key', success)
                    if (val /= version_str) then
                        write(error_unit, *) 'unexpected property value for encoder: ', val , ' /= ', version_str
                        success = .false.
                    end if

                    call check_call(frame%property('foo', val), 'getting property by key', success)
                    if (val /= 'bar') then
                        write(error_unit, *) 'unexpected property value for foo: ', val , ' /= bar'
                        success = .false.
                    end if
                else
                    call check_call(frame%property('encoder', val), 'getting property by key', success)
                    if (val /= version_str) then
                        write(error_unit, *) 'unexpected property value for encoder: ', val , ' /= ', version_str
                        success = .false.
                    end if

                    call check_call(frame%property('foo', val), 'getting property by key', success)
                    if (val /= 'baz') then
                        write(error_unit, *) 'unexpected property value for foo: ', val , ' /= baz'
                        success = .false.
                    end if
                end if

                ! Check for reading of non-existent properties
                call check_call(frame%property('non-existent', exists=exists), 'getting property by key', success)
                if (exists) then
                    write(error_unit, *) 'unexpected non-existent property: ', exists, ' /= F'
                    success = .false.
                end if
            end do

            nframes = nframes + 1

            ! Advances to the next frame in the stream in non-aggregated mode
            err = frame%next(aggregated)
        end do

        if (err /= ODC_ITERATION_COMPLETE) call check_call(err, 'get next frame', success)

        call check_call(reader%close(), 'closing reader', success)

        ! Check number of frames
        if (nframes - 1 /= 2) then
            write(error_unit, *) 'unexpected number of frames:', nframes - 1, '/=', 2
            success = .false.
        end if

    end function

    function test_frame_properties_3_aggregated() result(success)

        ! Where the properties overlap with entries whose keys are the same, but the values different (aggregated)

        type(odc_reader) :: reader
        type(odc_frame) :: frame
        integer :: err, nframes = 1, nproperties, idx
        logical :: aggregated = .true.
        character(:), allocatable, target :: version, key, val
        character(255) :: version_str
        logical :: success, exists
        success = .true.

        call test_check_file_exists('properties-3.odb', success)

        call check_call(reader%open_path('properties-3.odb'), 'opening path', success)
        call check_call(frame%initialise(reader), 'initialising frame', success)

        call check_call(odc_version(version), 'getting version number', success)
        write(version_str, *) 'odc version ', version
        version_str = trim(adjustl(version_str))

        ! Advance to the first frame in the stream in aggregated mode
        err = frame%next(aggregated)

        do while (err == ODC_SUCCESS)
            call check_call(frame%properties_count(nproperties), 'getting properties count', success)

            ! Check properties count
            if (nproperties /= 2) then
                write(error_unit, *) 'unexpected number of properties:', nproperties, '/=', 2
                success = .false.
            end if

            do idx = 1, 2
                call check_call(frame%property_idx(idx, key, val), 'getting property by index', success)

                ! Check getting properties by index
                if (idx == 1) then
                    if (key /= 'encoder' .or. val /= version_str) then
                        write(error_unit, *) 'unexpected property: ', key, ' => ', val , ' /= encoder => ', version_str
                        success = .false.
                    end if
                else
                    ! Value from the first frame will win
                    if (key /= 'foo' .or. val /= 'bar') then
                        write(error_unit, *) 'unexpected property: ', key, ' => ', val , ' /= foo => bar'
                        success = .false.
                    end if
                end if

                ! Check getting property values by key
                call check_call(frame%property('encoder', val), 'getting property by key', success)
                if (val /= version_str) then
                    write(error_unit, *) 'unexpected property value for encoder: ', val , ' /= ', version_str
                    success = .false.
                end if

                call check_call(frame%property('foo', val), 'getting property by key', success)

                ! Value from the first frame will win
                if (val /= 'bar') then
                    write(error_unit, *) 'unexpected property value for foo: ', val , ' /= bar'
                    success = .false.
                end if

                ! Check for reading of non-existent properties
                call check_call(frame%property('non-existent', exists=exists), 'getting property by key', success)
                if (exists) then
                    write(error_unit, *) 'unexpected non-existent property: ', exists, ' /= .false.'
                    success = .false.
                end if
            end do

            nframes = nframes + 1

            ! Advances to the next frame in the stream in aggregated mode
            err = frame%next(aggregated)
        end do

        if (err /= ODC_ITERATION_COMPLETE) call check_call(err, 'get next frame', success)

        call check_call(reader%close(), 'closing reader', success)

        ! Check number of frames
        if (nframes - 1 /= 1) then
            write(error_unit, *) 'unexpected number of frames:', nframes - 1, '/=', 1
            success = .false.
        end if

    end function

    subroutine test_check_file_exists(path, success)
        character(*), intent(in) :: path
        logical, intent(inout) :: success

        inquire(file=path, exist=success)

        if (.not. success) then
            write(error_unit, *) 'unexpected missing file: ', path
        end if
    end subroutine

    subroutine test_generate_odb(path, properties_mode, success)
        character(*), intent(in) :: path
        integer, intent(in) :: properties_mode
        logical, intent(inout) :: success

        integer(8), parameter :: nrows = 10
        character(8), target :: data1(nrows)
        integer(8), target :: data2(nrows)
        real(8), target :: data3(nrows)

        character(4) :: expver_str = 'xxxx'
        integer(8) :: date = 20210401
        integer :: i

        type(odc_encoder) :: encoder
        integer, target :: outunit
        integer(8), target :: bytes_written

        ! Set treatment of integers as longs
        call check_call(odc_integer_behaviour(ODC_INTEGERS_AS_LONGS), 'setting integer behaviour to longs', success)

        ! Fill in the passed data arrays with scratch values
        do i = 1, nrows
            data1(i) = expver_str // c_null_char  ! expver
            data2(i) = date  ! date@hdr
            data3(i) = 12.3456 * (i - 1)  ! obsvalue@body
        end do

        ! Encode ODB-2 into a file
        open(newunit=outunit, file=path, access='stream', form='unformatted', status='replace')

        ! Encode two ODB-2 frames with the same data
        do i = 1, 2

            ! Initialise encoder
            call check_call(encoder%initialise(), 'initialising encoder', success)

            ! Set number of rows to allocate in the encoder
            call check_call(encoder%set_row_count(nrows), 'setting number of rows', success)

            ! Define all column names and their types
            call check_call(encoder%add_column('expver', ODC_STRING), 'adding expver column', success)
            call check_call(encoder%add_column('date@hdr', ODC_INTEGER), 'adding date@hdr column', success)
            call check_call(encoder%add_column('obsvalue@body', ODC_REAL), 'adding obsvalue@body column', success)

            ! Set a custom data layout and data array for each column
            call check_call(encoder%column_set_data_array(1, 8, stride=8, data=c_loc(data1)), 'setting expver array', success)
            call check_call(encoder%column_set_data_array(2, 8, stride=8, data=c_loc(data2)), 'setting date array', success)
            call check_call(encoder%column_set_data_array(3, 8, stride=8, data=c_loc(data3)), 'setting obsvalue array', success)

            ! Encode additional properties depending on the current mode

            select case(properties_mode)

                ! Where the properties in the two frames are distinct
                case (1)
                    if (i == 1) then
                        call check_call(encoder%add_property('foo', 'bar'), 'adding property', success)
                    else
                        call check_call(encoder%add_property('baz', 'qux'), 'adding property', success)
                    end if

                ! Where the properties in the two frames overlap with entries that are the same
                case (2)
                    call check_call(encoder%add_property('foo', 'bar'), 'adding property', success)
                    call check_call(encoder%add_property('baz', 'qux'), 'adding property', success)

                ! Where the properties overlap with entries whose keys are the same, but the values different
                case (3)
                    if (i == 1) then
                        call check_call(encoder%add_property('foo', 'bar'), 'adding property', success)
                    else
                        call check_call(encoder%add_property('foo', 'baz'), 'adding property', success)
                    end if

            end select

            call check_call(encoder%encode(outunit, bytes_written), 'do encode', success)

            ! Deallocate memory used up by the encoder
            call check_call(encoder%free(), 'cleaning up encoder', success)
        end do

        close(outunit)

    end subroutine

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
    success = test_frame_properties_1_non_aggregated() .and. success
    success = test_frame_properties_1_aggregated() .and. success
    success = test_frame_properties_2_non_aggregated() .and. success
    success = test_frame_properties_2_aggregated() .and. success
    success = test_frame_properties_3_non_aggregated() .and. success
    success = test_frame_properties_3_aggregated() .and. success

    if (.not. success) stop -1

end program
