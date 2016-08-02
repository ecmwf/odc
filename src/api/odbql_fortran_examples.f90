! (C) Copyright 1996-2012 ECMWF.
! 
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
! In applying this licence, ECMWF does not waive the privileges and immunities 
! granted to it by virtue of its status as an intergovernmental organisation nor
! does it submit to any jurisdiction.
!
! Piotr Kuchta  July 2016


program example_fortran_api
  use odbql_wrappers
  implicit none
  character(len=10)                            :: version

  call odbql_libversion(version)
  write(0,*) "This program is linked to ODB API version: ", version

  call odbql_fortran_example()

  write(0,*) "That's all, folks!"
contains

subroutine odbql_fortran_example
 implicit none
 type(odbql)                                   :: db
 type(odbql_stmt)                              :: stmt
 integer(kind=C_INT)                           :: rc, number_of_columns, column, row
 character(len=30)                             :: val, column_name
 character(len=1000)                           :: unparsed_sql
 real(kind=C_DOUBLE)                           :: v

!!!! Write to a file with INSERT

 call odbql_open("CREATE TYPE BF_T AS (f1 bit1, f2 bit2); CREATE TABLE foo AS (x INTEGER, y REAL, v STRING, status BF_T) ON 'fort.odb';", db)
 call odbql_prepare_v2(db, "INSERT INTO foo (x,y,v,status) VALUES (?,?,?,?);", -1, stmt, unparsed_sql)

! Populate first row with NULLs
 do column = 1,4
     call odbql_bind_null(stmt, column)
 end do
 rc = odbql_step(stmt)

! Write 3 rows with some values other than NULL
 do row = 1,3
    call odbql_bind_int(stmt, 1, 1 * row)
    v = 0.1 * row
    call odbql_bind_double(stmt, 2, v)
    call odbql_bind_text(stmt, 3, "hello", 5)
    call odbql_bind_int(stmt, 4, 1 * row)

    rc = odbql_step(stmt)
 end do

 call odbql_finalize(stmt)
 call odbql_close(db)

! Associate table with a file name
 call odbql_open("CREATE TABLE foo ON 'fort.odb';", db)
! You could as well retrieve some data directly from MARS instead:
! call odbql_open("CREATE TABLE foo ON 'mars://RETRIEVE,CLASS=OD,TYPE=MFB,STREAM=OPER,EXPVER=0001,DATE=20160720,TIME=1200,DATABASE=marsod';", db)
 call odbql_prepare_v2(db, "SELECT * FROM foo;", -1, stmt, unparsed_sql)
 number_of_columns = odbql_column_count(stmt)
 write(0,*) "Number of columns: ", number_of_columns 

 row = 0
 do 
   rc = odbql_step(stmt)
   if (rc == ODBQL_DONE) exit
   if (rc /= ODBQL_ROW) stop 

   write(6,*) 'Row ', row
   do column = 1,number_of_columns

       call odbql_column_name(stmt, column, column_name)
       if (odbql_column_value(stmt, column)) then
           call odbql_column_text(stmt, column, val)
           write(6,*) column, ' ', column_name, ': ', val
       else
           write(6,*) column, ' ', column_name, ': MISSING'
       end if
   end do
   row = row + 1
 end do 

 call odbql_finalize(stmt)
 call odbql_close(db)

end subroutine odbql_fortran_example

end program example_fortran_api
