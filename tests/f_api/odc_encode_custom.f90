! To build this program, please make sure to first compile odc Fortran module,
! and then reference linked libraries:
!
!     gfortran -c ../../src/odc/api/odc.f90
!     gfortran -lodccore -lfodc -o odc-fortran-encode-custom odc_encode_custom.f90

program odc_ls
    use, intrinsic :: iso_c_binding, only: c_loc,c_null_char
    use odc
    implicit none

    character(255) :: path

    integer(8), parameter :: nrows = 20
    character(8), target :: data1(nrows)
    integer(8), target :: data2(nrows)
    character(8), target :: data3(nrows)
    character(16), target :: data4(nrows)
    real(8), target :: data5(nrows)
    integer(8), target :: data6(nrows)
    real(8), target :: data7(nrows)
    integer(8), target :: data8(nrows)

    type(odc_encoder) :: encoder
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
    call create_scratch_data(data1, data2, data3, data4, data5, data6, data7, nrows)

    ! Initialise encoder
    call check_call(encoder%initialise(), 'initialising encoder')

    ! Set number of rows to allocate in the encoder
    call check_call(encoder%set_row_count(nrows), 'setting number of rows')

    ! Define all column names and their types
    call check_call(encoder%add_column('expver', ODC_STRING), 'adding expver column')
    call check_call(encoder%add_column('date@hdr', ODC_INTEGER), 'adding date@hdr column')
    call check_call(encoder%add_column('statid@hdr', ODC_STRING), 'adding statid@hdr column')
    call check_call(encoder%add_column('wigos@hdr', ODC_STRING), 'adding wigos@hdr column')
    call check_call(encoder%add_column('obsvalue@body', ODC_REAL), 'adding obsvalue@bod column')
    call check_call(encoder%add_column('integer_missing', ODC_INTEGER), 'adding integer_missing column')
    call check_call(encoder%add_column('double_missing', ODC_REAL), 'adding double_missing column')
    call check_call(encoder%add_column('bitfield_column', ODC_BITFIELD), 'adding bitfield_column')

    ! Column `wigos@hdr` is a 16-byte string column
    call check_call(encoder%column_set_data_size(4, element_size_doubles=2), 'setting column data size')

    ! Column `bitfield_column` is an integer with 4 bitfield values in it
    call check_call(encoder%column_add_bitfield(8, "flag_a", 1), 'adding flag_a bitfield')
    call check_call(encoder%column_add_bitfield(8, "flag_b", 2), 'adding flag_b bitfield')
    call check_call(encoder%column_add_bitfield(8, "flag_c", 3), 'adding flag_c bitfield')
    call check_call(encoder%column_add_bitfield(8, "flag_d", 1), 'adding flag_d bitfield')

    ! Set a custom data layout and data array for each column
    call check_call(encoder%column_set_data_array(1, 8, stride=8, data=c_loc(data1)), 'setting expver array')
    call check_call(encoder%column_set_data_array(2, 8, stride=8, data=c_loc(data2)), 'setting date array')
    call check_call(encoder%column_set_data_array(3, 8, stride=8, data=c_loc(data3)), 'setting statid array')
    call check_call(encoder%column_set_data_array(4, 16, stride=16, data=c_loc(data4)), 'setting wigos array')
    call check_call(encoder%column_set_data_array(5, 8, stride=8, data=c_loc(data5)), 'setting obsvalue array')
    call check_call(encoder%column_set_data_array(6, 8, stride=8, data=c_loc(data6)), 'setting integer_missing array')
    call check_call(encoder%column_set_data_array(7, 8, stride=8, data=c_loc(data7)), 'setting double_missing array')
    call check_call(encoder%column_set_data_array(8, 8, stride=8, data=c_loc(data8)), 'setting bitfield_column array')

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
        write(6, *) '    odc-fortran-encode-custom <odb2 output file>'
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

    subroutine create_scratch_data(data1, data2, data3, data4, data5, data6, data7, nrows)
        character(8), intent(out) :: data1(:)
        integer(8), intent(out) :: data2(:)
        character(8), intent(out):: data3(:)
        character(16), intent(out) :: data4(:)
        real(8), intent(out) :: data5(:)
        integer(8), intent(out) :: data6(:)
        real(8), intent(out) :: data7(:)
        integer(8), intent(in) :: nrows

        character(4) :: expver_str = 'xxxx'
        character(8) :: date_str
        integer(8) :: date
        character(6) :: statid_str
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

        ! Fill in the passed data arrays with scratch values
        do i = 1, nrows
            data1(i) = expver_str // c_null_char  ! expver
            data2(i) = date  ! date@hdr

            write(statid_str, '(a,i0.2)') 'stat', i - 1
            data3(i) = statid_str // c_null_char  ! statid@hdr

            write(wigos_str, '(a,i0.2,a)') '0-12345-0-678', i - 1, '' // c_null_char
            data4(i) = wigos_str  ! wigos@hdr

            data5(i) = 12.3456 * real(i - 1)  ! obsvalue@body
            data6(i) = missing_integers(i)  ! integer_missing
            data7(i) = missing_doubles(i)  ! double_missing
            data8(i) = bitfield_values(i)  ! bitfield_column
        end do
    end subroutine

end program
