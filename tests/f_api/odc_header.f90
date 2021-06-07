! To build this program, please make sure to first compile odc Fortran module,
! and then reference linked libraries:
!
!     gfortran -c ../../src/odc/api/odc.f90
!     gfortran -lodccore -lfodc -o odc-fortran-header odc_header.f90

program odc_header
    use, intrinsic :: iso_fortran_env, only: error_unit,output_unit
    use odc
    implicit none

    character(255) :: path, spacer
    type(odc_reader) :: reader
    type(odc_frame) :: frame
    integer(8), target :: nrows
    integer :: err, narg, nframe
    integer :: nproperties, idx
    character(:), allocatable, target :: key, val
    character(:), allocatable, target :: name, type_name, bf_name
    integer, target :: ncols, col, type, element_size, bitfield_count
    integer, target :: bf, bf_offset, bf_size

    if (command_argument_count() < 1) then
        call usage()
        stop 1
    end if

    ! Initialise API
    call check_call(odc_initialise_api(), 'initialising api')

    ! Iterate over all supplied path arguments
    do narg = 1, command_argument_count()
        call get_command_argument(narg, path)

        ! Open current path and initialise frame
        call check_call(reader%open_path(trim(path)), 'opening path')
        call check_call(frame%initialise(reader), 'initialising frame')

        write(output_unit, '(a,a)') 'File: ', trim(path)
        call flush(output_unit)

        nframe = 1

        ! Advance to the first frame in the stream in non-aggregated mode
        err = frame%next()

        do while (err == ODC_SUCCESS)

            ! Get row and column counts
            call check_call(frame%row_count(nrows), 'getting row count')
            call check_call(frame%column_count(ncols), 'getting column count')

            write(output_unit, '(a,i0,a,i0,a,i0)') '  Frame: ', nframe, ', Row count: ', nrows, &
                ', Column count: ', ncols
            call flush(output_unit)

            ! Get number of properties encoded in the frame
            call check_call(frame%properties_count(nproperties), 'getting property count')

            do idx = 1, nproperties

                ! Get property key and value by its index
                call check_call(frame%property_idx(idx, key, val), 'getting property by index')

                write(output_unit, '(a,a,a,a)') '  Property: ', key, ' => ', val
            end do

            ! Iterate over frame columns
            do col = 1, ncols

                ! Get column information
                call check_call(frame%column_attributes(col, name, type, element_size, bitfield_count=bitfield_count), &
                    'getting column attributes')

                ! Lookup column type name
                call check_call(odc_column_type_name(type, type_name), 'getting column type name')

                write(output_unit, '(a,i0,a,a,a,a,a,i0)') '    Column: ', col, ', Name: ', name, &
                    ', Type: ', type_name, ', Size: ', element_size
                call flush(output_unit)

                ! Process bitfields only
                if (type == ODC_BITFIELD) then
                    do bf = 1, bitfield_count

                        ! Get bitfield information
                        call check_call(frame%bitfield_attributes(col, bf, bf_name, bf_offset, bf_size), &
                            'getting bitfield attributes')

                        write(output_unit, '(a,i0,a,a,a,i0,a,i0)') '      Bitfield: ', bf, ', Name: ', bf_name, &
                            ', Offset: ', bf_offset, ', Nbits: ', bf_size
                        call flush(output_unit)
                    end do
                end if
            end do

            nframe = nframe + 1
            write(output_unit, '(a)') ''
            call flush(output_unit)

            ! Advances to the next frame in the stream in non-aggregated mode
            err = frame%next()
        end do

        if (err /= ODC_ITERATION_COMPLETE) call check_call(err, "get next frame")

        call check_call(reader%close(), "closing reader")
    end do

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
        write(output_unit, *) '    odc-fortran-header <odb2 file 1> [<odb2 file 2> ...]'
    end subroutine

end program
