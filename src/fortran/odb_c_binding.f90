! © Copyright 1996-2012 ECMWF.
! 
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
! In applying this licence, ECMWF does not waive the privileges and immunities 
! granted to it by virtue of its status as an intergovernmental organisation nor
! does it submit to any jurisdiction.
!

!------------------------------------------------------------------------------
! ECMWF
!------------------------------------------------------------------------------
!
! MODULE: odb_c_binding
!
!> @author Anne Fouilloux, ECMWF
!
! DESCRIPTION: 
!> Provides Fortran bindings for ODB API
!
! REVISION HISTORY:
! DD Mmm YYYY - Initial Version
! TODO_dd_mmm_yyyy - TODO_describe_appropriate_changes - TODO_name
!------------------------------------------------------------------------------

module odb_c_binding
  use iso_c_binding
  use, intrinsic :: iso_c_binding
  implicit none
  integer, parameter :: ODB_IGNORE=0
  integer, parameter :: ODB_INTEGER=1
  integer, parameter :: ODB_REAL=2
  integer, parameter :: ODB_STRING=3
  integer, parameter :: ODB_BITFIELD=4
interface 

!> Initialize ODB API. This function must be called before any other function from the ODB API.
   subroutine odb_start() bind(C, name = "odb_start")
   end subroutine

!> Counts number of rows in an ODB file.
!>
!> @param[in] filename  name of the file
!> @return number       of rows on file
   function odb_count(filename) bind(C, name="odb_count")
     use, intrinsic                       :: iso_c_binding
     character(kind=C_CHAR),dimension(*)  :: filename
     real(kind=C_DOUBLE)                  :: odb_count
   end function odb_count

   function odb_read_new(config, err) bind(C, name = "odb_create")
     use, intrinsic                       :: iso_c_binding
     character(kind=C_CHAR),dimension(*)  :: config
     integer(kind=C_INT)                  :: err
     type(C_PTR)                          :: odb_read_new
   end function odb_read_new

   function odb_read_delete(odb) bind(C, name = "odb_destroy")
     use, intrinsic                       :: iso_c_binding
     type(C_PTR),VALUE                    :: odb
     integer(kind=C_INT)                  :: odb_read_delete
   end function odb_read_delete


!> Create new read iterator.
   function odb_read_iterator_new(odb, filename, err) bind(C, name="odb_create_read_iterator")
     use, intrinsic                       :: iso_c_binding
     type(C_PTR), VALUE                   :: odb
     character(kind=C_CHAR),dimension(*)  :: filename
     integer(kind=C_INT)                  :: err
     type(C_PTR)                          :: odb_read_iterator_new
   end function odb_read_iterator_new

   function odb_read_iterator_delete(odb_iterator) bind(C, name="odb_read_iterator_destroy")
     use, intrinsic                       :: iso_c_binding
     type(C_PTR), VALUE                   :: odb_iterator
     integer(kind=C_INT)                  :: odb_read_iterator_delete
   end function odb_read_iterator_delete

   function odb_read_get_no_of_columns(odb_iterator, ncols) bind(C, name="odb_read_iterator_get_no_of_columns")
     use, intrinsic                       :: iso_c_binding
     type(C_PTR),VALUE                    :: odb_iterator
     integer(kind=C_INT)                  :: ncols
     integer(kind=C_INT)                  :: odb_read_get_no_of_columns ! return an error code
   end function odb_read_get_no_of_columns

   function odb_read_get_column_type(odb_iterator, n, type) bind(C, name="odb_read_iterator_get_column_type")
     use, intrinsic                       :: iso_c_binding
     type(C_PTR), VALUE                   :: odb_iterator
     integer(kind=C_INT),VALUE            :: n
     integer(kind=C_INT)                  :: type
     integer(kind=C_INT)                  :: odb_read_get_column_type
   end function odb_read_get_column_type

   function odb_read_get_column_name(odb_iterator, n, colname, nchar) bind(C, name="odb_read_iterator_get_column_name")
     use, intrinsic                       :: iso_c_binding
     type(C_PTR), VALUE                   :: odb_iterator
     integer(kind=C_INT), VALUE           :: n
     integer(kind=C_INT)                  :: nchar
     type(C_PTR)                          :: colname
     integer(kind=C_INT)                  :: odb_read_get_column_name
   end function odb_read_get_column_name

   function odb_read_get_next_row(odb_iterator, count, data, new_dataset) bind(C, name="odb_read_iterator_get_next_row")
     use, intrinsic                       :: iso_c_binding
     type(C_PTR), VALUE                   :: odb_iterator
     integer(kind=C_INT), VALUE           :: count
     integer(kind=C_INT)                  :: new_dataset
     real(kind=C_DOUBLE),dimension(*)     :: data
     integer(kind=C_INT)                  :: odb_read_get_next_row
   end function odb_read_get_next_row

  
! SELECT ITERATOR
   function odb_select_iterator_new(odb, sql, err) bind(C, name="odb_create_select_iterator")
     use, intrinsic                       :: iso_c_binding
     type(C_PTR), VALUE                   :: odb
     character(kind=C_CHAR),dimension(*)  :: sql
     integer(kind=C_INT)                  :: err
     type(C_PTR)                          :: odb_select_iterator_new
   end function odb_select_iterator_new

   function odb_select_iterator_new_from_file(odb, sql, filename, err) bind(C, name="odb_create_select_iterator_from_file")
     use, intrinsic                       :: iso_c_binding
     type(C_PTR), VALUE                   :: odb
     character(kind=C_CHAR),dimension(*)  :: sql, filename
     integer(kind=C_INT)                  :: err
     type(C_PTR)                          :: odb_select_iterator_new_from_file
   end function odb_select_iterator_new_from_file

   function odb_select_iterator_delete(odb_iterator) bind(C, name="odb_select_iterator_destroy")
     use, intrinsic                       :: iso_c_binding
     type(C_PTR), VALUE                   :: odb_iterator
     integer(kind=C_INT)                  :: odb_select_iterator_delete
   end function odb_select_iterator_delete

   function odb_select_get_no_of_columns(odb_iterator, ncols) bind(C, name="odb_select_iterator_get_no_of_columns")
     use, intrinsic                       :: iso_c_binding
     type(C_PTR),VALUE                    :: odb_iterator
     integer(kind=C_INT)                  :: ncols
     integer(kind=C_INT)                  :: odb_select_get_no_of_columns ! return an error code
   end function odb_select_get_no_of_columns

   function odb_select_get_column_type(odb_iterator, n, type) bind(C, name="odb_select_iterator_get_column_type")
     use, intrinsic                       :: iso_c_binding
     type(C_PTR), VALUE                   :: odb_iterator
     integer(kind=C_INT),VALUE            :: n
     integer(kind=C_INT)                  :: type
     integer(kind=C_INT)                  :: odb_select_get_column_type
   end function odb_select_get_column_type

   function odb_select_get_column_name(odb_iterator, n, colname, nchar) bind(C, name="odb_select_iterator_get_column_name")
     use, intrinsic                       :: iso_c_binding
     type(C_PTR), VALUE                   :: odb_iterator
     integer(kind=C_INT), VALUE           :: n
     type(C_PTR)                          :: colname
     integer(kind=C_INT)                  :: nchar
     integer(kind=C_INT)                  :: odb_select_get_column_name
   end function odb_select_get_column_name

   function odb_select_get_next_row(odb_iterator, count, data, new_dataset) bind(C, name="odb_select_iterator_get_next_row")
     use, intrinsic                       :: iso_c_binding
     type(C_PTR), VALUE                   :: odb_iterator
     integer(kind=C_INT), VALUE           :: count
     integer(kind=C_INT)                  :: new_dataset
     real(kind=C_DOUBLE),dimension(*)     :: data
     integer(kind=C_INT)                  :: odb_select_get_next_row
   end function odb_select_get_next_row

  
! WRITE
   function odb_write_new(config, err) bind(C, name = "odb_writer_create")
     use, intrinsic                       :: iso_c_binding
     character(kind=C_CHAR),dimension(*)  :: config
     integer(kind=C_INT)                  :: err
     type(C_PTR)                          :: odb_write_new
   end function odb_write_new

   function odb_write_delete(odb) bind(C, name = "odb_writer_destroy")
     use, intrinsic                       :: iso_c_binding
     type(C_PTR),VALUE                    :: odb
     integer(kind=C_INT)                  :: odb_write_delete
   end function odb_write_delete

! WRITE iterator
   function odb_write_iterator_new(odb, filename, err) bind(C, name="odb_create_write_iterator")
     use, intrinsic                       :: iso_c_binding
     type(C_PTR), VALUE                   :: odb
     character(kind=C_CHAR),dimension(*)  :: filename
     integer(kind=C_INT)                  :: err
     type(C_PTR)                          :: odb_write_iterator_new
   end function odb_write_iterator_new

   function odb_append_iterator_new(odb, filename, err) bind(C, name="odb_create_append_iterator")
     use, intrinsic                       :: iso_c_binding
     type(C_PTR), VALUE                   :: odb
     character(kind=C_CHAR),dimension(*)  :: filename
     integer(kind=C_INT)                  :: err
     type(C_PTR)                          :: odb_append_iterator_new
   end function odb_append_iterator_new

   function odb_write_iterator_delete(odb_iterator) bind(C, name="odb_write_iterator_destroy")
     use, intrinsic                       :: iso_c_binding
     type(C_PTR), VALUE                   :: odb_iterator
     integer(kind=C_INT)                  :: odb_write_iterator_delete
   end function odb_write_iterator_delete

   function odb_write_set_no_of_columns(odb_iterator, ncols) bind(C, name="odb_write_iterator_set_no_of_columns")
     use, intrinsic                       :: iso_c_binding
     type(C_PTR),VALUE                    :: odb_iterator
     integer(kind=C_INT), VALUE           :: ncols
     integer(kind=C_INT)                  :: odb_write_set_no_of_columns ! return an error code
   end function odb_write_set_no_of_columns

   function odb_write_set_column(odb_iterator, n, type, colname) bind(C, name="odb_write_iterator_set_column")
     use, intrinsic                       :: iso_c_binding
     type(C_PTR), VALUE                   :: odb_iterator
     integer(kind=C_INT),VALUE            :: n
     integer(kind=C_INT),VALUE            :: type
     character(kind=C_CHAR),dimension(*)  :: colname
     integer(kind=C_INT)                  :: odb_write_set_column
   end function odb_write_set_column

   function odb_write_set_bitfield(odb_iterator, n, type, colname, bitfield_names, bitfield_sizes) &
 bind(C, name="odb_write_iterator_set_bitfield")
     use, intrinsic                       :: iso_c_binding
     type(C_PTR), VALUE                   :: odb_iterator
     integer(kind=C_INT),VALUE            :: n
     integer(kind=C_INT),VALUE            :: type
     character(kind=C_CHAR),dimension(*)  :: colname
     character(kind=C_CHAR),dimension(*)  :: bitfield_names, bitfield_sizes
     integer(kind=C_INT)                  :: odb_write_set_bitfield
   end function odb_write_set_bitfield

   function odb_write_set_missing_value(odb_iterator, n, v) bind(C, name="odb_write_iterator_set_missing_value")
     use, intrinsic                       :: iso_c_binding
     type(C_PTR), VALUE                   :: odb_iterator
     integer(kind=C_INT),VALUE            :: n
     real(kind=C_DOUBLE),VALUE            :: v
     integer(kind=C_INT)                  :: odb_write_set_missing_value
   end function odb_write_set_missing_value

   function odb_write_set_next_row(odb_iterator, data, count) bind(C, name="odb_write_iterator_set_next_row")
     use, intrinsic                       :: iso_c_binding
     type(C_PTR), VALUE                   :: odb_iterator
     integer(kind=C_INT), VALUE           :: count
     real(kind=C_DOUBLE),dimension(*)     :: data
     integer(kind=C_INT)                  :: odb_write_set_next_row
   end function odb_write_set_next_row

   function odb_write_header(odb_iterator) bind(C, name="odb_write_iterator_write_header")
     use, intrinsic                       :: iso_c_binding
     type(C_PTR), VALUE                   :: odb_iterator
     integer(kind=C_INT)                  :: odb_write_header
   end function odb_write_header

end interface
end module odb_c_binding
