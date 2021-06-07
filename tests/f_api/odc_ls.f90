! To build this program, please make sure to first compile odc Fortran module,
! and then reference linked libraries:
!
!     gfortran -c ../../src/odc/api/odc.f90
!     gfortran -lodccore -lfodc -o odc-fortran-ls odc_ls.f90

program odc_ls
    use, intrinsic :: iso_fortran_env, only: error_unit,output_unit
    use odc
    implicit none

    character(255) :: path
    type(odc_reader) :: reader
    type(odc_frame) :: frame
    type(odc_decoder) :: decoder
    integer(8) :: nrows
    integer :: err, ncols

    if (command_argument_count() /= 1) then
        call usage()
        stop 1
    end if

    call get_command_argument(1, path)
    call check_call(odc_initialise_api(), "initialising api")

    call check_call(reader%open_path(trim(path)), "opening path")
    call check_call(frame%initialise(reader), "initialising frame")

    err = frame%next()
    do while (err == ODC_SUCCESS)

        call check_call(frame%column_count(ncols), "getting column count")
        call write_header(frame, ncols)

        call check_call(decoder%initialise(), "initialising decoder")
        call check_call(decoder%defaults_from_frame(frame), "setting decoder structure")
        call check_call(decoder%decode(frame, nrows), "decoding data")
        call write_data(decoder, frame, nrows, ncols)
        call check_call(decoder%free(), "cleaning up decoder")

        err = frame%next()
    end do

    if (err /= ODC_ITERATION_COMPLETE) call check_call(err, "get next frame")

    call check_call(reader%close(), "closing reader")

contains

    subroutine check_call(err, desc)
        integer, intent(in) :: err
        character(*), intent(in) :: desc

        if (err /= ODC_SUCCESS) then
            write(error_unit, *) '**** An error occurred in ODC library'
            write(error_unit, *) 'Description: ', desc
            write(error_unit, *) 'Error: ', odc_error_string(err)
            stop 1
        end if
    end subroutine

    subroutine usage()
        write(output_unit, *) 'Usage:'
        write(output_unit, *) '    odc-fortran-ls <odb2 file>'
    end subroutine

    subroutine write_header(frame, ncols)
        type(odc_frame), intent(in) :: frame
        integer, intent(in) :: ncols
        character(:), allocatable :: name_str
        integer :: col

        do col = 1, ncols
            call write_integer(output_unit, col)
            call check_call(frame%column_attributes(col, name=name_str), "getting column name")
            write(output_unit, '(3a)', advance='no') '. ', name_str, char(9)
        end do
        write(output_unit,*)
    end subroutine

    subroutine write_data(decoder, frame, nrows, ncols)
        type(odc_decoder), intent(inout) :: decoder
        type(odc_frame), intent(in) :: frame
        integer(8), intent(in) :: nrows
        integer, intent(in) :: ncols
        integer(8) :: row
        real(8), pointer :: array_data(:,:)
        integer :: ncols_decoder, ncols_frame, col, current_index, bitfield_count
        integer(8) :: missing_integer
        real(8) :: missing_double
        integer, dimension(ncols) :: types, sizes, bf_sizes, indexes
        integer, target :: bf, bf_size

        call check_call(decoder%data(array_data), "getting access to data")

        call check_call(decoder%column_count(ncols_decoder), "getting decoder column count")
        call check_call(frame%column_count(ncols_frame), "getting frame column count")
        if (ncols_decoder /= ncols_frame .or. ncols_decoder /= ncols) then
            write(error_unit, *) 'Something went wrong in the decode target initialisation'
            stop 1
        end if

        current_index = 1
        do col = 1, ncols
            call check_call(frame%column_attributes(col, type=types(col), bitfield_count=bitfield_count), "getting column type")
            call check_call(decoder%column_data_array(col, element_size_doubles=sizes(col)), "getting element size")
            indexes(col) = current_index
            current_index = current_index + sizes(col)

            if (types(col) == ODC_BITFIELD) then
                bf_sizes(col) = 0
                do bf = 1, bitfield_count
                    call check_call(frame%bitfield_attributes(col, bf, size=bf_size), "getting bitfield size")
                    bf_sizes(col) = bf_sizes(col) + bf_size
                end do
            end if
        end do

        call check_call(odc_missing_integer(missing_integer), "getting missing integer")
        call check_call(odc_missing_double(missing_double), "getting missing double")

        do row = 1, nrows
            do col = 1, ncols
                select case(types(col))
                case (ODC_INTEGER)
                    if (int(array_data(row, indexes(col))) == missing_integer) then
                        write(output_unit, '(a)', advance='no') '.'
                    else
                        call write_integer(output_unit, int(array_data(row, indexes(col))))
                    end if
                case (ODC_BITFIELD)
                    if (int(array_data(row, indexes(col))) == 0) then
                        write(output_unit, '(a)', advance='no') '.'
                    else
                        call write_bitfield(output_unit, int(array_data(row, indexes(col))), bf_sizes(col))
                    end if
                case (ODC_REAL, ODC_DOUBLE)
                    if (array_data(row, indexes(col)) == missing_double) then
                        write(output_unit, '(a)', advance='no') '.'
                    else
                        call write_double(output_unit, array_data(row, indexes(col)))
                    end if
                case (ODC_STRING)
                    call write_string(output_unit, array_data(row, indexes(col):(indexes(col)+sizes(col)-1)))
                case default
                    write(output_unit, '(a)', advance='no') '<unknown>'
                end select
                write(output_unit, '(a)', advance='no') char(9)
            end do
            write(output_unit, *)
        end do
    end subroutine

    subroutine write_integer(iunit, i)
        integer, intent(in) :: iunit, i
        character(32) :: val
        write(val, *) i
        write(iunit, '(a)', advance='no') trim(adjustl(val))
    end subroutine

    subroutine write_bitfield(iunit, i, size)
        integer, intent(in) :: iunit, i, size
        character(32) :: padding_format, val
        write(padding_format, '(a,i0,a)') '(b32.', size, ')'
        write(val, padding_format) i
        write(iunit, '(a)', advance='no') trim(adjustl(val))
    end subroutine

    subroutine write_double(iunit, r)
        integer, intent(in) :: iunit
        real(8), intent(in) :: r
        character(32) :: val
        write(val, '(f8.4)') r
        write(iunit, '(a)', advance='no') trim(adjustl(val))
    end subroutine

    subroutine write_string(iunit, double_string)
        integer, intent(in) :: iunit
        real(8), intent(in), dimension(:) :: double_string
        character(8*size(double_string)) :: strtmp
        if (all(transfer(double_string, 1_8, size(double_string)) == 0)) then
            write(iunit, '(a)', advance='no') '.'
        else
            write(iunit, '(a)', advance='no') trim(adjustl(transfer(double_string, strtmp)))
        end if
    end subroutine

end program
