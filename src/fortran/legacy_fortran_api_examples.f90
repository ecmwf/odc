! (C) Copyright 1996-2012 ECMWF.
! 
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
! In applying this licence, ECMWF does not waive the privileges and immunities 
! granted to it by virtue of its status as an intergovernmental organisation nor
! does it submit to any jurisdiction.
!

program example_fortran_api
  use, intrinsic :: iso_c_binding
  use odc_c_binding
  implicit none

  integer, parameter            :: max_varlen = 128
  integer(kind=4), parameter    :: ncolumns = 6

  ! Correct values for checking

  integer(c_int), dimension(ncolumns), parameter:: column_types = (/ODB_INTEGER, ODB_REAL, ODB_BITFIELD, &
                                                                    ODB_STRING, ODB_STRING, ODB_DOUBLE/)

  character(8), dimension(ncolumns), parameter  :: column_names = (/'ifoo    ', 'nbar    ', 'status  ', 'wigos   ', &
                                                                    'expver  ', 'dbar    '/)

  integer(c_int), dimension(ncolumns), parameter:: column_offsets = (/1, 2, 3, 4, 7, 8/)
  integer(c_int), dimension(ncolumns), parameter:: column_sizes = (/1, 1, 1, 3, 1, 1/)
  integer, parameter                            :: row_size_doubles = 8

  interface
    function strlen(s) result(l) bind(c, name='strlen')
      use, intrinsic    :: iso_c_binding
      character(c_char) :: s
      integer(c_int)    :: l
    end function
  end interface

  write(0,*) "Calling odb_start..."

  call odb_start()

  call example_fortran_api_setup()
  call example_fortran_api1()
  call example_fortran_api2()
  call example_fortran_api_append()

contains

subroutine example_fortran_api_append
 implicit none
 type(C_PTR)                                   :: odb_handle, odb_it
 integer(kind=C_INT)                           :: cerr
 character(kind=C_CHAR, len=max_varlen)        :: config = C_NULL_CHAR
 character(kind=C_CHAR, len=max_varlen)        :: outputfile="example_fortran_api_append.odb"//achar(0)
 integer(kind=4)                               :: i
 integer(kind=C_INT)                           :: itype, c_ncolumns 
 real(kind=C_DOUBLE), dimension(:), allocatable:: one_row
 integer(kind=c_int)                           :: offsets(ncolumns)
 integer(kind=c_int)                           :: row_length_doubles
 character(len=100)                            :: expver="fihn"//achar(0)
 character(len=100)                            :: wigos="this-is-a-long-string"//achar(0)

 write(0,*) 'example_fortran_api_append'
 c_ncolumns = ncolumns
 odb_handle = odb_write_new(config, cerr)
 odb_it = odb_write_iterator_new(odb_handle, outputfile, cerr);
 
 cerr = odb_write_set_no_of_columns(odb_it, ncolumns)
 if (cerr == 0) cerr = odb_write_set_column(odb_it, 0, ODB_INTEGER, "ifoo"//achar(0))
 if (cerr == 0) cerr = odb_write_set_column(odb_it, 1, ODB_REAL, "nbar"//achar(0))
 if (cerr == 0) cerr = odb_write_set_bitfield(odb_it, 2, ODB_BITFIELD, "status"//achar(0), &
                                              "active:passive:blacklisted:"//achar(0), &
                                              "1:1:4:"//achar(0))
 if (cerr == 0) cerr = odb_write_set_column(odb_it, 3, ODB_STRING, "wigos"//achar(0))
 if (cerr == 0) cerr = odb_write_set_column_size_doubles(odb_it, 3, 4);
 if (cerr == 0) cerr = odb_write_set_column(odb_it, 4, ODB_STRING, "expver"//achar(0))
 if (cerr == 0) cerr = odb_write_set_column(odb_it, 5, ODB_DOUBLE, "dbar"//achar(0))

 if (cerr == 0) cerr = odb_write_set_missing_value(odb_it, 0, 1.0_8)
 if (cerr == 0) cerr = odb_write_header(odb_it)

 if (cerr == 0) cerr = odb_write_get_row_buffer_size_doubles(odb_it, row_length_doubles)
 do i = 1, ncolumns
     if (cerr == 0) cerr = odb_write_get_column_offset(odb_it, i-1, offsets(i))
 enddo
 if (cerr /= 0) stop 1

 ! Sanity check!
 if (row_length_doubles /= 9) stop 1
 if (any(offsets /= (/1, 2, 3, 4, 8, 9/))) stop 1

 allocate(one_row(row_length_doubles))
 do i=1,10
   one_row(offsets(1)) = i
   one_row(offsets(2)) = i
   one_row(offsets(3)) = 5
   one_row(offsets(4):offsets(4)+3) = transfer(wigos, one_row(offsets(4):offsets(4)+3))
   one_row(offsets(5)) = transfer(expver, one_row(5))
   one_row(offsets(6)) = 5
   cerr = odb_write_set_next_row(odb_it, one_row, c_ncolumns)
   if (cerr /= 0) stop 1
 enddo
 deallocate(one_row)

 cerr = odb_write_iterator_delete(odb_it)

 if (cerr == 0) odb_it = odb_append_iterator_new(odb_handle, outputfile, cerr);
 
 if (cerr == 0) cerr = odb_write_set_no_of_columns(odb_it, ncolumns)
 if (cerr == 0) cerr = odb_write_set_column(odb_it, 0, ODB_INTEGER, "ifoo"//achar(0))
 if (cerr == 0) cerr = odb_write_set_column(odb_it, 1, ODB_REAL, "nbar"//achar(0))
 if (cerr == 0) cerr = odb_write_set_bitfield(odb_it, 2, ODB_BITFIELD, "status"//achar(0), &
                                     "active:passive:blacklisted:"//achar(0), &
                                     "1:1:4:"//achar(0))
 if (cerr == 0) cerr = odb_write_set_column(odb_it, 3, ODB_STRING, "wigos"//achar(0))
 if (cerr == 0) cerr = odb_write_set_column_size_doubles(odb_it, 3, 4);
 if (cerr == 0) cerr = odb_write_set_column(odb_it, 4, ODB_STRING, "expver"//achar(0))
 if (cerr == 0) cerr = odb_write_set_column(odb_it, 5, ODB_DOUBLE, "dbar"//achar(0))

 if (cerr == 0) cerr = odb_write_set_missing_value(odb_it, 0, 1.0_8)
 if (cerr == 0) cerr = odb_write_header(odb_it)

 if (cerr == 0) cerr = odb_write_get_row_buffer_size_doubles(odb_it, row_length_doubles)
 do i = 1, ncolumns
     if (cerr == 0) cerr = odb_write_get_column_offset(odb_it, i-1, offsets(i))
 enddo
 if (cerr /= 0) stop 1

 ! Sanity check!
 if (row_length_doubles /= 9) stop 1
 if (any(offsets /= (/1, 2, 3, 4, 8, 9/))) stop 1

 allocate(one_row(row_length_doubles))
 do i=1,10
   one_row(offsets(1)) = i
   one_row(offsets(2)) = i
   one_row(offsets(3)) = 5
   one_row(offsets(4):offsets(4)+3) = transfer(wigos, one_row(offsets(4):offsets(4)+3))
   one_row(offsets(5)) = transfer(expver, one_row(5))
   one_row(offsets(6)) = 5
   cerr = odb_write_set_next_row(odb_it, one_row, c_ncolumns)
   if (cerr /= 0) stop 1
 enddo
 deallocate(one_row)

 cerr = odb_write_iterator_delete(odb_it)
 if (cerr == 0) cerr = odb_write_delete(odb_handle)
 if (cerr /= 0) stop 1

end subroutine example_fortran_api_append

subroutine example_fortran_api_setup
 implicit none
 type(C_PTR)                                   :: odb_handle, odb_it
 integer(kind=C_INT)                           :: cerr
 character(kind=C_CHAR, len=max_varlen)        :: config = C_NULL_CHAR
 character(kind=C_CHAR, len=max_varlen)        :: outputfile="test.odb"//achar(0)
 integer(kind=4)                               :: i
 integer(kind=C_INT)                           :: itype, c_ncolumns 
 real(kind=C_DOUBLE), dimension(:), allocatable:: one_row
 integer(kind=c_int)                           :: offsets(ncolumns)
 integer(kind=c_int)                           :: row_length_doubles
 character(len=100)                            :: expver="fihn"//achar(0)
 character(len=100)                            :: wigos="this-is-a-long-string"//achar(0)

 write(0,*) 'example_fortran_api_setup'
 c_ncolumns = ncolumns
 odb_handle = odb_write_new(config, cerr)
 odb_it = odb_write_iterator_new(odb_handle, outputfile, cerr);
 
 cerr = odb_write_set_no_of_columns(odb_it, ncolumns)
 if (cerr == 0) cerr = odb_write_set_column(odb_it, 0, ODB_INTEGER, "ifoo"//achar(0))
 if (cerr == 0) cerr = odb_write_set_column(odb_it, 1, ODB_REAL, "nbar"//achar(0))
 if (cerr == 0) cerr = odb_write_set_bitfield(odb_it, 2, ODB_BITFIELD, "status"//achar(0), &
                                              "active:passive:blacklisted:"//achar(0), &
                                              "1:1:4:"//achar(0))
 if (cerr == 0) cerr = odb_write_set_column(odb_it, 3, ODB_STRING, "wigos"//achar(0))
 if (cerr == 0) cerr = odb_write_set_column_size_doubles(odb_it, 3, 4);
 if (cerr == 0) cerr = odb_write_set_column(odb_it, 4, ODB_STRING, "expver"//achar(0))
 if (cerr == 0) cerr = odb_write_set_column(odb_it, 5, ODB_DOUBLE, "dbar"//achar(0))

 if (cerr == 0) cerr = odb_write_set_missing_value(odb_it, 0, 1.0_8)
 if (cerr == 0) cerr = odb_write_header(odb_it)

 if (cerr == 0) cerr = odb_write_get_row_buffer_size_doubles(odb_it, row_length_doubles)
 do i = 1, ncolumns
     if (cerr == 0) cerr = odb_write_get_column_offset(odb_it, i-1, offsets(i))
 enddo
 if (cerr /= 0) stop 1

 ! Sanity check!
 if (row_length_doubles /= 9) stop 1
 if (any(offsets /= (/1, 2, 3, 4, 8, 9/))) stop 1

 allocate(one_row(row_length_doubles))
 do i=1,10
   one_row(offsets(1)) = i
   one_row(offsets(2)) = i
   one_row(offsets(3)) = 5
   one_row(offsets(4):offsets(4)+3) = transfer(wigos, one_row(offsets(4):offsets(4)+3))
   one_row(offsets(5)) = transfer(expver, one_row(5))
   one_row(offsets(6)) = 5
   cerr = odb_write_set_next_row(odb_it, one_row, c_ncolumns)
   if (cerr /= 0) stop 1
 enddo
 deallocate(one_row)

 cerr = odb_write_iterator_delete(odb_it)
 if (cerr == 0) cerr = odb_write_delete(odb_handle)
 if (cerr /= 0) stop 1

end subroutine example_fortran_api_setup

subroutine example_fortran_api1

 ! This function tests the reader api

 implicit none
 type(C_PTR)                                   :: odb_handle, odb_it
 integer(kind=C_INT)                           :: cerr
 character(kind=C_CHAR, len=max_varlen)        :: config = C_NULL_CHAR
 character(kind=C_CHAR, len=max_varlen)        :: inputfile = "test.odb"//achar(0)
 type(C_PTR)                                   :: ptr_colname
 type(C_PTR)                                   :: ptr_bitfield_names
 type(C_PTR)                                   :: ptr_bitfield_sizes
 character(kind=C_CHAR,len=1), dimension(:), pointer :: f_ptr_colname
 character(kind=C_CHAR,len=1), dimension(:), pointer :: f_ptr_bitfield_names
 character(kind=C_CHAR,len=1), dimension(:), pointer :: f_ptr_bitfield_sizes
 character(len=max_varlen)                     :: colname
 character(len=max_varlen)                     :: bitfield_names
 character(len=max_varlen)                     :: bitfield_sizes
 integer(kind=4)                               :: i
 integer(kind=C_INT)                           :: itype, newdataset, c_ncolumns=2, size_name 
 integer(kind=C_INT)                           :: bitfield_names_size, bitfield_sizes_size
 real(kind=C_DOUBLE), dimension(:), allocatable:: one_row
 real(kind=C_DOUBLE)                           :: val
 integer(c_int)                                :: isize, ioffset
 character(len=24)                             :: tmp_str1
 character(len=8)                              :: tmp_str2
 integer                                       :: col

 write(0,*) 'example_fortran_api1'
 odb_handle = odb_read_new(config, cerr)
 odb_it = odb_read_iterator_new(odb_handle, inputfile, cerr);
 if (cerr /= 0) STOP 1

 ! Total number of columns
 
 cerr = odb_read_get_no_of_columns(odb_it, c_ncolumns)
 if (cerr /=0) stop 2
 if (c_ncolumns /= ncolumns) stop 3

 ! Total amount of data

 cerr = odb_read_get_row_buffer_size_doubles(odb_it, isize)
 if (cerr /= 0) stop 4
 if (isize /= row_size_doubles) stop 5 ! n.b. 9 in writer, but only needed 3 columns to encode strings!

 ! Check the values reported per column ...

 do col = 1, ncolumns

     cerr = odb_read_get_column_name(odb_it, col-1, ptr_colname,size_name)
     if (cerr == 0) cerr = odb_read_get_column_type(odb_it, col-1, itype)
     if (cerr == 0) cerr = odb_read_get_column_offset(odb_it, col-1, ioffset)
     if (cerr == 0) cerr = odb_read_get_column_size_doubles(odb_it, col-1, isize)
     if (cerr /= 0) stop 6

     ! Check the column names

     call c_f_pointer(CPTR=ptr_colname, FPTR=f_ptr_colname, shape=(/size_name/));
     do i=1, size_name
        colname(i:i)  = f_ptr_colname(i)
     end do
     write(0,'(a,i1,3a,i1,a,i1,a)') 'column name ', col, ' : ', colname(1:size_name), &
                                    ' [', ioffset, ', ', isize, ']'

     ! Do we get the expected values?

     if (colname(1:size_name) /= trim(column_names(col))) stop 7
     if (itype /= column_types(col)) stop 9
     if (ioffset /= column_offsets(col)) stop 11
     if (isize /= column_sizes(col)) stop 12

 end do

 ! Test modified missing value

 val = -13
 cerr = odb_read_get_missing_value(odb_it, 0, val)
 if (cerr /=0) STOP 13
 write(0,*) 'odb_read_get_missing_value: missing value of column 0 => ', val
 if (val /= 1.0) stop 14

 ! Test the contents of the bitfields

 cerr = odb_read_get_bitfield(odb_it, 2, ptr_bitfield_names, ptr_bitfield_sizes, bitfield_names_size, bitfield_sizes_size)
 write(0,*) 'odb_read_get_bitfield column 2 => ', cerr
 if (cerr /=0) STOP 15
 write(0,*) 'column 2 bitfield_names_size: ', bitfield_names_size
 call C_F_POINTER(CPTR=ptr_bitfield_names, FPTR=f_ptr_bitfield_names, shape=(/bitfield_names_size/));
 do i=1, bitfield_names_size
    bitfield_names(i:i) = f_ptr_bitfield_names(i)
 end do
 write(0,*) 'column 2 bitfield_names: ', bitfield_names(1:bitfield_names_size)
 if (bitfield_names(1:bitfield_names_size) /= 'active:passive:blacklisted:') STOP 16

 write(0,*) 'column 2 bitfield_sizes_size: ', bitfield_sizes_size
 call C_F_POINTER(CPTR=ptr_bitfield_sizes, FPTR=f_ptr_bitfield_sizes, shape=(/bitfield_sizes_size/));
 do i=1, bitfield_sizes_size
    bitfield_sizes(i:i) = f_ptr_bitfield_sizes(i)
 end do
 write(0,*) 'column 2 bitfield_sizes: ', bitfield_sizes(1:bitfield_sizes_size)
 if (bitfield_sizes(1:bitfield_sizes_size) /= '1:1:4:') STOP 17

 ! Test the contents of the data section!

 allocate(one_row(row_size_doubles))
 do i = 1, 100

   cerr = odb_read_get_next_row(odb_it, c_ncolumns,  one_row, newdataset)
   if (cerr /= 0) exit

   ! Extract the strings

   tmp_str1(1:24) = transfer(one_row(column_offsets(4):column_offsets(5)-1), tmp_str1(1:24))
   tmp_str2(1:8) = transfer(one_row(column_offsets(5)), tmp_str2(1:8))

   write(0,*) i, ":", one_row(column_offsets(1)), &
                      one_row(column_offsets(2)), &
                      one_row(column_offsets(3)), &
                      tmp_str1(1:24), " ", &
                      tmp_str2(1:8), &
                      one_row(column_offsets(6))

   if (one_row(column_offsets(1)) /= i) stop 18
   if (one_row(column_offsets(2)) /= i) stop 19
   if (one_row(column_offsets(3)) /= 5) stop 20
   if (trim(tmp_str1(1:strlen(tmp_str1))) /= 'this-is-a-long-string') stop 21
   if (trim(tmp_str2(1:strlen(tmp_str2))) /= 'fihn') stop 22
   if (one_row(column_offsets(6)) /= 5) stop 23

 enddo
 deallocate(one_row)

 ! Did we get the correct number of rows?

 if (i /= 11) stop 24

 ! Clean up

 cerr = odb_read_iterator_delete(odb_it)
 cerr = odb_read_delete(odb_handle)

end subroutine example_fortran_api1

subroutine example_fortran_api2
 implicit none
 type(C_PTR)                                   :: odb_handle, odb_it
 integer(kind=C_INT)                           :: cerr
 character(kind=C_CHAR, len=64)                :: config = C_NULL_CHAR
 type(C_PTR)                                   :: ptr_colname
 type(C_PTR)                                   :: ptr_bitfield_names
 type(C_PTR)                                   :: ptr_bitfield_sizes
 character(kind=C_CHAR), dimension(:), pointer :: f_ptr_colname
 character(kind=C_CHAR,len=1), dimension(:), pointer :: f_ptr_bitfield_names
 character(kind=C_CHAR,len=1), dimension(:), pointer :: f_ptr_bitfield_sizes
 character(len=max_varlen)                     :: colname
 character(len=max_varlen)                     :: bitfield_names
 character(len=max_varlen)                     :: bitfield_sizes
 integer(kind=4)                               :: i
 character(kind=C_CHAR, len=128)               :: sql='select * from "test.odb"'//achar(0)
 integer(kind=C_INT)                           :: itype, newdataset, c_ncolumns=3, size_name 
 integer(kind=C_INT)                           :: bitfield_names_size, bitfield_sizes_size, ioffset, isize
 real(kind=C_DOUBLE), dimension(:), allocatable:: one_row
 character(len=64)                             :: tmp_str1, tmp_str2
 integer                                       :: col

 write(0,*) 'example_fortran_api2'

 odb_handle = odb_select_new(config, cerr)
 if (cerr == 0) odb_it = odb_select_iterator_new(odb_handle, sql, cerr);
 if (cerr /= 0) STOP 25

 ! Total number of columns

 cerr = odb_select_get_no_of_columns(odb_it, c_ncolumns)
 if (cerr /=0) STOP 26
 if (c_ncolumns /= ncolumns) stop 27

 ! Total amount of data

 cerr = odb_select_get_row_buffer_size_doubles(odb_it, isize)
 if (cerr /= 0) stop 28
 if (isize /= row_size_doubles) stop 29 ! n.b. 9 in writer, but only needed 3 columns to encode strings!

 ! Check the values reported per column ...

 do col = 1, ncolumns

     cerr = odb_select_get_column_name(odb_it, col-1, ptr_colname,size_name)
     if (cerr == 0) cerr = odb_select_get_column_type(odb_it, col-1, itype)
     if (cerr == 0) cerr = odb_select_get_column_offset(odb_it, col-1, ioffset)
     if (cerr == 0) cerr = odb_select_get_column_size_doubles(odb_it, col-1, isize)
     if (cerr /= 0) stop 30

     ! Check the column names

     call c_f_pointer(CPTR=ptr_colname, FPTR=f_ptr_colname, shape=(/size_name/));
     do i=1, size_name
        colname(i:i)  = f_ptr_colname(i)
     end do
     write(0,'(a,i1,3a,i1,a,i1,a)') 'column name ', col, ' : ', colname(1:size_name), &
                                    ' [', ioffset, ', ', isize, ']'

     ! Do we get the expected values

     write(9,*) itype, column_types(col)
     if (colname(1:size_name) /= trim(column_names(col))) stop 31
     if (itype /= column_types(col)) stop 32
     if (ioffset /= column_offsets(col)) stop 33
     if (isize /= column_sizes(col)) stop 34

 end do

 ! Test the contents of the bitfields

 cerr = odb_select_get_bitfield(odb_it, 2, ptr_bitfield_names, ptr_bitfield_sizes, bitfield_names_size, bitfield_sizes_size)
 write(0,*) 'odb_select_get_bitfield column 2 => ', cerr
 if (cerr /=0) STOP 35
 write(0,*) 'column 2 bitfield_names_size: ', bitfield_names_size
 call C_F_POINTER(CPTR=ptr_bitfield_names, FPTR=f_ptr_bitfield_names, shape=(/bitfield_names_size/));
 do i=1, bitfield_names_size
    bitfield_names(i:i) = f_ptr_bitfield_names(i)
 end do
 write(0,*) 'column 2 bitfield_names: ', bitfield_names(1:bitfield_names_size)
 if (bitfield_names(1:bitfield_names_size) /= 'active:passive:blacklisted:') STOP 36

 write(0,*) 'column 2 bitfield_sizes_size: ', bitfield_sizes_size
 call C_F_POINTER(CPTR=ptr_bitfield_sizes, FPTR=f_ptr_bitfield_sizes, shape=(/bitfield_sizes_size/));
 do i=1, bitfield_sizes_size
    bitfield_sizes(i:i) = f_ptr_bitfield_sizes(i)
 end do
 write(0,*) 'column 2 bitfield_sizes: ', bitfield_sizes(1:bitfield_sizes_size)
 if (bitfield_sizes(1:bitfield_sizes_size) /= '1:1:4:') STOP 37

 ! Test the contents of the data section!

 allocate(one_row(row_size_doubles))
 do i = 1, 100

   cerr = odb_select_get_next_row(odb_it, c_ncolumns,  one_row, newdataset)
   if ( cerr /= 0) exit

   ! Extract the strings

   tmp_str1(1:24) = transfer(one_row(column_offsets(4):column_offsets(5)-1), tmp_str1(1:24))
   tmp_str2(1:8) = transfer(one_row(column_offsets(5)), tmp_str2(1:8))

   write(0,*) i, ":", one_row(column_offsets(1)), &
                      one_row(column_offsets(2)), &
                      one_row(column_offsets(3)), &
                      tmp_str1(1:24), " ", &
                      tmp_str2(1:8), &
                      one_row(column_offsets(6))

   if (one_row(column_offsets(1)) /= i) stop 39
   if (one_row(column_offsets(2)) /= i) stop 40
   if (one_row(column_offsets(3)) /= 5) stop 41
   if (trim(tmp_str1(1:strlen(tmp_str1))) /= 'this-is-a-long-string') stop 42
   if (trim(tmp_str2(1:strlen(tmp_str2))) /= 'fihn') stop 43
   if (one_row(column_offsets(6)) /= 5) stop 44

 enddo
 deallocate(one_row)

 ! Did we get the correct number of rows?

 if (i /= 11) stop 24

 ! Clean up

 cerr = odb_select_iterator_delete(odb_it)
 cerr = odb_read_delete(odb_handle)

end subroutine example_fortran_api2
end program example_fortran_api
