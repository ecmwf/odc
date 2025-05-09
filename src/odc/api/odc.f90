
module odc
    ! NOTE: The API reference of this code has been prepared, so any further changes to the public API must be manually
    ! documented. Please see <f90-reference.rst> in this repository for more information.

    use, intrinsic :: iso_c_binding
    implicit none

    integer(c_int), public, parameter :: ODC_IGNORE = 0
    integer(c_int), public, parameter :: ODC_INTEGER = 1
    integer(c_int), public, parameter :: ODC_REAL = 2
    integer(c_int), public, parameter :: ODC_STRING = 3
    integer(c_int), public, parameter :: ODC_BITFIELD = 4
    integer(c_int), public, parameter :: ODC_DOUBLE = 5

    ! Error values

    integer, public, parameter :: ODC_SUCCESS = 0
    integer, public, parameter :: ODC_ITERATION_COMPLETE = 1
    integer, public, parameter :: ODC_ERROR_GENERAL_EXCEPTION = 2
    integer, public, parameter :: ODC_ERROR_UNKNOWN_EXCEPTION = 3

    ! Integer behaviour values

    integer(c_int), public, parameter :: ODC_INTEGERS_AS_DOUBLES = 1  ! this is the default
    integer(c_int), public, parameter :: ODC_INTEGERS_AS_LONGS = 2

    private

    integer, parameter :: dp = selected_real_kind(15, 307)
    integer, parameter :: double_size = 8 !c_sizeof(1.0_dp) !intel compiler...
    integer, parameter :: int64 = selected_int_kind(15)

    type odc_reader
        type(c_ptr) :: impl = c_null_ptr
    contains
        procedure :: open_path => reader_open_path
        procedure :: close => reader_close
    end type

    type odc_frame
        type(c_ptr) :: impl = c_null_ptr
    contains
        procedure :: initialise => frame_initialise
        procedure :: free => frame_free
        procedure :: copy => frame_copy
        procedure :: next => frame_next
        procedure :: row_count => frame_row_count
        procedure :: column_count => frame_column_count
        procedure :: column_attributes => frame_column_attributes
        procedure :: bitfield_attributes => frame_bitfield_attributes
        procedure :: properties_count => frame_properties_count
        procedure :: property_idx => frame_property_idx
        procedure :: property => frame_property
    end type

    type odc_decoder
        type(c_ptr) :: impl = c_null_ptr
    contains
        procedure :: initialise => decoder_initialise
        procedure :: free => decoder_free
        procedure :: defaults_from_frame => decoder_defaults_from_frame
        procedure :: set_row_count => decoder_set_row_count
        procedure :: row_count => decoder_row_count
        procedure :: set_data => decoder_set_data_array
        procedure :: data => decoder_data_array
        procedure :: add_column => decoder_add_column
        procedure :: column_count => decoder_column_count
        procedure :: column_set_data_size => decoder_column_set_data_size
        procedure :: column_set_data_array => decoder_column_set_data_array
        procedure :: column_data_array => decoder_column_data_array
        procedure :: decode => decoder_decode
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
        procedure :: set_data => encoder_set_data_array
        procedure :: add_column => encoder_add_column
        procedure :: add_property => encoder_add_property
        procedure :: column_set_data_size => encoder_column_set_data_size
        procedure :: column_set_data_array => encoder_column_set_data_array
        procedure :: column_add_bitfield => encoder_column_add_bitfield
        procedure :: encode => encoder_encode
    end type

    ! Type declarations

    public :: odc_reader
    public :: odc_frame
    public :: odc_encoder
    public :: odc_decoder

    ! Configuration management functions

    public :: odc_version, odc_vcs_version
    public :: odc_initialise_api
    public :: odc_column_type_name, odc_column_type_count
    public :: odc_error_string
    public :: odc_missing_integer, odc_missing_double
    public :: odc_set_missing_integer, odc_set_missing_double
    public :: odc_set_failure_handler
    public :: odc_integer_behaviour

    ! Error handling definitions

    abstract interface
        subroutine failure_handler_t(context, error)
            implicit none
            integer, parameter :: int64 = selected_int_kind(15)
            integer(int64), intent(in) :: context
            integer, intent(in) :: error
        end subroutine
    end interface

    integer(int64), save :: failure_handler_context
    procedure(failure_handler_t), pointer, save :: failure_handler_fn

    ! For utility

    interface
        pure function strlen(str) result(len) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: str
            integer(c_long) :: len
        end function
    end interface

    ! Wrap the C api functions

    interface

        function c_odc_version(pstr) result(err) bind(c, name='odc_version')
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(out) :: pstr
            integer(c_int) :: err
        end function

        function c_odc_vcs_version(pstr) result(err) bind(c, name='odc_vcs_version')
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(out) :: pstr
            integer(c_int) :: err
        end function

        function odc_initialise_api() result(err) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            integer(c_int) :: err
        end function

        function odc_integer_behaviour(integer_behaviour) result(err) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            integer(c_int), intent(in), value :: integer_behaviour
            integer(c_int) :: err
        end function

        function c_odc_set_failure_handler(handler, context) result(err) bind(c, name='odc_set_failure_handler')
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_funptr), intent(in), value :: handler
            type(c_ptr), intent(in), value :: context
            integer(c_int) :: err
        end function

        function odc_halt_on_failure(halt) result(err) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            logical(c_bool), intent(in), value :: halt
            integer(c_int) :: err
        end function

        function c_odc_column_type_name(type, pstr) result(err) bind(c, name='odc_column_type_name')
            use, intrinsic :: iso_c_binding
            implicit none
            integer(c_int), intent(in), value :: type
            type(c_ptr), intent(out) :: pstr
            integer(c_int) :: err
        end function

        function odc_column_type_count(ntypes) result(err) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            integer(c_int), intent(out) :: ntypes
            integer(c_int) :: err
        end function

        function c_odc_error_string(err) result(error_string) bind(c, name='odc_error_string')
            use, intrinsic :: iso_c_binding
            implicit none
            integer(c_int), intent(in), value :: err
            type(c_ptr) :: error_string
        end function

        function odc_missing_integer(missing_integer) result(err) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            integer(c_long), intent(out) :: missing_integer
            integer(c_int) :: err
        end function

        function odc_missing_double(missing_double) result(err) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            real(c_double), intent(out) :: missing_double
            integer(c_int) :: err
        end function

        function odc_set_missing_integer(missing_integer) result(err) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            integer(c_long), intent(in), value :: missing_integer
            integer(c_int) :: err
        end function

        function odc_set_missing_double(missing_double) result(err) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            real(c_double), intent(in), value :: missing_double
            integer(c_int) :: err
        end function

        ! READ object api

        function odc_open_path(reader, path) result(err) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: path
            type(c_ptr), intent(out) :: reader
            integer(c_int) :: err
        end function

        function odc_close(reader) result(err) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: reader
            integer(c_int) :: err
        end function

        ! Frame functionality

        function odc_new_frame(frame, reader) result(err) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(out) :: frame
            type(c_ptr), intent(in), value :: reader
            integer(c_int) :: err
        end function

        function odc_free_frame(frame) result(err) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: frame
            integer(c_int) :: err
        end function

        function odc_next_frame(frame) result(err) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: frame
            integer(c_int) :: err
        end function

        function odc_next_frame_aggregated(frame, maximum_rows) result(err) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: frame
            integer(c_long), intent(in), value :: maximum_rows
            integer(c_int) :: err
        end function

        function odc_copy_frame(source_frame, copy) result(err) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: source_frame
            type(c_ptr), intent(out) :: copy
            integer(c_int) :: err
        end function

        function odc_frame_row_count(frame, count) result(err) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: frame
            integer(c_long), intent(out) :: count
            integer(c_int) :: err
        end function

        function odc_frame_column_count(frame, count) result(err) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: frame
            integer(c_int), intent(out) :: count
            integer(c_int) :: err
        end function

        function odc_frame_column_attributes(frame, col, name, type, element_size, bitfield_count) result(err) bind(c)
            ! n.b. 0-indexed column (C API)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: frame
            integer(c_int), intent(in), value :: col
            type(c_ptr), intent(out) :: name
            integer(c_int), intent(out) :: type
            integer(c_int), intent(out) :: element_size
            integer(c_int), intent(out) :: bitfield_count
            integer(c_int) :: err
        end function

        function odc_frame_bitfield_attributes(frame, col, field, name, offset, size) result(err) bind(c)
            ! n.b. 0-indexed column (C API)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: frame
            integer(c_int), intent(in), value :: col
            integer(c_int), intent(in), value :: field
            type(c_ptr), intent(out) :: name
            integer(c_int), intent(out) :: offset
            integer(c_int), intent(out) :: size
            integer(c_int) :: err
        end function

        function odc_frame_properties_count(frame, nproperties) result(err) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: frame
            integer(c_int), intent(out) :: nproperties
            integer(c_int) :: err
        end function

        function odc_frame_property_idx(frame, idx, key, val) result(err) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: frame
            integer(c_int), intent(in), value :: idx
            type(c_ptr), intent(out) :: key
            type(c_ptr), intent(out) :: val
            integer(c_int) :: err
        end function

        function odc_frame_property(frame, key, value) result(err) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: frame
            type(c_ptr), intent(in), value :: key
            type(c_ptr), intent(out) :: value
            integer(c_int) :: err
        end function

        ! Work with decoders

        function odc_new_decoder(decoder) result(err) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(out) :: decoder
            integer(c_int) :: err
        end function

        function odc_free_decoder(decoder) result(err) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: decoder
            integer(c_int) :: err
        end function

        function odc_decoder_defaults_from_frame(decoder, frame) result(err) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: decoder
            type(c_ptr), intent(in), value :: frame
            integer(c_int) :: err
        end function

        function odc_decoder_set_column_major(decoder, columnMajor) result(err) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: decoder
            logical(c_bool), intent(in), value :: columnMajor
            integer(c_int) :: err
        end function

        function odc_decoder_set_row_count(decoder, row_count) result(err) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: decoder
            integer(c_long), intent(in), value :: row_count
            integer(c_int) :: err
        end function

        function odc_decoder_row_count(decoder, row_count) result(err) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: decoder
            integer(c_long), intent(out) :: row_count
            integer(c_int) :: err
        end function

        function odc_decoder_set_data_array(decoder, data, width, height, columnMajor) result(err) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: decoder
            type(c_ptr), intent(in), value :: data
            integer(c_long), intent(in), value :: width
            integer(c_long), intent(in), value :: height
            logical(c_bool), intent(in), value :: columnMajor
            integer(c_int) :: err
        end function

        function odc_decoder_data_array(decoder, data, width, height, columnMajor) result(err) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: decoder
            type(c_ptr), intent(out) :: data
            integer(c_long), intent(out) :: width
            integer(c_long), intent(out) :: height
            logical(c_bool), intent(out) :: columnMajor
            integer(c_int) :: err
       end function

        function odc_decoder_add_column(decoder, name) result(err) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: decoder
            type(c_ptr), intent(in), value :: name
            integer(c_int) :: err
        end function

        function odc_decoder_column_count(decoder, count) result(err) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: decoder
            integer(c_int), intent(out) :: count
            integer(c_int) :: err
        end function

        function odc_decoder_column_set_data_size(decoder, col, element_size) result(err) bind(c)
            ! n.b. 0-indexed column (C API)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: decoder
            integer(c_int), intent(in), value :: col
            integer(c_int), intent(in), value :: element_size
            integer(c_int) :: err
        end function

        function odc_decoder_column_set_data_array(decoder, col, element_size, stride, data) result(err) bind(c)
            ! n.b. 0-indexed column (C API)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: decoder
            integer(c_int), intent(in), value :: col
            integer(c_int), intent(in), value :: element_size
            integer(c_int), intent(in), value :: stride
            type(c_ptr), intent(in), value :: data
            integer(c_int) :: err
        end function

        function odc_decoder_column_data_array(decoder, col, element_size, stride, data) result(err) bind(c)
            ! n.b. 0-indexed column (C API)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: decoder
            integer(c_int), intent(in), value :: col
            integer(c_int), intent(out) :: element_size
            integer(c_int), intent(out) :: stride
            type(c_ptr), intent(out) :: data
            integer(c_int) :: err
        end function

        ! Do actual decoding

        function odc_decode(decoder, frame, rows_decoded) result(err) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: decoder
            type(c_ptr), intent(in), value :: frame
            integer(c_long), intent(out) :: rows_decoded
            integer(c_int) :: err
        end function

        function odc_decode_threaded(decoder, frame, rows_decoded, nthreads) result(err) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: decoder
            type(c_ptr), intent(in), value :: frame
            integer(c_long), intent(out) :: rows_decoded
            integer(c_int), intent(in), value :: nthreads
            integer(c_int) :: err
        end function

        ! Work with encoders

        function odc_new_encoder(encoder) result(err) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(out) :: encoder
            integer(c_int) :: err
        end function

        function odc_free_encoder(encoder) result(err) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: encoder
            integer(c_int) :: err
        end function

        function odc_encoder_set_row_count(encoder, row_count) result(err) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: encoder
            integer(c_long), intent(in), value :: row_count
            integer(c_int) :: err
        end function

        function odc_encoder_set_rows_per_frame(encoder, rows_per_frame) result(err) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: encoder
            integer(c_long), intent(in), value :: rows_per_frame
            integer(c_int) :: err
        end function

        function odc_encoder_set_data_array(encoder, data, width, height, columnMajorWidth) result(err) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: encoder
            type(c_ptr), intent(in), value :: data
            integer(c_long), intent(in), value :: width
            integer(c_long), intent(in), value :: height
            integer(c_int), intent(in), value :: columnMajorWidth
            integer(c_int) :: err
        end function

        function odc_encoder_add_column(encoder, name, type) result(err) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: encoder
            type(c_ptr), intent(in), value :: name
            integer(c_int), intent(in), value :: type
            integer(c_int) :: err
        end function

        function odc_encoder_add_property(encoder, key, val) result(err) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: encoder
            type(c_ptr), intent(in), value :: key
            type(c_ptr), intent(in), value :: val
            integer(c_int) :: err
        end function

        function odc_encoder_column_set_data_size(encoder, col, element_size) result(err) bind(c)
            ! n.b. 0-indexed column (C API)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: encoder
            integer(c_int), intent(in), value :: col
            integer(c_int), intent(in), value :: element_size
            integer(c_int) :: err
        end function

        function odc_encoder_column_set_data_array(encoder, col, element_size, stride, data) result(err) bind(c)
            ! n.b. 0-indexed column (C API)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: encoder
            integer(c_int), intent(in), value :: col
            integer(c_int), intent(in), value :: element_size
            integer(c_int), intent(in), value :: stride
            type(c_ptr), intent(in), value :: data
            integer(c_int) :: err
        end function

        function odc_encoder_column_add_bitfield(encoder, col, name, nbits) result(err) bind(c)
            ! n.b. 0-indexed column (C API)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: encoder
            integer(c_int), intent(in), value :: col
            type(c_ptr), intent(in), value :: name
            integer(c_int), intent(in), value :: nbits
            integer(c_int) :: err
        end function

        function odc_encode_to_stream(encoder, handle, stream_fn, bytes_encoded) result(err) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: encoder
            type(c_ptr), intent(in), value :: handle
            type(c_funptr), intent(in), value :: stream_fn
            integer(c_long), intent(out) :: bytes_encoded
            integer(c_int) :: err
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

    subroutine failure_handler_wrapper(unused_context, error) bind(c)
        type(c_ptr), intent(in), value :: unused_context
        integer(c_long), intent(in), value :: error
        call failure_handler_fn(failure_handler_context, int(error))
    end subroutine

    function odc_set_failure_handler(handler, context) result(err)
        integer(int64) :: context
        integer :: err

        interface
            subroutine handler (ctx, err)
                implicit none
                integer, parameter :: int64 = selected_int_kind(15)
                integer(int64), intent(in) :: ctx
                integer, intent(in) :: err
            end subroutine
        end interface

        failure_handler_fn => handler
        failure_handler_context = context
        err = c_odc_set_failure_handler(c_funloc(failure_handler_wrapper), c_null_ptr)
    end function

    function odc_version(version_str) result(err)
        character(:), allocatable, intent(out) :: version_str
        type(c_ptr) :: tmp_str
        integer :: err
        err = c_odc_version(tmp_str)
        if (err == ODC_SUCCESS) version_str = fortranise_cstr(tmp_str)
    end function

    function odc_vcs_version(git_sha1) result(err)
        character(:), allocatable, intent(out) :: git_sha1
        type(c_ptr) :: tmp_str
        integer :: err
        err = c_odc_vcs_version(tmp_str)
        if (err == ODC_SUCCESS) git_sha1 = fortranise_cstr(tmp_str)
    end function

    function odc_column_type_name(type, type_name) result(err)
        integer(c_int), intent(in) :: type
        character(:), allocatable, intent(out) :: type_name
        type(c_ptr) :: tmp_str
        integer :: err
        err = c_odc_column_type_name(type, tmp_str)
        if (err == ODC_SUCCESS)  type_name = fortranise_cstr(tmp_str)
    end function

    function odc_error_string(err) result(error_string)
        integer, intent(in) :: err
        character(:), allocatable, target :: error_string
        error_string = fortranise_cstr(c_odc_error_string(err))
    end function

    ! Methods for reader objects

    function reader_open_path(reader, path) result(err)
        class(odc_reader), intent(inout) :: reader
        character(*), intent(in) :: path
        integer :: err
        character(:), allocatable, target :: nullified_path
        nullified_path = trim(path) // c_null_char
        err = odc_open_path(reader%impl, c_loc(nullified_path))
    end function

    function reader_close(reader) result(err)
        class(odc_reader), intent(inout) :: reader
        integer :: err
        err = odc_close(reader%impl)
        reader%impl = c_null_ptr
    end function

    ! Methods for frame object

    function frame_initialise(frame, reader) result(err)
        class(odc_frame), intent(inout) :: frame
        type(odc_reader), intent(inout) :: reader
        integer :: err
        err = odc_new_frame(frame%impl, reader%impl)
    end function

    function frame_free(frame) result(err)
        class(odc_frame), intent(inout) :: frame
        integer :: err
        err = odc_free_frame(frame%impl)
    end function

    function frame_copy(frame, new_frame) result(err)
        class(odc_frame), intent(inout) :: frame
        class(odc_frame), intent(inout) :: new_frame
        integer :: err
        err = odc_copy_frame(frame%impl, new_frame%impl)
    end function

    function frame_next(frame, aggregated, maximum_rows) result(err)
        class(odc_frame), intent(inout) :: frame
        logical, intent(in), optional :: aggregated
        integer(c_long), intent(in), optional :: maximum_rows
        integer :: err

        integer(c_long) :: l_maximum_rows = -1
        logical :: l_aggregated = .false.

        if (present(aggregated)) l_aggregated = aggregated
        if (present(maximum_rows)) then
            l_maximum_rows = maximum_rows
            if (.not. present(aggregated)) l_aggregated = .true.
        end if

        if (l_aggregated) then
            err = odc_next_frame_aggregated(frame%impl, l_maximum_rows)
        else
            err = odc_next_frame(frame%impl)
        end if
    end function

    function frame_row_count(frame, nrows) result(err)
        class(odc_frame), intent(in) :: frame
        integer(c_long), intent(out) :: nrows
        integer :: err
        err = odc_frame_row_count(frame%impl, nrows)
    end function

    function frame_column_count(frame, ncols) result(err)
        class(odc_frame), intent(in) :: frame
        integer(c_int), intent(out) :: ncols
        integer :: err
        err = odc_frame_column_count(frame%impl, ncols)
    end function

    function frame_column_attributes(frame, col, name, type, element_size, element_size_doubles, bitfield_count) result(err)
        ! n.b. 1-indexed column (Fortran API)
        class(odc_frame), intent(in) :: frame
        integer, intent(in) :: col
        integer :: err

        character(:), allocatable, intent(out), optional :: name
        integer, intent(out), optional :: type
        integer, intent(out), optional :: element_size
        integer, intent(out), optional :: element_size_doubles
        integer, intent(out), optional :: bitfield_count

        type(c_ptr) :: name_tmp
        integer(c_int) :: type_tmp
        integer(c_int) :: element_size_tmp
        integer(c_int) :: bitfield_count_tmp

        err = odc_frame_column_attributes(frame%impl, col-1, name_tmp, type_tmp,&
                                     element_size_tmp, bitfield_count_tmp)

        if (err == ODC_SUCCESS) then
            if (present(name)) name = fortranise_cstr(name_tmp)
            if (present(type)) type = type_tmp
            if (present(element_size)) element_size = element_size_tmp
            if (present(element_size_doubles)) element_size_doubles = element_size_tmp / double_size
            if (present(bitfield_count)) bitfield_count = bitfield_count_tmp
        end if

    end function

    function frame_bitfield_attributes(frame, col, field, name, offset, size) result(err)
        ! n.b. 1-indexed column (Fortran API)
        class(odc_frame), intent(in) :: frame
        integer, intent(in) :: col
        integer, intent(in) :: field
        integer :: err

        character(:), allocatable, intent(out), optional :: name
        integer, intent(out), optional :: offset
        integer, intent(out), optional :: size

        type(c_ptr) :: name_tmp
        integer(c_int) :: offset_tmp
        integer(c_int) :: size_tmp

        err = odc_frame_bitfield_attributes(frame%impl, col-1, field-1, name_tmp, offset_tmp, size_tmp)

        if (err == ODC_SUCCESS) then
            if (present(name)) name = fortranise_cstr(name_tmp)
            if (present(offset)) offset = offset_tmp
            if (present(size)) size = size_tmp
        end if

    end function

    function frame_properties_count(frame, nproperties) result(err)
        class(odc_frame), intent(in) :: frame
        integer, intent(out) :: nproperties
        integer :: err

        integer(c_int) :: nproperties_tmp

        err = odc_frame_properties_count(frame%impl, nproperties_tmp)

        if (err == ODC_SUCCESS) then
            nproperties = nproperties_tmp
        end if

    end function

    function frame_property_idx(frame, idx, key, val) result(err)
        class(odc_frame), intent(in) :: frame
        integer, intent(in) :: idx
        character(:), allocatable, intent(out) :: key
        character(:), allocatable, intent(out) :: val
        integer :: err

        type(c_ptr) :: key_tmp
        type(c_ptr) :: val_tmp

        err = odc_frame_property_idx(frame%impl, idx-1, key_tmp, val_tmp)

        if (err == ODC_SUCCESS) then
            key = fortranise_cstr(key_tmp)
            val = fortranise_cstr(val_tmp)
        end if

    end function

    function frame_property(frame, key, val, exists) result(err)
        class(odc_frame), intent(in) :: frame
        character(*), intent(in) :: key
        character(:), allocatable, intent(out), optional :: val
        logical, intent(out), optional :: exists
        integer :: err

        character(:), allocatable, target :: nullified_key
        type(c_ptr) :: val_tmp

        nullified_key = trim(key) // c_null_char

        err = odc_frame_property(frame%impl, c_loc(nullified_key), val_tmp)

        if (err == ODC_SUCCESS) then
            if (c_associated(val_tmp)) then
                if (present(val)) val = fortranise_cstr(val_tmp)
                if (present(exists)) exists = .true.
            else
                if (present(exists)) exists = .false.
            end if
        end if

    end function

    ! Methods for decoder object

    function decoder_initialise(decoder, column_major) result(err)
        class(odc_decoder), intent(inout) :: decoder
        logical, intent(in), optional :: column_major
        integer :: err
        logical(c_bool) :: l_column_major = .true.
        if (present(column_major)) l_column_major = column_major
        err = odc_new_decoder(decoder%impl)
        if (err == ODC_SUCCESS) then
            err = odc_decoder_set_column_major(decoder%impl, l_column_major)
        end if
    end function

    function decoder_free(decoder) result(err)
        class(odc_decoder), intent(inout) :: decoder
        integer :: err
        err = odc_free_decoder(decoder%impl)
    end function

    function decoder_defaults_from_frame(decoder, frame) result(err)
        class(odc_decoder), intent(inout) :: decoder
        type(odc_frame), intent(in) :: frame
        integer :: err
        err = odc_decoder_defaults_from_frame(decoder%impl, frame%impl)
    end function

    function decoder_set_row_count(decoder, count) result(err)
        class(odc_decoder), intent(inout) :: decoder
        integer(c_long), intent(in) :: count
        integer :: err
        err = odc_decoder_set_row_count(decoder%impl, count)
    end function

    function decoder_row_count(decoder, count) result(err)
        class(odc_decoder), intent(in) :: decoder
        integer(c_long), intent(out) :: count
        integer :: err
        err = odc_decoder_row_count(decoder%impl, count)
    end function

    function decoder_set_data_array(decoder, data, column_major) result(err)
        class(odc_decoder), intent(inout) :: decoder
        real(dp), intent(inout), target :: data(:,:)
        logical, intent(in), optional :: column_major
        integer(c_long) :: width, height
        logical(c_bool) :: l_column_major = .true.
        integer :: err
        if (present(column_major)) l_column_major = column_major
        if (l_column_major) then
            width = size(data, 2) * double_size
            height = size(data, 1)
        else
            width = size(data, 1) * double_size
            height = size(data, 2)
        end if
        err = odc_decoder_set_data_array(decoder%impl, c_loc(data), width, height, l_column_major)
    end function

    function decoder_data_array(decoder, data, column_major) result(err)
        class(odc_decoder), intent(in) :: decoder
        real(dp), intent(inout), pointer, optional :: data(:,:)
        logical, intent(out), optional :: column_major
        integer :: err

        type(c_ptr) :: cdata
        integer(c_long) :: width, height
        logical(c_bool) :: cmajor

        err = odc_decoder_data_array(decoder%impl, cdata, width, height, cmajor)

        if (err == ODC_SUCCESS) then
            if (present(data)) then
                if (cmajor) then
                    call c_f_pointer(cdata, data, [height, width / double_size])
                else
                    call c_f_pointer(cdata, data, [width / double_size, height])
                end if
            end if
            if (present(column_major)) column_major = cmajor
        end if

    end function

    function decoder_add_column(decoder, name) result(err)
        class(odc_decoder), intent(inout) :: decoder
        character(*), intent(in) :: name
        character(:), allocatable, target :: nullified_name
        integer :: err

        nullified_name = trim(name) // c_null_char
        err = odc_decoder_add_column(decoder%impl, c_loc(nullified_name))
    end function

    function decoder_column_count(decoder, count) result(err)
        class(odc_decoder), intent(in) :: decoder
        integer, intent(out) :: count
        integer(c_int) :: count_tmp
        integer :: err
        err = odc_decoder_column_count(decoder%impl, count_tmp)
        count = count_tmp
    end function

    function decoder_column_set_data_size(decoder, col, element_size) result(err)
        ! n.b. 1-indexed column (Fortran API)
        class(odc_decoder), intent(inout) :: decoder
        integer, intent(in) :: col
        integer(c_int), intent(in) :: element_size
        integer :: err

        err = odc_decoder_column_set_data_size(decoder%impl, col-1, element_size)
    end function

    function decoder_column_set_data_array(decoder, col, element_size, stride, data) result(err)
        ! n.b. 1-indexed column (Fortran API)
        class(odc_decoder), intent(inout) :: decoder
        integer, intent(in) :: col
        integer, intent(in), optional :: element_size
        integer, intent(in), optional :: stride
        type(c_ptr), intent(in), optional :: data
        integer :: err

        integer(c_int) :: l_element_size = 0
        integer(c_int) :: l_stride = 0
        type(c_ptr) :: l_data = c_null_ptr
        if (present(element_size)) l_element_size = element_size
        if (present(stride)) l_stride = stride
        if (present(data)) l_data = data

        err = odc_decoder_column_set_data_array(decoder%impl, col-1, l_element_size, l_stride, l_data)
    end function

    function decoder_column_data_array(decoder, col, element_size, element_size_doubles, stride, data) result(err)
        ! n.b. 1-indexed column (Fortran API)
        class(odc_decoder), intent(in) :: decoder
        integer, intent(in) :: col
        integer, intent(out), optional :: element_size
        integer, intent(out), optional :: element_size_doubles
        integer, intent(out), optional :: stride
        type(c_ptr), intent(out), optional :: data
        integer :: err

        integer(c_int) :: l_element_size
        integer(c_int) :: l_stride
        type(c_ptr) :: l_data

        err = odc_decoder_column_data_array(decoder%impl, col-1, l_element_size, l_stride, l_data)

        if (err == ODC_SUCCESS) then
            if (present(element_size)) element_size = l_element_size
            if (present(element_size_doubles)) element_size_doubles = l_element_size / double_size
            if (present(stride)) stride = l_stride
            if (present(data)) data = l_data
        end if
    end function

    function decoder_decode(decoder, frame, rows_decoded, nthreads) result(err)
        class(odc_decoder), intent(inout) :: decoder
        class(odc_frame), intent(inout) :: frame
        integer(c_long), intent(out) :: rows_decoded
        integer, intent(in), optional :: nthreads
        integer :: err

        if (present(nthreads)) then
            err = odc_decode_threaded(decoder%impl, frame%impl, rows_decoded, nthreads)
        else
            err = odc_decode(decoder%impl, frame%impl, rows_decoded)
        end if
    end function

    ! Methods for the encoder

    function encoder_initialise(encoder) result(err)
        class(odc_encoder), intent(inout) :: encoder
        integer :: err
        err = odc_new_encoder(encoder%impl)
    end function

    function encoder_free(encoder) result(err)
        class(odc_encoder), intent(inout) :: encoder
        integer :: err
        err = odc_free_encoder(encoder%impl)
    end function

    function encoder_set_row_count(encoder, row_count) result(err)
        class(odc_encoder), intent(inout) :: encoder
        integer(c_long), intent(in) :: row_count
        integer :: err
        err = odc_encoder_set_row_count(encoder%impl, row_count)
    end function

    function encoder_set_rows_per_frame(encoder, rows_per_frame) result(err)
        class(odc_encoder), intent(inout) :: encoder
        integer(c_long), intent(in) :: rows_per_frame
        integer :: err
        err = odc_encoder_set_rows_per_frame(encoder%impl, rows_per_frame)
    end function

    function encoder_set_data_array(encoder, data, column_major) result(err)
        class(odc_encoder), intent(inout) :: encoder
        real(dp), intent(in), target :: data(:,:)
        logical, intent(in), optional :: column_major
        integer(c_long) :: width, height
        integer(c_int) :: columnMajorWidth
        integer :: err
        logical(c_bool) :: l_column_major = .true.

        if (present(column_major)) l_column_major = column_major
        if (l_column_major) then
            width = size(data, 2) * double_size
            height = size(data, 1)
            columnMajorWidth = 8
        else
            width = size(data, 1) * double_size
            height = size(data, 2)
            columnMajorWidth = 0
        end if
        err = odc_encoder_set_data_array(encoder%impl, c_loc(data), width, height, columnMajorWidth)
    end function

    function encoder_add_column(encoder, name, type) result(err)
        class(odc_encoder), intent(inout) :: encoder
        character(*), intent(in) :: name
        integer, intent(in) :: type
        integer :: err
        character(:), allocatable, target :: nullified_name
        nullified_name = trim(name) // c_null_char
        err = odc_encoder_add_column(encoder%impl, c_loc(nullified_name), type)
    end function

    function encoder_add_property(encoder, key, val) result(err)
        class(odc_encoder), intent(inout) :: encoder
        character(*), intent(in) :: key
        character(*), intent(in) :: val
        integer :: err
        character(:), allocatable, target :: nullified_key
        character(:), allocatable, target :: nullified_val
        nullified_key = trim(key) // c_null_char
        nullified_val = trim(val) // c_null_char
        err = odc_encoder_add_property(encoder%impl, c_loc(nullified_key), c_loc(nullified_val))
    end function

    function encoder_column_set_data_size(encoder, col, element_size, element_size_doubles) result(err)
        ! n.b. 1-indexed column (Fortran API)
        class(odc_encoder), intent(inout) :: encoder
        integer, intent(in) :: col
        integer, intent(in), optional :: element_size
        integer, intent(in), optional :: element_size_doubles
        integer :: err


        integer(c_int) :: l_element_size = 0
        if (present(element_size)) l_element_size = element_size
        if (present(element_size_doubles)) l_element_size = element_size_doubles * double_size

        err = odc_encoder_column_set_data_size(encoder%impl, col-1, l_element_size)
    end function

    function encoder_column_set_data_array(encoder, col, element_size, element_size_doubles, stride, data) result(err)
        ! n.b. 1-indexed column (Fortran API)
        class(odc_encoder), intent(inout) :: encoder
        integer, intent(in) :: col
        integer, intent(in), optional :: element_size
        integer, intent(in), optional :: element_size_doubles
        integer, intent(in), optional :: stride
        type(c_ptr), intent(in), optional :: data
        integer :: err

        integer(c_int) :: l_element_size = 0
        integer(c_int) :: l_stride = 0
        type(c_ptr) :: l_data = c_null_ptr
        if (present(element_size)) l_element_size = element_size
        if (present(element_size_doubles)) l_element_size = element_size_doubles * double_size
        if (present(stride)) l_stride = stride
        if (present(data)) l_data = data

        err = odc_encoder_column_set_data_array(encoder%impl, col-1, l_element_size, l_stride, l_data)
    end function

    function encoder_column_add_bitfield(encoder, col, name, nbits) result(err)
        ! n.b. 1-indexed column (Fortran API)
        class(odc_encoder), intent(inout) :: encoder
        integer, intent(in) :: col
        character(*), intent(in) :: name
        integer, intent(in) :: nbits
        integer :: err
        character(:), allocatable, target :: nullified_name
        nullified_name = trim(name) // c_null_char
        err = odc_encoder_column_add_bitfield(encoder%impl, col-1, c_loc(nullified_name), nbits)
    end function

    ! Helper function for streamage

    function write_fn(context, buffer, length) result(written) bind(c)
        type(c_ptr), intent(in), value :: context
        type(c_ptr), intent(in), value :: buffer
        integer(c_long), intent(in), value :: length
        integer(c_long) :: written
        integer, pointer :: fortran_unit
        character(c_char), pointer :: fortran_buffer(:)

        call c_f_pointer(context, fortran_unit)
        call c_f_pointer(buffer, fortran_buffer, [length])
        write(fortran_unit) fortran_buffer
        written = length
    end function

    function encoder_encode(encoder, outunit, bytes_written) result(err)
        class(odc_encoder), intent(inout) :: encoder
        integer, intent(in), target :: outunit
        integer(c_long), intent(out) :: bytes_written
        integer :: err

        err = odc_encode_to_stream(encoder%impl, c_loc(outunit), c_funloc(write_fn), bytes_written)

    end function

end module
