! (C) Copyright 1996-2017 ECMWF.
! 
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
! In applying this licence, ECMWF does not waive the privileges and immunities 
! granted to it by virtue of its status as an intergovernmental organisation nor
! does it submit to any jurisdiction.
!
! Piotr Kuchta, ECMWF, April 2017


program test_regression
  use odbql_wrappers
  implicit none
  character(len=10)                            :: version

  call odbql_libversion(version)
  write(0,*) "This program is linked to ODB API version: ", version

  call test_odb127()

  write(0,*) "Phew, made it! All regression tests passed."
contains

subroutine test_odb127
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
 call odbql_prepare_v2(db, "CREATE TABLE foo AS (varno INTEGER,obsvalue REAL) ON 'odb127f.odb';", &
 & -1, stmt, unparsed_sql)
 call odbql_prepare_v2(db, "INSERT INTO foo (varno,obsvalue) VALUES (?,?);", -1, stmt, unparsed_sql)

! Populate first row with NULLs
 call odbql_bind_null(stmt, 1)
 call odbql_bind_null(stmt, 2)
 call odbql_step(stmt)

! Write 3 rows with some values other than NULL
 do row_no = 1,1000
    call odbql_bind_int(stmt, 1, row_no)
    v = 0.1 * row_no
    call odbql_bind_double(stmt, 2, v)

    call odbql_step(stmt)
 end do

! Write internal buffers to disk and close the file
 call odbql_finalize(stmt)
 call odbql_close(db)

!!!! Read from a file with SQL SELECT

! Associate table with a file name
 call odbql_open("", db)
 call odbql_prepare_v2(db, "SELECT varno FROM 'odb127f.odb' WHERE varno = $ps;", &
  & -1, stmt, unparsed_sql, status)
 if (status == ODBQL_DONE) stop 1
 number_of_columns = odbql_column_count(stmt)
 write(0,*) "Number of columns: ", number_of_columns 

 call odbql_step(stmt, status)
 if (status /= ODBQL_ROW) stop 2

 val = odbql_column_value(stmt, 1)
 if (.not. c_associated(val%this)) stop 3
 
 int_val = odbql_value_double(val)
 write(6,*) 'VALUE ', int_val
 if (int_val /= 110 ) stop 4
       
 call odbql_finalize(stmt)
 call odbql_close(db)

end subroutine test_odb127

end program test_regression
