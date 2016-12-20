! (C) Copyright 1996-2012 ECMWF.
! 
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
! In applying this licence, ECMWF does not waive the privileges and immunities 
! granted to it by virtue of its status as an intergovernmental organisation nor
! does it submit to any jurisdiction.
!
! Piotr Kuchta, ECMWF, July 2016


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
 type(odbql_value)                             :: val
 integer(kind=C_INT)                           :: status, number_of_columns, column_no, row_no, int_val
 character(len=30)                             :: string_val, column_name
 character(len=1000)                           :: unparsed_sql
 real(kind=C_DOUBLE)                           :: v, real_val

!!!! Write to a file with SQL INSERT

 call odbql_open("", db)
 call odbql_prepare_v2(db, "CREATE TYPE BF_T AS (f1 bit1, f2 bit2);&
 & CREATE TABLE foo AS (x INTEGER,y REAL,v STRING,status BF_T) ON 'fort.odb';",-1, stmt, unparsed_sql)
 call odbql_prepare_v2(db, "INSERT INTO foo (x,y,v,status) VALUES (?,?,?,?);", -1, stmt, unparsed_sql)

! Populate first row with NULLs
 do column_no = 1,4
     call odbql_bind_null(stmt, column_no)
 end do
 call odbql_step(stmt)

! Write 3 rows with some values other than NULL
 do row_no = 1,3
    call odbql_bind_int(stmt, 1, 1 * row_no)
    v = 0.1 * row_no
    call odbql_bind_double(stmt, 2, v)
    call odbql_bind_text(stmt, 3, "hello", 5)
    call odbql_bind_int(stmt, 4, 1 * row_no)

    call odbql_step(stmt)
 end do

! Write internal buffers to disk and close the file
 call odbql_finalize(stmt)
 call odbql_close(db)

!!!! Read from a file with SQL SELECT

! Associate table with a file name
! call odbql_open("fort.odb", db)
! You could as well retrieve some data directly from MARS instead:
 call odbql_open("mars://RETRIEVE,CLASS=OD,TYPE=MFB,STREAM=OPER,EXPVER=0001,DATE=20160720,TIME=1200,DATABASE=marsod", db)
 call odbql_prepare_v2(db, "SELECT *;", -1, stmt, unparsed_sql)
 number_of_columns = odbql_column_count(stmt)
 write(0,*) "Number of columns: ", number_of_columns 

 row_no = 1
 do 
   call odbql_step(stmt, status)
   if (status == ODBQL_DONE) exit
   if (status /= ODBQL_ROW) stop 

   write(6,*) 'Row ', row_no
   do column_no = 1,number_of_columns

       call odbql_column_name(stmt, column_no, column_name)
       val = odbql_column_value(stmt, column_no)
       if (.not. c_associated(val%this)) then
           write(6,*) column_no, ' ', column_name, ': MISSING'
       else
           select case (odbql_column_type(stmt, column_no))
           case (ODBQL_TEXT)
               call odbql_column_text(stmt, column_no, string_val)
               write(6,*) column_no, ' ', column_name, ':string = ', string_val
           case (ODBQL_BITFIELD)
               int_val = odbql_value_int(val)
               write(6,*) column_no, ' ', column_name, ':integer = ', int_val
           case (ODBQL_INTEGER)
               int_val = odbql_value_int(val)
               write(6,*) column_no, ' ', column_name, ':integer = ', int_val
           case (ODBQL_FLOAT)
               real_val = odbql_value_double(val)
               write(6,*) column_no, ' ', column_name, ':float = ', real_val
           end select 

           ! Note, odbql_column_text can be called for columns of any type
           call odbql_column_text(stmt, column_no, string_val)
           write(6,*) column_no, ' ', column_name, ': ', string_val

       end if
   end do
   row_no = row_no + 1
 end do 

 call odbql_finalize(stmt)
 call odbql_close(db)

end subroutine odbql_fortran_example

end program example_fortran_api
