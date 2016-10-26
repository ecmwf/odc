! (C) Copyright 1996-2012 ECMWF.
! 
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
! In applying this licence, ECMWF does not waive the privileges and immunities 
! granted to it by virtue of its status as an intergovernmental organisation nor
! does it submit to any jurisdiction.
!

program test_fortran_api
  use, intrinsic :: iso_c_binding
  use odb_c_binding
  implicit none

  integer, parameter :: max_varlen = 128
  integer(kind=4)    :: ncolumns = 4

  write(0,*) "Calling odb_start..."

  call odb_start()

  call test_fortran_api_non_existing_file()

contains

subroutine test_fortran_api_non_existing_file
 implicit none
 type(C_PTR)                                   :: odb_handle, odb_it, odb_select_handle, odb_select_it
 integer(kind=C_INT)                           :: cerr
 character(kind=C_CHAR, len=64)                :: config = C_NULL_CHAR
 character(kind=C_CHAR, len=max_varlen)        :: inputfile = "test_fortran_api_non_existing_file.odb"//achar(0)
 character(kind=C_CHAR, len=128)               :: sql='select * from "test_fortran_api_non_existing_file.odb";'//achar(0)

 write(0,*) 'test_fortran_api_non_existing_file'
 odb_handle = odb_read_new(config, cerr)
 odb_it = odb_read_iterator_new(odb_handle, inputfile, cerr);
 ! This should fail
 write(0,*) 'test_fortran_api_non_existing_file: odb_read_iterator_new => ', cerr
 if (cerr == 0) STOP 1

 odb_handle = odb_select_new(config, cerr)
 odb_it = odb_select_iterator_new(odb_handle, sql, cerr);
 if (cerr == 0) STOP 1
end subroutine test_fortran_api_non_existing_file

end program test_fortran_api
