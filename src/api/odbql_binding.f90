

!!!!! THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT MANUALLY !!!!!

module odbql_binding
  use iso_c_binding
  use, intrinsic :: iso_c_binding
  implicit none
interface

!> const char * odbql_errmsg(odbql* db)

    function odbql_errmsg_c (db) bind(C, name="odbql_errmsg")
     use, intrinsic                       :: iso_c_binding
     type(C_PTR), VALUE                   :: db
     type(C_PTR)                          :: odbql_errmsg_c
    end function odbql_errmsg_c


    

!> const char * odbql_libversion(void)

    function odbql_libversion_c () bind(C, name="odbql_libversion")
     use, intrinsic                       :: iso_c_binding
     
     type(C_PTR)                          :: odbql_libversion_c
    end function odbql_libversion_c


    

!> int odbql_open(const char *filename, odbql **ppDb)

    function odbql_open_c (filename,ppDb) bind(C, name="odbql_open")
     use, intrinsic                       :: iso_c_binding
     character(kind=C_CHAR),dimension(*)  :: filename
     type(C_PTR)                          :: ppDb
     integer(kind=C_INT)                  :: odbql_open_c
    end function odbql_open_c


    

!> int odbql_close(odbql* db)

    function odbql_close_c (db) bind(C, name="odbql_close")
     use, intrinsic                       :: iso_c_binding
     type(C_PTR), VALUE                   :: db
     integer(kind=C_INT)                  :: odbql_close_c
    end function odbql_close_c


    

!> int odbql_prepare_v2(odbql *db, const char *zSql, int nByte, odbql_stmt **ppStmt, const char **pzTail)

    function odbql_prepare_v2_c (db,zSql,nByte,ppStmt,pzTail) bind(C, name="odbql_prepare_v2")
     use, intrinsic                       :: iso_c_binding
     type(C_PTR), VALUE                   :: db
     character(kind=C_CHAR),dimension(*)  :: zSql
     integer(kind=C_INT), VALUE           :: nByte
     type(C_PTR)                          :: ppStmt
     character(kind=C_CHAR),dimension(*)  :: pzTail
     integer(kind=C_INT)                  :: odbql_prepare_v2_c
    end function odbql_prepare_v2_c


    

!> int odbql_step(odbql_stmt* stmt)

    function odbql_step_c (stmt) bind(C, name="odbql_step")
     use, intrinsic                       :: iso_c_binding
     type(C_PTR), VALUE                   :: stmt
     integer(kind=C_INT)                  :: odbql_step_c
    end function odbql_step_c


    

!> int odbql_bind_double(odbql_stmt* stmt, int i, double v)

    function odbql_bind_double_c (stmt,i,v) bind(C, name="odbql_bind_double")
     use, intrinsic                       :: iso_c_binding
     type(C_PTR), VALUE                   :: stmt
     integer(kind=C_INT), VALUE           :: i
     real(kind=C_DOUBLE), VALUE           :: v
     integer(kind=C_INT)                  :: odbql_bind_double_c
    end function odbql_bind_double_c


    

!> int odbql_bind_int(odbql_stmt* stmt, int i, int v)

    function odbql_bind_int_c (stmt,i,v) bind(C, name="odbql_bind_int")
     use, intrinsic                       :: iso_c_binding
     type(C_PTR), VALUE                   :: stmt
     integer(kind=C_INT), VALUE           :: i
     integer(kind=C_INT), VALUE           :: v
     integer(kind=C_INT)                  :: odbql_bind_int_c
    end function odbql_bind_int_c


    

!> int odbql_bind_null(odbql_stmt* stmt, int i)

    function odbql_bind_null_c (stmt,i) bind(C, name="odbql_bind_null")
     use, intrinsic                       :: iso_c_binding
     type(C_PTR), VALUE                   :: stmt
     integer(kind=C_INT), VALUE           :: i
     integer(kind=C_INT)                  :: odbql_bind_null_c
    end function odbql_bind_null_c


    

!> int odbql_bind_text(odbql_stmt* stmt, int i, const char* s, int n, void(*d)(void*))

    function odbql_bind_text_c (stmt,i,s,n,d) bind(C, name="odbql_bind_text")
     use, intrinsic                       :: iso_c_binding
     type(C_PTR), VALUE                   :: stmt
     integer(kind=C_INT), VALUE           :: i
     character(kind=C_CHAR),dimension(*)  :: s
     integer(kind=C_INT), VALUE           :: n
     type(C_PTR), VALUE                   :: d
     integer(kind=C_INT)                  :: odbql_bind_text_c
    end function odbql_bind_text_c


    

!> const unsigned char *odbql_column_text(odbql_stmt* stmt, int column)

    function odbql_column_text_c (stmt,column) bind(C, name="odbql_column_text")
     use, intrinsic                       :: iso_c_binding
     type(C_PTR), VALUE                   :: stmt
     integer(kind=C_INT), VALUE           :: column
     type(C_PTR)                          :: odbql_column_text_c
    end function odbql_column_text_c


    

!> int odbql_finalize(odbql_stmt *stmt)

    function odbql_finalize_c (stmt) bind(C, name="odbql_finalize")
     use, intrinsic                       :: iso_c_binding
     type(C_PTR), VALUE                   :: stmt
     integer(kind=C_INT)                  :: odbql_finalize_c
    end function odbql_finalize_c


    

!> const char *odbql_column_name(odbql_stmt* stmt, int iCol)

    function odbql_column_name_c (stmt,iCol) bind(C, name="odbql_column_name")
     use, intrinsic                       :: iso_c_binding
     type(C_PTR), VALUE                   :: stmt
     integer(kind=C_INT), VALUE           :: iCol
     type(C_PTR)                          :: odbql_column_name_c
    end function odbql_column_name_c


    

!> int odbql_column_type(odbql_stmt* stmt, int iCol)

    function odbql_column_type_c (stmt,iCol) bind(C, name="odbql_column_type")
     use, intrinsic                       :: iso_c_binding
     type(C_PTR), VALUE                   :: stmt
     integer(kind=C_INT), VALUE           :: iCol
     integer(kind=C_INT)                  :: odbql_column_type_c
    end function odbql_column_type_c


    

!> int odbql_column_count(odbql_stmt *stmt)

    function odbql_column_count_c (stmt) bind(C, name="odbql_column_count")
     use, intrinsic                       :: iso_c_binding
     type(C_PTR), VALUE                   :: stmt
     integer(kind=C_INT)                  :: odbql_column_count_c
    end function odbql_column_count_c


    

end interface
end module odbql_binding
