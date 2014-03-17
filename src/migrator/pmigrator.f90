program pmigrator
    !use, intrinsic :: iso_c_binding
    use odb_module
    use odbgetput
    use odb2
    implicit none

    integer, parameter                             :: max_varlen = 128
    real(8), allocatable                           :: x(:,:)
    integer(4)                                     :: myproc, nproc, npools, jp, db_handle
    integer(4)                                     :: nrows, ncols, nra, rc, poolno
    integer(4)                                     :: numargs, iargc, jr, jc, i
    character(len=500)                             :: dbname, query_name, output_file, file_name
    character(len=500), allocatable                :: colnames(:), datatypes(:)
    type (todb2)                                   :: output_odb

    call odb_start()
    rc = ODB_init(myproc=myproc, nproc=nproc)

    numargs = iargc()
    if (numargs < 1) then
        call ODB_abort('querydata', 'Usage: pmigrator DBname query output_file', numargs)
    endif

    call getarg(1, dbname) ! Database name
    call getarg(2, query_name) ! Query name
    call getarg(3, output_file) ! output file name (pool number will be appended)

    write(*,*) '===== outputfile ', len(TRIM(output_file)), TRIM(output_file)

    npools = 0 ! Gets true value from ODB_open()
    db_handle = ODB_open(dbname, 'READONLY', npools)
     
    ncols = ODB_getnames(db_handle, query_name, 'name')
    allocate(colnames(ncols))
    allocate(datatypes(ncols))
    rc = ODB_getnames(db_handle, query_name, 'name', colnames)
    rc = ODB_getnames(db_handle, query_name, 'datatypes', datatypes)


    do jp = myproc, npools, nproc ! Round-robin, "my" pools only
        call odb2_open(output_odb, trim(output_file)//'.'//trim(str(jp)), status='w')
        call set_output_metadata(output_odb, colnames, datatypes, ncols)
        !-Open output and create columns
        rc = ODB_select(db_handle, query_name, nrows, ncols, nra=nra, poolno=jp)
        if (nrows > 0) then
            allocate(x(nra,0:ncols))
            rc = ODB_get(db_handle, query_name, x, nrows, ncols, poolno=jp)
            call odb2_write(output_odb, transpose(x(:,1:ncols)), ncols, nrows)
            !write(*,'((4a20))') (trim(colnames(jc)), jc=1,ncols)
            !do jr=1,nrows
            !    !write(*,'(1p,(5x,4(1x,g20.10)))') x(jr,1:ncols)
            !enddo
            deallocate(x)
        endif
        rc = ODB_cancel(db_handle, query_name, poolno=jp)
        rc = ODB_release(db_handle, poolno=jp)
        call odb2_close(output_odb)
    enddo

    deallocate(colnames)
    deallocate(datatypes)
    rc = ODB_close(db_handle)
    rc = ODB_end()

contains

character(len=20) function str(k)
    !"Convert an integer to string."
    integer, intent(in) :: k
    write (str, *) k
    str = adjustl(str)
end function str

subroutine set_output_metadata(output_odb, column_names, column_types, number_of_columns)
    type (todb2),intent(inout)     :: output_odb
    character(len=*),allocatable   :: column_names(:), column_types(:)
    integer                        :: number_of_columns, i

    !write(*,*) 'number_of_columns:', number_of_columns
    !do i = 1,number_of_columns
    !    write (*,*) trim(column_types(i))
    !enddo
    call odb2_set_columns(output_odb, number_of_columns, column_types)

    !number_of_columns = 4
    !allocate(column_names(number_of_columns))
    !column_names(1) = 'pk9real:lat'
    !column_names(2) = 'pk9real:lon'
    !column_names(3) = 'pk1int:varno'
    !column_names(4) = 'pk9real:obsvalue'
    !call odb2_set_columns(output_odb, number_of_columns, column_names)

end subroutine set_output_metadata

end program pmigrator
