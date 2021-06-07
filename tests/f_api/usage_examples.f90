
module fapi_usage_examples_tests
    use, intrinsic :: iso_fortran_env
    use odc
    implicit none

contains

    function test_odc_encode_row_major() result(success)

        ! Test the encode row-major example

        logical :: success
        character(:), allocatable :: command
        integer, target :: command_exit_code
        character(:), allocatable :: command_output
        character(:), allocatable :: command_expected

        success = .true.

        command = './odc-fortran-encode-row-major > odc-fortran-encode-row-major.out 2>/dev/null'

        call execute_command_line(command, exitstat=command_exit_code)

        ! Check exit code (error)
        if (command_exit_code /= 1) then
            write(error_unit, *) 'unexpected command exit code:', command_exit_code, '/=', 1
            success = .false.
        end if

        command_output = test_read_text_file('odc-fortran-encode-row-major.out')
        command_expected = test_read_text_file('odc-fortran-encode-row-major.1')

        ! Check command output
        if (command_output /= command_expected) then
            write(error_unit, *) 'unexpected command output:', command_output, '/=', command_expected
            success = .false.
        end if

        command = './odc-fortran-encode-row-major test-1.odb > odc-fortran-encode-row-major.out'

        call execute_command_line(command, exitstat=command_exit_code)

        ! Check exit code (success)
        if (command_exit_code /= 0) then
            write(error_unit, *) 'unexpected command exit code:', command_exit_code, '/=', 0
            success = .false.
        end if

        command_output = test_read_text_file('odc-fortran-encode-row-major.out')
        command_expected = test_read_text_file('odc-fortran-encode-row-major.2')

        ! Check command output
        if (command_output /= command_expected) then
            write(error_unit, *) 'unexpected command output:', command_output, '/=', command_expected
            success = .false.
        end if
    end function

    function test_odc_encode_custom() result(success)

        ! Test the encode custom layout example

        logical :: success
        character(:), allocatable :: command
        integer, target :: command_exit_code
        character(:), allocatable :: command_output
        character(:), allocatable :: command_expected

        success = .true.

        command = './odc-fortran-encode-custom > odc-fortran-encode-custom.out 2>/dev/null'

        call execute_command_line(command, exitstat=command_exit_code)

        ! Check exit code (error)
        if (command_exit_code /= 1) then
            write(error_unit, *) 'unexpected command exit code:', command_exit_code, '/=', 1
            success = .false.
        end if

        command_output = test_read_text_file('odc-fortran-encode-custom.out')
        command_expected = test_read_text_file('odc-fortran-encode-custom.1')

        ! Check command output
        if (command_output /= command_expected) then
            write(error_unit, *) 'unexpected command output:', command_output, '/=', command_expected
            success = .false.
        end if

        command = './odc-fortran-encode-custom test-2.odb > odc-fortran-encode-custom.out'

        call execute_command_line(command, exitstat=command_exit_code)

        ! Check exit code (success)
        if (command_exit_code /= 0) then
            write(error_unit, *) 'unexpected command exit code:', command_exit_code, '/=', 0
            success = .false.
        end if

        command_output = test_read_text_file('odc-fortran-encode-custom.out')
        command_expected = test_read_text_file('odc-fortran-encode-custom.2')

        ! Check command output
        if (command_output /= command_expected) then
            write(error_unit, *) 'unexpected command output:', command_output, '/=', command_expected
            success = .false.
        end if
    end function

    function test_odc_ls() result(success)

        ! Test the list program example

        logical :: success
        character(:), allocatable :: command
        integer, target :: command_exit_code
        character(:), allocatable :: command_output
        character(:), allocatable :: command_expected
        character(8) :: date_str

        success = .true.

        command = './odc-fortran-ls > odc-fortran-ls.out 2>/dev/null'

        call execute_command_line(command, exitstat=command_exit_code)

        ! Check exit code (error)
        if (command_exit_code /= 1) then
            write(error_unit, *) 'unexpected command exit code:', command_exit_code, '/=', 1
            success = .false.
        end if

        command_output = test_read_text_file('odc-fortran-ls.out')
        command_expected = test_read_text_file('odc-fortran-ls.1')

        ! Check command output
        if (command_output /= command_expected) then
            write(error_unit, *) 'unexpected command output:', command_output, '/=', command_expected
            success = .false.
        end if

        command = './odc-fortran-ls test-1.odb > odc-fortran-ls.out'

        call execute_command_line(command, exitstat=command_exit_code)

        ! Check exit code (success)
        if (command_exit_code /= 0) then
            write(error_unit, *) 'unexpected command exit code:', command_exit_code, '/=', 0
            success = .false.
        end if

        command_output = test_read_text_file('odc-fortran-ls.out')
        command_expected = test_read_text_file('odc-fortran-ls.2')

        ! Prepare the current date as a string
        call date_and_time(date=date_str)

        ! Replace placeholder with current date
        command_expected = test_replace_text(command_expected, '%current_date%', date_str)

        ! Check command output
        if (command_output /= command_expected) then
            write(error_unit, *) 'unexpected command output:', command_output, '/=', command_expected
            success = .false.
        end if
    end function

    function test_odc_header() result(success)

        ! Test the header statistics example

        logical :: success
        character(:), allocatable :: command
        integer, target :: command_exit_code
        character(:), allocatable :: command_output
        character(:), allocatable :: command_expected
        character(:), allocatable, target :: version
        integer :: rc

        success = .true.

        command = './odc-fortran-header > odc-fortran-header.out 2>/dev/null'

        call execute_command_line(command, exitstat=command_exit_code)

        ! Check exit code (error)
        if (command_exit_code /= 1) then
            write(error_unit, *) 'unexpected command exit code:', command_exit_code, '/=', 1
            success = .false.
        end if

        command_output = test_read_text_file('odc-fortran-header.out')
        command_expected = test_read_text_file('odc-fortran-header.1')

        ! Check command output
        if (command_output /= command_expected) then
            write(error_unit, *) 'unexpected command output:', command_output, '/=', command_expected
            success = .false.
        end if

        command = './odc-fortran-header test-1.odb test-2.odb > odc-fortran-header.out'

        call execute_command_line(command, exitstat=command_exit_code)

        ! Check exit code (success)
        if (command_exit_code /= 0) then
            write(error_unit, *) 'unexpected command exit code:', command_exit_code, '/=', 0
            success = .false.
        end if

        command_output = test_read_text_file('odc-fortran-header.out')
        command_expected = test_read_text_file('odc-fortran-header.2')

        ! Replace placeholder with current version number
        rc = odc_version(version)
        command_expected = test_replace_text(command_expected, '%odc_version_placeholder%', version)

        ! Check command output
        if (command_output /= command_expected) then
            write(error_unit, *) 'unexpected command output:', command_output, '/=', command_expected
            success = .false.
        end if
    end function

    function test_read_text_file(file_name) result(file_contents)
        character(*) :: file_name
        character(:), allocatable, target :: file_contents
        integer, target :: file_unit, file_contents_size

        open(unit=file_unit, file=trim(file_name), action='read', form='unformatted', access='stream')
        inquire(unit=file_unit, size=file_contents_size)
        allocate(character(file_contents_size) :: file_contents)
        read(file_unit) file_contents
        close(file_unit)
    end function

    function test_replace_text(haystack, needle, replacement) result(output)
        character(*) :: haystack, needle, replacement
        character(len(haystack)+100) :: output
        integer :: i, in, ir

        output = haystack
        in = len_trim(needle)
        ir = len_trim(replacement)
        do
           i = index(output, needle(:in))
           if (i == 0) exit
           output = output(:i-1) // replacement(:ir) // output(i+in:)
        end do

    end function

end module

program fapi_usage_examples

    use fapi_usage_examples_tests
    implicit none

    logical :: success
    success = .true.

    success = success .and. test_odc_encode_row_major()
    success = success .and. test_odc_encode_custom()
    success = success .and. test_odc_ls()
    success = success .and. test_odc_header()

    if (.not. success) stop -1

end program
