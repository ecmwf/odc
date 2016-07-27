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
  use odbql_wrappers
  implicit none
  character(len=255)                            :: s

  integer(kind=4)    :: ncolumns = 5

  s = ""
  write(0,*) "Here we go..."

  call odbql_libversion(s)

  call odbql_fortran_example()

contains

subroutine odbql_fortran_example
 implicit none
 type(C_PTR)                                   :: db, stmt
 integer(kind=C_INT)                           :: rc, number_of_columns, i
 character(len=255)                            :: val, unparsed_sql

 write(0,*) 'odbql_fortran_example'

 rc = odbql_open("CREATE TABLE foo ON 'example_select_data_read_results.odb';", db)
 rc = odbql_prepare_v2(db, "SELECT * FROM foo;", -1, stmt, unparsed_sql)
 number_of_columns = odbql_column_count(stmt)

 rc = odbql_step(stmt)

 do i=2,number_of_columns
     call odbql_column_text(stmt, i, val)
     write(0,*) ' ', val
 enddo

 rc = odbql_finalize(stmt)
 rc = odbql_close(db)

end subroutine odbql_fortran_example

end program example_fortran_api
