! (C) Copyright 1996-2012 ECMWF.
! 
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
! In applying this licence, ECMWF does not waive the privileges and immunities 
! granted to it by virtue of its status as an intergovernmental organisation nor
! does it submit to any jurisdiction.
!

program test_client_lib_fortran_server_side_ecml
  use, intrinsic :: iso_c_binding
  use odb_c_binding
  implicit none

  integer, parameter :: max_varlen = 128
  integer(kind=4)    :: ncolumns 

  write(0,*) "Calling odb_start..."
  call odb_start()

  call example_fortran_api_retrieve_server_side()

contains

subroutine example_fortran_api_retrieve_server_side
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
 integer(kind=4)                               :: i, ci, row_count

 character(kind=C_CHAR, len=2048)              :: sql='select * from &
 & "mars://retrieve,class=OD,date=20150218,time=1200,type=OFB,&
 & obsgroup=conv,reportype=16058,stream=oper,expver=qu12,&
 & server_side=(function,of=source,_=(sql,filter=\"select count(*)\",target=(temporary_file))),&
 & database=localhost";'//achar(0)
 integer(kind=C_INT)                           :: itype, newdataset, c_ncolumns=1, size_name 
 integer(kind=C_INT)                           :: bitfield_names_size, bitfield_sizes_size
 real(kind=C_DOUBLE), dimension(:), allocatable:: one_row
 character(len=8)                              :: tmp_str

 write(0,*) 'example_fortran_api_retrieve: ', sql

 odb_handle = odb_select_new(config, cerr)
 if (cerr /=0) STOP 1

 odb_it = odb_select_iterator_new(odb_handle, sql, cerr);
 if (cerr /=0) STOP 1
 
 cerr = odb_select_get_no_of_columns(odb_it, ncolumns)
 if (cerr /=0) STOP 1
 write(0,*) '-=-=-=-=-= example_fortran_api_retrieve: number of columns: ', ncolumns
 if (c_ncolumns /= ncolumns) STOP 2
 
 do ci=0, ncolumns - 1
     cerr = odb_select_get_column_name(odb_it, ci, ptr_colname, size_name)
     if (cerr /=0) STOP 1
     call C_F_POINTER(CPTR=ptr_colname, FPTR=f_ptr_colname, shape=(/size_name/));
     do i=1, size_name 
        colname(i:i)  = f_ptr_colname(i)
     end do
     write(0,*) ' : ', colname(1:i)
 end do

 allocate(one_row(c_ncolumns))
 cerr=0
 i = 0
 do 
   cerr = odb_select_get_next_row(odb_it, c_ncolumns, one_row, newdataset)
   if ( cerr /= 0) exit
   row_count = one_row(1)
   i = i + 1
 enddo
 deallocate(one_row)

 if (i /= 1) STOP 3

 write(0,*) '-=-=-=-=-= example_fortran_api_retrieve: number of rows calculated on the server: ', row_count
 if (row_count /= 9380) STOP 33

 cerr = odb_select_iterator_delete(odb_it)
 cerr = odb_read_delete(odb_handle)

end subroutine example_fortran_api_retrieve_server_side
end program test_client_lib_fortran_server_side_ecml
