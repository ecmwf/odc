

!!!!! THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT MANUALLY !!!!!

module odbql_wrappers
  use odbql_binding
  use odbql_constants
  implicit none
  
  type odbql
    type(c_ptr) :: this
  end type

  type odbql_stmt
    type(c_ptr) :: this
  end type

contains



!> Helper function to convert C pointer to logical:

    function c_ptr_to_logical(ptr)

      use, intrinsic :: iso_c_binding, only: c_ptr

      type(c_ptr), intent(in)                       :: ptr
      logical                                       :: c_ptr_to_logical

      c_ptr_to_logical = c_associated(ptr)

    end function c_ptr_to_logical


!> Helper function to convert C '\0' terminated strings to Fortran strings

    function C_to_F_string(c_string_pointer) result(f_string)
      use, intrinsic :: iso_c_binding, only: c_ptr,c_f_pointer,c_char,c_null_char
      type(c_ptr), intent(in)                       :: c_string_pointer
      character(len=:), allocatable                 :: f_string
      character(kind=c_char), dimension(:), pointer :: char_array_pointer => null()
      character(len=255)                            :: aux_string
      integer                                       :: i,length

      call c_f_pointer(c_string_pointer,char_array_pointer,[255])
      if (.not.associated(char_array_pointer)) then
          allocate(character(len=4)::f_string)
          f_string = "NULL"
          return
      end if
      aux_string = " "
      do i=1,255
        if (char_array_pointer(i)==c_null_char) then
          length=i-1; exit
        end if
        aux_string(i:i)=char_array_pointer(i)
      end do
      allocate(character(len=length)::f_string)
      f_string = aux_string(1:length)
    end function C_to_F_string




!> const char * odbql_errmsg(odbql* db)

    subroutine odbql_errmsg (db,return_value) 
     use odbql_binding
     use, intrinsic                             :: iso_c_binding
     type(odbql), value                         :: db
     character(len=*), intent(out)              :: return_value

     

     
     return_value = C_to_F_string(odbql_errmsg_c(db%this))
     

    end subroutine odbql_errmsg

    

!> const char * odbql_libversion(void)

    subroutine odbql_libversion (return_value) 
     use odbql_binding
     use, intrinsic                             :: iso_c_binding
     
     character(len=*), intent(out)              :: return_value

     

     
     return_value = C_to_F_string(odbql_libversion_c())
     

    end subroutine odbql_libversion

    

!> error_code_t odbql_open(const char *filename, odbql **ppDb)

    subroutine odbql_open (filename,ppDb,status) 
     use odbql_binding
     use, intrinsic                             :: iso_c_binding
     character(len=*), intent(in)               :: filename
     type(odbql)                                :: ppDb
     integer(kind=C_INT), intent(out), optional :: status

     character(len=len_trim(filename)+1)        :: filename_tmp
     integer(kind=c_int)                        :: rc

     filename_tmp = filename//achar(0)
     rc = odbql_open_c(filename_tmp,ppDb%this)
     
     if (present(status)) then
         status = rc ! let user handle the error
     else
         if (rc /= ODBQL_OK .and. rc /= ODBQL_ROW .and. rc /= ODBQL_DONE) then
             write (0,*) 'Error in odbql_open'
             stop
         end if
     end if

    end subroutine odbql_open

    

!> error_code_t odbql_close(odbql* db)

    subroutine odbql_close (db,status) 
     use odbql_binding
     use, intrinsic                             :: iso_c_binding
     type(odbql), value                         :: db
     integer(kind=C_INT), intent(out), optional :: status

     
     integer(kind=c_int)                        :: rc

     
     rc = odbql_close_c(db%this)
     
     if (present(status)) then
         status = rc ! let user handle the error
     else
         if (rc /= ODBQL_OK .and. rc /= ODBQL_ROW .and. rc /= ODBQL_DONE) then
             write (0,*) 'Error in odbql_close'
             stop
         end if
     end if

    end subroutine odbql_close

    

!> error_code_t odbql_prepare_v2(odbql *db, const char *zSql, int nByte, odbql_stmt **ppStmt, const char **pzTail)

    subroutine odbql_prepare_v2 (db,zSql,nByte,ppStmt,pzTail,status) 
     use odbql_binding
     use, intrinsic                             :: iso_c_binding
     type(odbql), value                         :: db
     character(len=*), intent(in)               :: zSql
     integer(kind=C_INT), value                 :: nByte
     type(odbql_stmt)                           :: ppStmt
     character(len=*), intent(out)              :: pzTail
     integer(kind=C_INT), intent(out), optional :: status

     character(len=len_trim(zSql)+1)            :: zSql_tmp
     integer(kind=c_int)                        :: rc

     zSql_tmp = zSql//achar(0)
     rc = odbql_prepare_v2_c(db%this,zSql_tmp,nByte,ppStmt%this,pzTail)
     
     if (present(status)) then
         status = rc ! let user handle the error
     else
         if (rc /= ODBQL_OK .and. rc /= ODBQL_ROW .and. rc /= ODBQL_DONE) then
             write (0,*) 'Error in odbql_prepare_v2'
             stop
         end if
     end if

    end subroutine odbql_prepare_v2

    

!> error_code_t odbql_step(odbql_stmt* stmt)

    subroutine odbql_step (stmt,status) 
     use odbql_binding
     use, intrinsic                             :: iso_c_binding
     type(odbql_stmt), value                    :: stmt
     integer(kind=C_INT), intent(out), optional :: status

     
     integer(kind=c_int)                        :: rc

     
     rc = odbql_step_c(stmt%this)
     
     if (present(status)) then
         status = rc ! let user handle the error
     else
         if (rc /= ODBQL_OK .and. rc /= ODBQL_ROW .and. rc /= ODBQL_DONE) then
             write (0,*) 'Error in odbql_step'
             stop
         end if
     end if

    end subroutine odbql_step

    

!> error_code_t odbql_bind_double(odbql_stmt* stmt, int iCol, double v)

    subroutine odbql_bind_double (stmt,iCol,v,status) 
     use odbql_binding
     use, intrinsic                             :: iso_c_binding
     type(odbql_stmt), value                    :: stmt
     integer(kind=C_INT), value                 :: iCol
     real(kind=C_DOUBLE), value                 :: v
     integer(kind=C_INT), intent(out), optional :: status

     
     integer(kind=c_int)                        :: rc

     
     rc = odbql_bind_double_c(stmt%this,iCol-1,v)
     
     if (present(status)) then
         status = rc ! let user handle the error
     else
         if (rc /= ODBQL_OK .and. rc /= ODBQL_ROW .and. rc /= ODBQL_DONE) then
             write (0,*) 'Error in odbql_bind_double'
             stop
         end if
     end if

    end subroutine odbql_bind_double

    

!> error_code_t odbql_bind_int(odbql_stmt* stmt, int iCol, int v)

    subroutine odbql_bind_int (stmt,iCol,v,status) 
     use odbql_binding
     use, intrinsic                             :: iso_c_binding
     type(odbql_stmt), value                    :: stmt
     integer(kind=C_INT), value                 :: iCol
     integer(kind=C_INT), value                 :: v
     integer(kind=C_INT), intent(out), optional :: status

     
     integer(kind=c_int)                        :: rc

     
     rc = odbql_bind_int_c(stmt%this,iCol-1,v)
     
     if (present(status)) then
         status = rc ! let user handle the error
     else
         if (rc /= ODBQL_OK .and. rc /= ODBQL_ROW .and. rc /= ODBQL_DONE) then
             write (0,*) 'Error in odbql_bind_int'
             stop
         end if
     end if

    end subroutine odbql_bind_int

    

!> error_code_t odbql_bind_null(odbql_stmt* stmt, int iCol)

    subroutine odbql_bind_null (stmt,iCol,status) 
     use odbql_binding
     use, intrinsic                             :: iso_c_binding
     type(odbql_stmt), value                    :: stmt
     integer(kind=C_INT), value                 :: iCol
     integer(kind=C_INT), intent(out), optional :: status

     
     integer(kind=c_int)                        :: rc

     
     rc = odbql_bind_null_c(stmt%this,iCol-1)
     
     if (present(status)) then
         status = rc ! let user handle the error
     else
         if (rc /= ODBQL_OK .and. rc /= ODBQL_ROW .and. rc /= ODBQL_DONE) then
             write (0,*) 'Error in odbql_bind_null'
             stop
         end if
     end if

    end subroutine odbql_bind_null

    

!> error_code_t odbql_bind_text(odbql_stmt* stmt, int iCol, const char* s, int n, void(*d)(void*))

    subroutine odbql_bind_text (stmt,iCol,s,n,status) 
     use odbql_binding
     use, intrinsic                             :: iso_c_binding
     type(odbql_stmt), value                    :: stmt
     integer(kind=C_INT), value                 :: iCol
     character(len=*), intent(in)               :: s
     integer(kind=C_INT), value                 :: n
     integer(kind=C_INT), intent(out), optional :: status

     character(len=len_trim(s)+1)               :: s_tmp
     type(C_PTR)                                :: d
     integer(kind=c_int)                        :: rc

     s_tmp = s//achar(0)
     rc = odbql_bind_text_c(stmt%this,iCol-1,s_tmp,n,d)
     
     if (present(status)) then
         status = rc ! let user handle the error
     else
         if (rc /= ODBQL_OK .and. rc /= ODBQL_ROW .and. rc /= ODBQL_DONE) then
             write (0,*) 'Error in odbql_bind_text'
             stop
         end if
     end if

    end subroutine odbql_bind_text

    

!> const unsigned char *odbql_column_text(odbql_stmt* stmt, int iCol)

    subroutine odbql_column_text (stmt,iCol,return_value) 
     use odbql_binding
     use, intrinsic                             :: iso_c_binding
     type(odbql_stmt), value                    :: stmt
     integer(kind=C_INT), value                 :: iCol
     character(len=*), intent(out)              :: return_value

     

     
     return_value = C_to_F_string(odbql_column_text_c(stmt%this,iCol-1))
     

    end subroutine odbql_column_text

    

!> error_code_t odbql_finalize(odbql_stmt *stmt)

    subroutine odbql_finalize (stmt,status) 
     use odbql_binding
     use, intrinsic                             :: iso_c_binding
     type(odbql_stmt), value                    :: stmt
     integer(kind=C_INT), intent(out), optional :: status

     
     integer(kind=c_int)                        :: rc

     
     rc = odbql_finalize_c(stmt%this)
     
     if (present(status)) then
         status = rc ! let user handle the error
     else
         if (rc /= ODBQL_OK .and. rc /= ODBQL_ROW .and. rc /= ODBQL_DONE) then
             write (0,*) 'Error in odbql_finalize'
             stop
         end if
     end if

    end subroutine odbql_finalize

    

!> const char *odbql_column_name(odbql_stmt* stmt, int iCol)

    subroutine odbql_column_name (stmt,iCol,return_value) 
     use odbql_binding
     use, intrinsic                             :: iso_c_binding
     type(odbql_stmt), value                    :: stmt
     integer(kind=C_INT), value                 :: iCol
     character(len=*), intent(out)              :: return_value

     

     
     return_value = C_to_F_string(odbql_column_name_c(stmt%this,iCol-1))
     

    end subroutine odbql_column_name

    

!> int odbql_column_type(odbql_stmt* stmt, int iCol)

    function odbql_column_type (stmt,iCol) 
     use odbql_binding
     use, intrinsic                             :: iso_c_binding
     type(odbql_stmt), value                    :: stmt
     integer(kind=C_INT), value                 :: iCol
     integer(kind=C_INT)                        :: odbql_column_type

     

     
     odbql_column_type = odbql_column_type_c(stmt%this,iCol-1)
     

    end function odbql_column_type

    

!> odbql_value *odbql_column_value(odbql_stmt* stmt, int iCol)

    function odbql_column_value (stmt,iCol) 
     use odbql_binding
     use, intrinsic                             :: iso_c_binding
     type(odbql_stmt), value                    :: stmt
     integer(kind=C_INT), value                 :: iCol
     logical                                    :: odbql_column_value

     

     
     odbql_column_value = c_ptr_to_logical(odbql_column_value_c(stmt%this,iCol-1))
     

    end function odbql_column_value

    

!> int odbql_column_count(odbql_stmt *stmt)

    function odbql_column_count (stmt) 
     use odbql_binding
     use, intrinsic                             :: iso_c_binding
     type(odbql_stmt), value                    :: stmt
     integer(kind=C_INT)                        :: odbql_column_count

     

     
     odbql_column_count = odbql_column_count_c(stmt%this)
     

    end function odbql_column_count

    

end module odbql_wrappers
