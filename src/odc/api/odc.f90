
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
    end type

    type odc_encoder
    contains
    end type

    type odc_decode_target
    contains
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

        function odc_alloc_next_table(o, aggregated) result(frame) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: o
            logical(c_bool), intent(in), value :: aggregated
            type(c_ptr) :: frame
        end function

        ! Frame functionality

        subroutine odc_free_table(frame) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: frame
        end subroutine

        function odc_table_row_count(frame) result(nrows) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: frame
            integer(c_long) :: nrows
        end function

        function odc_table_column_count(frame) result(ncols) bind(c)
            use, intrinsic :: iso_c_binding
            implicit none
            type(c_ptr), intent(in), value :: frame
            integer(c_int) :: ncols
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

    function reader_next_frame(reader, frame, aggregated) result(success)
        class(odc_reader), intent(inout) :: reader
        type(odc_frame), intent(inout) :: frame
        logical, intent(in), optional :: aggregated
        logical(c_bool) :: l_aggregated = .false.
        logical :: success

        if (c_associated(frame%impl)) then
            call frame%free()
        endif

        if (present(aggregated)) then
            l_aggregated = aggregated
        end if

        frame%impl = odc_alloc_next_table(reader%impl, l_aggregated)
        success = c_associated(frame%impl)

    end function

    subroutine frame_free(frame)
        class(odc_frame), intent(inout) :: frame
        call odc_free_table(frame%impl)
        frame%impl = c_null_ptr
    end subroutine

    function frame_row_count(frame) result(nrows)
        class(odc_frame), intent(inout) :: frame
        integer(c_long) :: nrows
        nrows = odc_table_row_count(frame%impl)
    end function

    function frame_column_count(frame) result(ncols)
        class(odc_frame), intent(inout) :: frame
        integer :: ncols
        ncols = odc_table_column_count(frame%impl)
    end function

end module
