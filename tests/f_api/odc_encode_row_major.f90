! To build this program, please make sure to first compile odc Fortran module,
! and then reference linked libraries:
!
!     gfortran -c ../../src/odc/api/odc.f90
!     gfortran -lodccore -lfodc -o odc-fortran-encode-row-major odc_encode_row_major.f90

program odc_ls
    use, intrinsic :: iso_c_binding, only: c_null_char
    use odc
    implicit none

    character(255) :: path

    integer, parameter :: ncols = 9
    integer(8), parameter :: nrows = 20
    real(8), target :: data(ncols, nrows)

    type(odc_encoder) :: encoder
    logical, parameter :: column_major = .false.
    character(10), parameter :: property_key = 'encoded_by'
    character(11), parameter :: property_value = 'odc_example'
    integer, target :: outunit
    integer(8), target :: bytes_written

    if (command_argument_count() /= 1) then
        call usage()
        stop 1
    end if

    ! Get output path from command argument
    call get_command_argument(1, path)

    ! Initialise API and set treatment of integers as longs
    call check_call(odc_initialise_api(), 'initialising api')
    call check_call(odc_integer_behaviour(ODC_INTEGERS_AS_LONGS), 'setting integer behaviour to longs')

    ! Set up the allocated array with scratch data
    call create_scratch_data(data, nrows)

    ! Initialise encoder
    call check_call(encoder%initialise(), 'initialising encoder')

    ! Define all column names and their types
    call check_call(encoder%add_column('expver', ODC_STRING), 'adding expver column')
    call check_call(encoder%add_column('date@hdr', ODC_INTEGER), 'adding date@hdr column')
    call check_call(encoder%add_column('statid@hdr', ODC_STRING), 'adding statid@hdr column')
    call check_call(encoder%add_column('wigos@hdr', ODC_STRING), 'adding wigos@hdr column')
    call check_call(encoder%add_column('obsvalue@body', ODC_REAL), 'adding obsvalue@bod column')
    call check_call(encoder%add_column('integer_missing', ODC_INTEGER), 'adding integer_missing column')
    call check_call(encoder%add_column('double_missing', ODC_REAL), 'adding double_missing column')
    call check_call(encoder%add_column('bitfield_column', ODC_BITFIELD), 'adding bitfield_column')

    ! Column `wigos@hdr` is a 16-byte string column, hence takes 2 columns in the array => ncols=9
    call check_call(encoder%column_set_data_size(4, element_size_doubles=2), 'setting column data size')

    ! Column `bitfield_column` is an integer with 4 bitfield values in it
    call check_call(encoder%column_add_bitfield(8, "flag_a", 1), 'adding flag_a bitfield')
    call check_call(encoder%column_add_bitfield(8, "flag_b", 2), 'adding flag_b bitfield')
    call check_call(encoder%column_add_bitfield(8, "flag_c", 3), 'adding flag_c bitfield')
    call check_call(encoder%column_add_bitfield(8, "flag_d", 1), 'adding flag_d bitfield')

    ! Set input data array from which data will be encoded
    call check_call(encoder%set_data(data, column_major), 'setting encoder data array')

    ! Add some key/value metadata to the frame
    call check_call(encoder%add_property(property_key, property_value), 'adding property')

    ! Encode ODB-2 into a file
    open(newunit=outunit, file=path, access='stream', form='unformatted', status='replace')
    call check_call(encoder%encode(outunit, bytes_written), 'encoding data')
    close(outunit)

    ! Deallocate memory used up by the encoder
    call check_call(encoder%free(), 'cleaning up encoder')

    write(6, '(a,i0,a,a)') 'Written ', nrows, ' rows to ', trim(path)

contains

    subroutine check_call(err, desc)
        integer, intent(in) :: err
        character(*), intent(in) :: desc

        if (err /= ODC_SUCCESS) then
            write(7, *) '**** An error occurred in ODC library'
            write(7, *) 'Description: ', desc
            write(7, *) 'Error: ', odc_error_string(err)
            stop 1
        end if
    end subroutine

    subroutine usage()
        write(6, *) 'Usage:'
        write(6, *) '    odc-fortran-encode-row-major <odb2 output file>'
    end subroutine

    subroutine cycle_ints(output, input)
        integer(8), intent(in) :: input(:)
        integer(8), intent(out) :: output(:)
        integer :: i

        do i = 1, size(output, 1)
            output(i) = input(mod(i - 1, size(input, 1)) + 1)
        end do
    end subroutine

    subroutine cycle_reals(output, input)
        real(8), intent(in) :: input(:)
        real(8), intent(out) :: output(:)
        integer :: i

        do i = 1, size(output, 1)
            output(i) = input(mod(i - 1, size(input, 1)) + 1)
        end do
    end subroutine

    subroutine create_scratch_data(data, nrows)
        real(8), intent(out) :: data(:,:)
        integer(8), intent(in) :: nrows

        character(5) :: expver_str
        character(8) :: date_str
        integer(8) :: date
        character(7) :: statid_str
        character(16) :: wigos_str
        integer(8) :: missing_integer
        integer(8) :: integer_pool(3)
        integer(8), target :: missing_integers(nrows)
        real(8) :: missing_double
        real(8) :: double_pool(3)
        real(8), target :: missing_doubles(nrows)
        integer(8) :: bitfield_pool(3)
        integer(8), target :: bitfield_values(nrows)
        integer(8) :: i

        ! Prepare the current date as an integer
        call date_and_time(date=date_str)
        read(date_str, *) date

        ! Prepare the list of integer values, including the missing value
        call check_call(odc_missing_integer(missing_integer), 'getting missing integer value')
        integer_pool = (/ int(1234, 8), int(4321, 8), missing_integer /)
        call cycle_ints(missing_integers, integer_pool)

        ! Prepare the list of double values, including the missing value
        call check_call(odc_missing_double(missing_double), 'getting missing double value')
        double_pool = (/ real(12.34, 8), real(43.21, 8), missing_double /)
        call cycle_reals(missing_doubles, double_pool)

        ! Prepare the list of bitfield values
        bitfield_pool = (/ int(b'00000001'), int(b'00001011'), int(b'01101011') /)
        call cycle_ints(bitfield_values, bitfield_pool)

        ! Fill in the passed data array with scratch values
        do i = 1, nrows
            write(expver_str, '(a,a)') 'xxxx', '' // c_null_char  ! proper string termination
            data(1, i) = transfer(expver_str, data(1, i))  ! expver

            data(2, i) = transfer(date, data(2, i))  ! date@hdr

            write(statid_str, '(a,i0.2,a)') 'stat', i - 1, '' // c_null_char  ! proper string termination
            data(3, i) = transfer(statid_str, data(3, i))  ! statid@hdr

            write(wigos_str, '(a,i0.2,a)') '0-12345-0-678', i - 1, '' // c_null_char  ! proper string termination
            data(4:5, i) = transfer(wigos_str, data(4:5, i))  ! wigos@hdr

            data(6, i) = 12.3456 * real(i - 1)  ! obsvalue@body
            data(7, i) = transfer(missing_integers(i), data(7, i))  ! integer_missing
            data(8, i) = missing_doubles(i)  ! double_missing
            data(9, i) = transfer(bitfield_values(i), data(9, i))  ! bitfield_column
        end do
    end subroutine

end program
