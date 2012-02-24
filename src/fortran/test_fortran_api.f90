program test_fortran_api
  use, intrinsic :: iso_c_binding
  use odb_c_binding
  implicit none

  integer, parameter :: max_varlen = 128
  integer(kind=4)    :: ncolumns = 4

  write(0,*) "Calling odb_start..."

  call odb_start()

  call test_fortran_api_setup()
  call test_fortran_api1()
  call test_fortran_api2()

contains
subroutine test_fortran_api_setup
 implicit none
 type(C_PTR)                                   :: odb_handle, odb_it
 integer(kind=C_INT)                           :: cerr
 character(kind=C_CHAR, len=max_varlen)        :: config = C_NULL_CHAR
 character(kind=C_CHAR, len=max_varlen)        :: outputfile="test.odb"//achar(0)
 integer(kind=4)                               :: i
 integer(kind=C_INT)                           :: itype, c_ncolumns 
 real(kind=C_DOUBLE), dimension(:), allocatable:: one_row
 character(len=100)                            :: expver="fihn"//achar(0)

 write(0,*) 'test_fortran_api_setup'
 c_ncolumns=ncolumns
 odb_handle = odb_write_new(config, cerr)
 odb_it = odb_write_iterator_new(odb_handle, outputfile, cerr);
 
 cerr = odb_write_set_no_of_columns(odb_it, ncolumns)
 cerr = odb_write_set_column(odb_it, 0, ODB_INTEGER, "ifoo"//achar(0))
 cerr = odb_write_set_column(odb_it, 1, ODB_REAL, "nbar"//achar(0))
 cerr = odb_write_set_bitfield(odb_it, 2, ODB_BITFIELD, "status"//achar(0), &
                                     "active:passive:blacklisted:"//achar(0), &
                                     "1:1:4:"//achar(0))
 cerr = odb_write_set_column(odb_it, 3, ODB_STRING, "expver"//achar(0))

 cerr = odb_write_set_missing_value(odb_it, 0, 1.0_8)
 cerr = odb_write_header(odb_it)

 allocate(one_row(ncolumns))
 do i=1,10
   one_row(1) = i
   one_row(2) = i 
   one_row(3) = 5 
   one_row(4) = transfer(expver, one_row(4))
   cerr = odb_write_set_next_row(odb_it, one_row, c_ncolumns)
 enddo
 deallocate(one_row)

 cerr = odb_write_iterator_delete(odb_it)
 cerr = odb_write_delete(odb_handle)

end subroutine test_fortran_api_setup

subroutine test_fortran_api1
 implicit none
 type(C_PTR)                                   :: odb_handle, odb_it
 integer(kind=C_INT)                           :: cerr
 character(kind=C_CHAR, len=max_varlen)        :: config = C_NULL_CHAR
 character(kind=C_CHAR, len=max_varlen)        :: inputfile="test.odb"//achar(0)
 type(C_PTR)                                   :: ptr_colname
 character(kind=C_CHAR,len=1), dimension(:), pointer :: f_ptr_colname
 character(len=max_varlen)                     :: colname
 integer(kind=4)                               :: i
 integer(kind=C_INT)                           :: itype, newdataset, c_ncolumns=2, size_name 
 real(kind=C_DOUBLE), dimension(:), allocatable:: one_row
 character(len=8)                              :: tmp_str

 write(0,*) 'test_fortran_api1'
 odb_handle = odb_read_new(config, cerr)
 odb_it = odb_read_iterator_new(odb_handle, inputfile, cerr);
 
 if (cerr /=0) STOP 1
 
 cerr = odb_read_get_no_of_columns(odb_it, c_ncolumns)
 if (cerr /=0) STOP 2
 if (c_ncolumns /= ncolumns) STOP 3
 
 cerr = odb_read_get_column_type(odb_it, 0, itype)
 if (cerr /=0) STOP 4
 if (itype /= ODB_INTEGER) STOP 5

 cerr = odb_read_get_column_type(odb_it, 1, itype)
 if (cerr /=0) STOP 6
 if (itype /= ODB_REAL) STOP 7

 cerr = odb_read_get_column_type(odb_it, 2, itype)
 if (cerr /=0) STOP 8
 if (itype /= ODB_BITFIELD) STOP 9

 cerr = odb_read_get_column_type(odb_it, 3, itype)
 if (cerr /=0) STOP 10
 if (itype /= ODB_STRING) STOP 11

 cerr = odb_read_get_column_name(odb_it, 0, ptr_colname, size_name)
 
 if (cerr /= 0) STOP 12
 call C_F_POINTER(CPTR=ptr_colname, FPTR=f_ptr_colname, shape=(/size_name/));
 do i=1, max_varlen ! should not be hard-coded...
    if (f_ptr_colname(i) .eq. C_NULL_CHAR) exit  ! should be C_CHAR_NULL
    colname(i:i)  = f_ptr_colname(i)
 end do
 write(0,*) 'column name 1 : ', colname(1:i)

 cerr = odb_read_get_column_name(odb_it, 1, ptr_colname,size_name)
 if (cerr /=0) STOP 1
 call C_F_POINTER(CPTR=ptr_colname, FPTR=f_ptr_colname, shape=(/size_name/));
 do i=1, max_varlen ! should not be hard-coded...
    if (f_ptr_colname(i) .eq. C_NULL_CHAR) exit  ! should be C_CHAR_NULL
    colname(i:i)  = f_ptr_colname(i)
 end do
 write(0,*) 'column name 2 : ', colname(1:i)

 cerr = odb_read_get_column_name(odb_it, 2, ptr_colname,size_name)
 if (cerr /=0) STOP 1
 call C_F_POINTER(CPTR=ptr_colname, FPTR=f_ptr_colname, shape=(/size_name/));
 do i=1, max_varlen ! should not be hard-coded...
    if (f_ptr_colname(i) .eq. C_NULL_CHAR) exit  ! should be C_CHAR_NULL
    colname(i:i)  = f_ptr_colname(i)
 end do
 write(0,*) 'column name 3 : ', colname(1:i)

 cerr = odb_read_get_column_name(odb_it, 3, ptr_colname,size_name)
 if (cerr /=0) STOP 1
 call C_F_POINTER(CPTR=ptr_colname, FPTR=f_ptr_colname, shape=(/size_name/));
 do i=1, max_varlen ! should not be hard-coded...
    if (f_ptr_colname(i) .eq. C_NULL_CHAR) exit  ! should be C_CHAR_NULL
    colname(i:i)  = f_ptr_colname(i)
 end do
 write(0,*) 'column name 4 : ', colname(1:i)



 allocate(one_row(c_ncolumns))
 cerr = 0
 i = 1
 do 
   cerr = odb_read_get_next_row(odb_it, c_ncolumns,  one_row, newdataset)
   if ( cerr /= 0) exit
   tmp_str = transfer(one_row(4), tmp_str)
   write(0,*) i, ":", one_row(1), one_row(2), one_row(3), tmp_str
   i = i + 1
 enddo
 deallocate(one_row)

 cerr = odb_read_iterator_delete(odb_it)
 cerr = odb_read_delete(odb_handle)

end subroutine test_fortran_api1

subroutine test_fortran_api2
 implicit none
 type(C_PTR)                                   :: odb_handle, odb_it
 integer(kind=C_INT)                           :: cerr
 character(kind=C_CHAR, len=64)                :: config = C_NULL_CHAR
 type(C_PTR)                                   :: ptr_colname
 character(kind=C_CHAR), dimension(:), pointer :: f_ptr_colname
 character(len=max_varlen)                     :: colname
 integer(kind=4)                               :: i
 character(kind=C_CHAR, len=128)               :: sql='select * from "test.odb";'//achar(0)
 integer(kind=C_INT)                           :: itype, newdataset, c_ncolumns=3, size_name 
 real(kind=C_DOUBLE), dimension(:), allocatable:: one_row
 character(len=8)                 :: tmp_str

 write(0,*) 'test_fortran_api2'

 odb_handle = odb_read_new(config, cerr)
 if (cerr /=0) STOP 1

 odb_it =  odb_select_iterator_new(odb_handle, sql, cerr);
 if (cerr /=0) STOP 1

 
 cerr = odb_select_get_no_of_columns(odb_it, c_ncolumns)
 if (cerr /=0) STOP 1
 if (c_ncolumns /= ncolumns) STOP "Error number of columns is not 4"
 
 cerr = odb_select_get_column_type(odb_it, 0, itype)
 if (cerr /=0) STOP 1
 if (itype /= ODB_INTEGER) STOP "Error type of column 1 is not integer!"

 cerr = odb_select_get_column_type(odb_it, 1, itype)
 if (cerr /=0) STOP 1
 if (itype /= ODB_REAL) STOP "Error type of column 2 is not real!"

 cerr = odb_select_get_column_type(odb_it, 2, itype)
 if (cerr /=0) STOP 1
 if (itype /= ODB_BITFIELD) STOP "Error type of column 3 is not bitfield!"

 cerr = odb_select_get_column_type(odb_it, 3, itype)
 if (cerr /=0) STOP 1
 if (itype /= ODB_STRING) STOP "Error type of column 4 is not string!"

 cerr = odb_select_get_column_name(odb_it, 0, ptr_colname,size_name)
 if (cerr /=0) STOP 1
 call C_F_POINTER(CPTR=ptr_colname, FPTR=f_ptr_colname, shape=(/size_name/));
 do i=1, max_varlen ! should not be hard-coded...
    if (f_ptr_colname(i) .eq. C_NULL_CHAR) exit  ! should be C_CHAR_NULL
    colname(i:i)  = f_ptr_colname(i)
 end do
 write(0,*) 'column name 1 : ', colname(1:i)

 cerr = odb_select_get_column_name(odb_it, 1, ptr_colname,size_name)
 if (cerr /=0) STOP 1
 call C_F_POINTER(CPTR=ptr_colname, FPTR=f_ptr_colname, shape=(/size_name/));
 do i=1, max_varlen ! should not be hard-coded...
    if (f_ptr_colname(i) .eq. C_NULL_CHAR) exit  ! should be C_CHAR_NULL
    colname(i:i)  = f_ptr_colname(i)
 end do
 write(0,*) 'column name 2 : ', colname(1:i)

 cerr = odb_select_get_column_name(odb_it, 2, ptr_colname,size_name)
 if (cerr /=0) STOP 1
 call C_F_POINTER(CPTR=ptr_colname, FPTR=f_ptr_colname, shape=(/size_name/));
 do i=1, max_varlen ! should not be hard-coded...
    if (f_ptr_colname(i) .eq. C_NULL_CHAR) exit  ! should be C_CHAR_NULL
    colname(i:i)  = f_ptr_colname(i)
 end do
 write(0,*) 'column name 3 : ', colname(1:i)


 cerr = odb_select_get_column_name(odb_it, 3, ptr_colname,size_name)
 if (cerr /=0) STOP 1
 call C_F_POINTER(CPTR=ptr_colname, FPTR=f_ptr_colname, shape=(/size_name/));
 do i=1, max_varlen ! should not be hard-coded...
    if (f_ptr_colname(i) .eq. C_NULL_CHAR) exit  ! should be C_CHAR_NULL
    colname(i:i)  = f_ptr_colname(i)
 end do
 write(0,*) 'column name 4 : ', colname(1:i)

 allocate(one_row(c_ncolumns))
 cerr=0
 i = 1
 do 
   cerr = odb_select_get_next_row(odb_it, c_ncolumns,  one_row, newdataset)
   if ( cerr /= 0) exit
   tmp_str = transfer(one_row(4), tmp_str)
   write(0,*) i, ":",  one_row(1), one_row(2), one_row(3), tmp_str
   i = i + 1
 enddo
 deallocate(one_row)

 cerr = odb_select_iterator_delete(odb_it)
 cerr = odb_read_delete(odb_handle)

end subroutine test_fortran_api2
end program test_fortran_api
