
module odc

    use, intrinsic :: iso_c_binding
    implicit none

    integer(c_int), public, parameter :: ODC_IGNORE = 0
    integer(c_int), public, parameter :: ODC_INTEGER = 1
    integer(c_int), public, parameter :: ODC_REAL = 2
    integer(c_int), public, parameter :: ODC_STRING = 3
    integer(c_int), public, parameter :: ODC_BITFIELD = 4
    integer(c_int), public, parameter :: ODC_DOUBLE = 5

    integer(c_int), public, parameter :: ODC_THROW = 1
    integer(c_int), public, parameter :: ODC_ERRORS_CHECKED = 2
    integer(c_int), public, parameter :: ODC_ERRORS_REPORT = 3

    private

    integer, parameter :: dp = selected_real_kind(15, 307)

    type odc_reader
        type(c_ptr) :: impl = c_null_ptr
    contains
        procedure :: open_path => reader_open_path
        procedure :: close => reader_close

        ! next_frame will clean up the frame object when called on the same object.
        ! If not, must call frame%free()
        procedure :: next_frame => reader_next_frame
    end type

    type odc_frame
        type(c_ptr) :: impl = c_null_ptr
    contains
        procedure :: free => frame_free
        procedure :: row_count => frame_row_count
        procedure :: column_count => frame_column_count
        procedure :: column_name => frame_column_name
        procedure :: column_type => frame_column_type
        procedure :: column_data_size => frame_column_data_size
        procedure :: column_data_size_doubles => frame_column_data_size_doubles
        procedure :: column_bitfield_count => frame_column_bitfield_count
        procedure :: column_bits_name => frame_column_bits_name
        procedure :: column_bits_size => frame_column_bits_size
        procedure :: column_bits_offset => frame_column_bits_offset
        procedure :: decode => frame_decode
    end type

    type odc_encoder
        type(c_ptr) :: impl = c_null_ptr
        real, pointer, dimension(:,:) :: data_array => null()
        logical :: column_major = .true.
    contains
        procedure :: initialise => encoder_initialise
        procedure :: free => encoder_free
        procedure :: set_row_count => encoder_set_row_count
        procedure :: set_rows_per_frame => encoder_set_rows_per_frame
        procedure :: set_data_array => encoder_set_data_array
        procedure :: add_column => encoder_add_column
        procedure :: column_set_size_doubles => encoder_column_set_size_doubles
        procedure :: column_set_stride => encoder_column_set_stride
        procedure :: column_set_data => encoder_column_set_data
        procedure :: column_add_bitfield_field => encoder_column_add_bitfield_field
        procedure :: encode => encoder_encode
    end type

    type odc_decode_target
        type(c_ptr) :: impl = c_null_ptr
    contains
        procedure :: initialise => dt_initialise
        procedure :: free => dt_free
        procedure :: set_row_count => dt_set_row_count
        procedure :: set_data_array => dt_set_data_array
        procedure :: data => dt_data_array
        procedure :: row_count => dt_row_count
        procedure :: column_count => dt_column_count
        procedure :: column_data_size_doubles => dt_column_data_size_doubles
    end type

    ! Type declarations

    public :: odc_reader
    public :: odc_frame
    public :: odc_encoder
    public :: odc_decode_target

    ! Configuration management functions

    public :: odc_version, odc_git_sha1
    public :: odc_initialise_api
    public :: odc_type_name, odc_type_count
    public :: odc_error_handling, odc_reset_error
    public :: odc_success, odc_error_string
    public :: odc_missing_integer, odc_missing_double
    public :: odc_set_missing_integer, odc_set_missing_double

    ! For utility

    interface
        pure function strlen(str) result(len) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: str
            integer(c_int) :: len
        end function
    end interface

    ! Wrap the C api functions

    interface

        pure function c_odc_version() result(pstr) bind(c, name='odc_version')
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr) :: pstr
        end function

        pure function c_odc_git_sha1() result(pstr) bind(c, name='odc_git_sha1')
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr) :: pstr
        end function

        subroutine odc_initialise_api() bind(c)
        end subroutine

        pure function c_odc_type_name(type) result(pstr) bind(c, name='odc_type_name')
            use, intrinsic :: iso_c_binding
            implicit none
            integer(c_int), intent(in), value :: type
            type(c_ptr) :: pstr
        end function

        pure function odc_type_count() result(ntypes) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            integer(c_int) :: ntypes
        end function

        subroutine odc_error_handling(handling_type) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            integer(c_int), intent(in), value :: handling_type
        end subroutine

        function c_odc_error_string() result(error_string) bind(c, name='odc_error_string')
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr) :: error_string
        end function

        subroutine odc_reset_error() bind(c)
        end subroutine

        pure function odc_missing_integer() result(missing_integer) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            integer(c_long) :: missing_integer
        end function

        pure function odc_missing_double() result(missing_double) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            real(c_double) :: missing_double
        end function

        subroutine odc_set_missing_integer(missing_integer) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            integer(c_long), intent(in), value :: missing_integer
        end subroutine

        subroutine odc_set_missing_double(missing_double) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            real(c_double), intent(in), value :: missing_double
        end subroutine

        function odc_success() result(successp) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            logical(c_bool) :: successp
        end function

        ! READ object api

        function odc_open_path(path) result(reader) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: path
            type(c_ptr) :: reader
        end function

        subroutine odc_close(o) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: o
        end subroutine

        function odc_alloc_next_frame(o) result(frame) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: o
            type(c_ptr) :: frame
        end function

        function odc_alloc_next_frame_aggregated(o, maximum_rows) result(frame) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: o
            integer(c_long), intent(in), value :: maximum_rows
            type(c_ptr) :: frame
        end function

        ! Frame functionality

        subroutine odc_free_frame(frame) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: frame
        end subroutine

        function odc_frame_row_count(frame) result(nrows) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: frame
            integer(c_long) :: nrows
        end function

        function odc_frame_column_count(frame) result(ncols) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: frame
            integer(c_int) :: ncols
        end function

        function odc_frame_column_name(frame, col) result(column_name) bind(c)
            ! n.b. 0-indexed column (C API)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: frame
            integer(c_int), intent(in), value :: col
            type(c_ptr) :: column_name
        end function

        function odc_frame_column_type(frame, col) result(column_type) bind(c)
            ! n.b. 0-indexed column (C API)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: frame
            integer(c_int), intent(in), value :: col
            integer(c_int) :: column_type
        end function

        function odc_frame_column_data_size(frame, col) result(element_size) bind(c)
            ! n.b. 0-indexed column (C API)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: frame
            integer(c_int), intent(in), value :: col
            integer(c_int) :: element_size
        end function

        function odc_frame_column_bitfield_count(frame, col) result(field_count) bind(c)
            ! n.b. 0-indexed column (C API)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: frame
            integer(c_int), intent(in), value :: col
            integer(c_int) :: field_count
        end function

        function odc_frame_column_bits_name(frame, col, field) result(bits_name) bind(c)
            ! n.b. 0-indexed column (C API)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: frame
            integer(c_int), intent(in), value :: col, field
            type(c_ptr) :: bits_name
        end function

        function odc_frame_column_bits_size(frame, col, field) result(bits_size) bind(c)
            ! n.b. 0-indexed column (C API)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: frame
            integer(c_int), intent(in), value :: col, field
            integer(c_int) :: bits_size
        end function

        function odc_frame_column_bits_offset(frame, col, field) result(bits_offset) bind(c)
            ! n.b. 0-indexed column (C API)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: frame
            integer(c_int), intent(in), value :: col, field
            integer(c_int) :: bits_offset
        end function

        subroutine odc_frame_build_all_decode_target(frame, target) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: frame, target
        end subroutine

        function odc_frame_decode(frame, decode_target, nthreads) result(row_count) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: frame, decode_target
            integer(c_int), intent(in), value :: nthreads
            integer(c_long) :: row_count
        end function

        ! Work with decode targets

        function odc_alloc_decode_target() result(decode_target) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr) :: decode_target
        end function

        subroutine odc_free_decode_target(decode_target) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: decode_target
        end subroutine

        subroutine odc_decode_target_set_row_count(decode_target, row_count) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: decode_target
            integer(c_long), intent(in), value :: row_count
        end subroutine

        subroutine odc_decode_target_set_array_data(decode_target, buffer, buffer_size) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: decode_target
            type(c_ptr), intent(in), value :: buffer
            integer(c_long), intent(in), value :: buffer_size
        end subroutine

        function odc_decode_target_array_data(decode_target) result(data) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: decode_target
            type(c_ptr) :: data
        end function

        pure function odc_decode_target_column_count(decode_target) result(column_count) bind(c)
            ! n.b. 0-indexed column (C API)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: decode_target
            integer(c_int) :: column_count
        end function

        pure function odc_decode_target_column_size(decode_target, column) result(column_size) bind(c)
            ! n.b. 0-indexed column (C API)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: decode_target
            integer(c_int), intent(in), value :: column
            integer(c_int) :: column_size
        end function

        pure function odc_decode_target_row_count(decode_target) result(row_count) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: decode_target
            integer(c_long) :: row_count
        end function

        ! Work with encoders

        function odc_alloc_encoder() result(encoder) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr) :: encoder
        end function

        subroutine odc_free_encoder(encoder) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: encoder
        end subroutine

        subroutine odc_encoder_set_row_count(encoder, row_count) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: encoder
            integer(c_long), intent(in), value :: row_count
        end subroutine

        subroutine odc_encoder_set_rows_per_frame(encoder, rows_per_frame) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: encoder
            integer(c_long), intent(in), value :: rows_per_frame
        end subroutine

        subroutine odc_encoder_set_data_array(encoder, data, column_major) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: encoder
            type(c_ptr), intent(in), value :: data
            logical(c_bool), intent(in), value :: column_major
        end subroutine

        function odc_encoder_add_column(encoder, name, type) result(column_number) bind(c)
            ! n.b. 0-indexed column (C API)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: encoder
            type(c_ptr), intent(in), value :: name
            integer(c_int), intent(in), value :: type
            integer(c_int) :: column_number
        end function

        subroutine odc_encoder_column_set_size(encoder, column, element_size) bind(c)
            ! n.b. 0-indexed column (C API)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: encoder
            integer(c_int), intent(in), value :: column
            integer(c_int), intent(in), value :: element_size
        end subroutine

        subroutine odc_encoder_column_set_stride(encoder, column, stride) bind(c)
            ! n.b. 0-indexed column (C API)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: encoder
            integer(c_int), intent(in), value :: column
            integer(c_int), intent(in), value :: stride
        end subroutine

        subroutine odc_encoder_column_set_data(encoder, column, data_ptr) bind(c)
            ! n.b. 0-indexed column (C API)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: encoder
            integer(c_int), intent(in), value :: column
            type(c_ptr), intent(in), value :: data_ptr
        end subroutine

        subroutine odc_encoder_column_add_bitfield_field(encoder, column, name, nbits) bind(c)
            ! n.b. 0-indexed column (C API)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: encoder
            integer(c_int), intent(in), value :: column
            type(c_ptr), intent(in), value :: name
            integer(c_int), intent(in), value :: nbits
        end subroutine

        function odc_encode_to_stream(encoder, handle, stream_fn) result(bytes_written) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: encoder
            type(c_ptr), intent(in), value :: handle
            type(c_funptr), intent(in), value :: stream_fn
            integer(c_long) :: bytes_written
        end function
    end interface

contains

    !function transfer_cstr(cstr, l) result(fstr)
    !    type(c_ptr), intent(in) :: cstr
    !    integer, intent(in) :: l
    !    character(l) :: fstr
    !    character(c_char), pointer :: tmp(:)
    !    call c_f_pointer(cstr, tmp, [l])
    !    fstr = transfer(tmp(1:l), fstr)
    !end function

    function fortranise_cstr(cstr) result(fstr)
        type(c_ptr), intent(in) :: cstr
        character(:), allocatable, target :: fstr
        character(c_char), pointer :: tmp(:)
        integer :: length

        length = strlen(cstr)
        allocate(character(length) :: fstr)
        call c_f_pointer(cstr, tmp, [length])
        fstr = transfer(tmp(1:length), fstr)
    end function

    function odc_version() result(version_str)
        character(:), allocatable :: version_str
        version_str = fortranise_cstr(c_odc_version())
    end function

    function odc_git_sha1() result(git_sha1)
        character(:), allocatable :: git_sha1
        git_sha1 = fortranise_cstr(c_odc_git_sha1())
    end function

    function odc_type_name(type) result(type_name)
        integer(c_int), intent(in) :: type
        character(:), allocatable :: type_name
        type_name = fortranise_cstr(c_odc_type_name(type))
    end function

    function odc_error_string() result(error_string)
        character(:), allocatable, target :: error_string
        error_string = fortranise_cstr(c_odc_error_string())
    end function

    ! Methods for reader objects

    subroutine reader_close(reader)
        class(odc_reader) :: reader
        call odc_close(reader%impl)
        reader%impl = c_null_ptr
    end subroutine

    subroutine reader_open_path(reader, path)
        class(odc_reader), intent(inout) :: reader
        character(*), intent(in) :: path
        character(:), allocatable, target :: nullified_path
        nullified_path = trim(path) // c_null_char
        reader%impl = odc_open_path(c_loc(nullified_path))
    end subroutine

    function reader_next_frame(reader, frame, aggregated, maximum_rows) result(success)
        class(odc_reader), intent(inout) :: reader
        type(odc_frame), intent(inout) :: frame
        logical, intent(in), optional :: aggregated
        integer, intent(in), optional :: maximum_rows
        logical :: l_aggregated = .false.
        integer(c_long) :: l_maximum_rows = -1
        logical :: success

        if (c_associated(frame%impl)) then
            call frame%free()
        endif

        if (present(aggregated)) then
            l_aggregated = aggregated
        end if

        if (present(maximum_rows)) then
            if (.not. present(aggregated)) then
                l_aggregated = .true.
            end if
            l_maximum_rows = maximum_rows
        end if

        if (l_aggregated) then
            frame%impl = odc_alloc_next_frame_aggregated(reader%impl, l_maximum_rows)
        else
            frame%impl = odc_alloc_next_frame(reader%impl)
        end if

        success = c_associated(frame%impl)

    end function

    subroutine frame_free(frame)
        class(odc_frame), intent(inout) :: frame
        if (c_associated(frame%impl)) then
            call odc_free_frame(frame%impl)
        end if
        frame%impl = c_null_ptr
    end subroutine

    function frame_row_count(frame) result(nrows)
        class(odc_frame), intent(in) :: frame
        integer(c_long) :: nrows
        nrows = odc_frame_row_count(frame%impl)
    end function

    function frame_column_count(frame) result(ncols)
        class(odc_frame), intent(in) :: frame
        integer :: ncols
        ncols = odc_frame_column_count(frame%impl)
    end function

    function frame_column_name(frame, col) result(column_name)
        ! n.b. 1-indexed column (Fortran API)
        class(odc_frame), intent(in) :: frame
        integer, intent(in) :: col
        character(:), allocatable :: column_name
        column_name = fortranise_cstr(odc_frame_column_name(frame%impl, col-1))
    end function

    function frame_column_type(frame, col) result(column_type)
        ! n.b. 1-indexed column (Fortran API)
        class(odc_frame), intent(in) :: frame
        integer, intent(in) :: col
        integer :: column_type
        column_type = odc_frame_column_type(frame%impl, col-1)
    end function

    function frame_column_data_size(frame, col) result(element_size)
        ! n.b. 1-indexed column (Fortran API)
        class(odc_frame), intent(in) :: frame
        integer, intent(in) :: col
        integer :: element_size
        element_size = odc_frame_column_data_size(frame%impl, col-1)
    end function

    function frame_column_data_size_doubles(frame, col) result(element_size)
        ! n.b. 1-indexed column (Fortran API)
        class(odc_frame), intent(in) :: frame
        integer, intent(in) :: col
        integer :: element_size
        element_size = odc_frame_column_data_size(frame%impl, col-1)
        element_size = merge(0, 1, mod(element_size, 8) == 0) + (element_size / 8)
    end function

    function frame_column_bitfield_count(frame, col) result(bitfield_count)
        ! n.b. 1-indexed column (Fortran API)
        class(odc_frame), intent(in) :: frame
        integer, intent(in) :: col
        integer :: bitfield_count
        bitfield_count = odc_frame_column_bitfield_count(frame%impl, col-1)
    end function

    function frame_column_bits_name(frame, col, field) result(bits_name)
        ! n.b. 1-indexed column (Fortran API)
        class(odc_frame), intent(in) :: frame
        integer, intent(in) :: col, field
        character(:), allocatable :: bits_name
        bits_name = fortranise_cstr(odc_frame_column_bits_name(frame%impl, col-1, field-1))
    end function

    function frame_column_bits_size(frame, col, field) result(bits_size)
        ! n.b. 1-indexed column (Fortran API)
        class(odc_frame), intent(in) :: frame
        integer, intent(in) :: col, field
        integer :: bits_size
        bits_size = odc_frame_column_bits_size(frame%impl, col-1, field-1)
    end function

    function frame_column_bits_offset(frame, col, field) result(bits_offset)
        ! n.b. 1-indexed column (Fortran API)
        class(odc_frame), intent(in) :: frame
        integer, intent(in) :: col, field
        integer :: bits_offset
        bits_offset = odc_frame_column_bits_offset(frame%impl, col-1, field-1)
    end function

    function frame_decode(frame, decode_target, nthreads) result(row_count)
        class(odc_frame), intent(inout) :: frame
        type(odc_decode_target), intent(inout) :: decode_target
        integer, intent(in), optional :: nthreads
        integer(c_long) :: row_count
        integer :: nthreads_ = 1
        if (present(nthreads)) nthreads_ = nthreads
        row_count = odc_frame_decode(frame%impl, decode_target%impl, nthreads_)
    end function

    subroutine dt_initialise(tgt, target_frame)
        class(odc_decode_target), intent(inout) :: tgt
        type(odc_frame), intent(in), optional :: target_frame
        tgt%impl = odc_alloc_decode_target()
        if (present(target_frame)) then
            call odc_frame_build_all_decode_target(target_frame%impl, tgt%impl)
        end if
    end subroutine

    subroutine dt_free(tgt)
        class(odc_decode_target), intent(inout) :: tgt
        call odc_free_decode_target(tgt%impl)
        tgt%impl = c_null_ptr
    end subroutine

    subroutine dt_set_row_count(tgt, row_count)
        class(odc_decode_target), intent(inout) :: tgt
        integer(c_long), intent(in) :: row_count
        call odc_decode_target_set_row_count(tgt%impl, row_count)
    end subroutine

    subroutine dt_set_data_array(tgt, data_array)
        class(odc_decode_target), intent(inout) :: tgt
        real(c_double), intent(inout), target :: data_array(:,:)
        integer(c_long) :: data_size
        call tgt%set_row_count(size(data_array, 1, c_long))
        data_size = 8 * size(data_array, 1) * size(data_array, 2)
        call odc_decode_target_set_array_data(tgt%impl, c_loc(data_array), data_size)
    end subroutine

    pure function dt_row_count(tgt) result(row_count)
        class(odc_decode_target), intent(in) :: tgt
        integer(c_long) :: row_count
        row_count = odc_decode_target_row_count(tgt%impl)
    end function

    pure function dt_column_count(tgt) result(column_count)
        class(odc_decode_target), intent(in) :: tgt
        integer(c_long) :: column_count
        column_count = odc_decode_target_column_count(tgt%impl)
    end function

    pure function dt_column_data_size_doubles(tgt, col) result(size_doubles)
        ! n.b. 1-indexed column (Fortran API)
        class(odc_decode_target), intent(in) :: tgt
        integer, intent(in) :: col
        integer :: size_doubles
        size_doubles = odc_decode_target_column_size(tgt%impl, col-1)
        size_doubles = merge(0, 1, mod(size_doubles, 8) == 0) + (size_doubles / 8)
    end function

    function dt_data_array(tgt) result(data)
        class(odc_decode_target), intent(inout) :: tgt
        type(c_ptr) :: c_data
        real(dp), pointer :: data(:,:)
        integer :: doubles_columns, col

        ! What are the dimensions of the array?
        doubles_columns = 0
        do col = 1, tgt%column_count()
            doubles_columns = doubles_columns + tgt%column_data_size_doubles(col)
        end do

        c_data = odc_decode_target_array_data(tgt%impl)
        call c_f_pointer(c_data, data, [int(tgt%row_count()), doubles_columns])
    end function

    ! Worker functions for the encoder

    subroutine encoder_initialise(encoder)
        class(odc_encoder), intent(inout) :: encoder
        encoder%impl = odc_alloc_encoder()
    end subroutine

    subroutine encoder_free(encoder)
        class(odc_encoder), intent(inout) :: encoder
        call odc_free_encoder(encoder%impl)
        encoder%impl = c_null_ptr
    end subroutine

    subroutine encoder_set_row_count(encoder, row_count)
        class(odc_encoder), intent(inout) :: encoder
        integer(c_long), intent(in) :: row_count
        call odc_encoder_set_row_count(encoder%impl, row_count)
    end subroutine

    subroutine encoder_set_rows_per_frame(encoder, nrows)
        class(odc_encoder), intent(inout) :: encoder
        integer(c_long), intent(in) :: nrows
        call odc_encoder_set_rows_per_frame(encoder%impl, nrows)
    end subroutine

    subroutine encoder_set_data_array(encoder, data_array, column_major)
        class(odc_encoder), intent(inout) :: encoder
        real(dp), intent(inout), target :: data_array(:,:)
        logical, intent(in), optional :: column_major

        if (present(column_major)) then
            encoder%column_major = column_major
        else
            encoder%column_major = .true.
        end if
        encoder%data_array => data_array
    end subroutine

    function encoder_add_column(encoder, name, type) result(column_index)
        ! n.b. 1-indexed column (Fortran API)
        class(odc_encoder), intent(inout) :: encoder
        character(*), intent(in) :: name
        integer, intent(in) :: type
        integer :: column_index
        character(:), allocatable, target :: cstr_name
        cstr_name = name // c_null_char
        column_index = 1 + odc_encoder_add_column(encoder%impl, c_loc(cstr_name), type)
    end function

    subroutine encoder_column_set_size_doubles(encoder, column, element_size)
        ! n.b. 1-indexed column (Fortran API)
        class(odc_encoder), intent(inout) :: encoder
        integer, intent(in) :: column, element_size
        call odc_encoder_column_set_size(encoder%impl, column-1, element_size*8)
    end subroutine

    subroutine encoder_column_set_stride(encoder, column, stride)
        ! n.b. 1-indexed column (Fortran API)
        class(odc_encoder), intent(inout) :: encoder
        integer, intent(in) :: column, stride
        call odc_encoder_column_set_stride(encoder%impl, column-1, stride)
    end subroutine

    subroutine encoder_column_set_data(encoder, column, data)
        ! n.b. 1-indexed column (Fortran API)
        class(odc_encoder), intent(inout) :: encoder
        integer, intent(in) :: column
        real(dp), intent(inout), target :: data(*)
        call odc_encoder_column_set_data(encoder%impl, column-1, c_loc(data))
    end subroutine

    subroutine encoder_column_add_bitfield_field(encoder, column, name, nbits)
        ! n.b. 1-indexed column (Fortran API)
        class(odc_encoder), intent(inout) :: encoder
        integer, intent(in) :: column
        character(*), intent(in) :: name
        integer, intent(in) :: nbits
        character(:), allocatable, target :: cstr_name
        cstr_name = name // c_null_char
        call odc_encoder_column_add_bitfield_field(encoder%impl, column-1, c_loc(cstr_name), nbits)
    end subroutine

    ! Helper function for streamage

    function write_fn(handle, buffer, length) result(written) bind(c)
        type(c_ptr), intent(in), value :: handle
        type(c_ptr), intent(in), value :: buffer
        integer(c_long), intent(in), value :: length
        integer(c_long) :: written
        integer, pointer :: fortran_unit
        character(c_char), pointer :: fortran_buffer(:)

        call c_f_pointer(handle, fortran_unit)
        call c_f_pointer(buffer, fortran_buffer, [length])
        write(fortran_unit) fortran_buffer
        written = length
    end function

    function encoder_encode(encoder, outunit) result(bytes_written)
        class(odc_encoder), intent(inout) :: encoder
        integer, intent(in), target :: outunit
        integer(c_long) :: bytes_written

        ! If we have set a data array, then set the details for the columns
        if (associated(encoder%data_array)) then
            if (encoder%column_major) then
                if (encoder%row_count() == 0) then
                    call encoder%set_row_count(size(encoder%data_array, 1, c_long))
                end if
            else
                if (encoder%row_count() == 0) then
                    call encoder%set_row_count(size(encoder%data_array, 2, c_long))
                end if
            end if
        end if

        bytes_written = odc_encode_to_stream(encoder%impl, c_loc(outunit), c_funloc(write_fn))

    end function

end module
