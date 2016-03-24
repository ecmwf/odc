!> @file   odbi.F90
!> @author Anne Fouilloux

module odbi

  use iso_c_binding
  use odb_c_binding
  use odb_module
  USE YOMHOOK   ,ONLY : LHOOK,   DR_HOOK

  implicit none
  integer, parameter :: MAX_STRING=1024
  integer, parameter :: ODB_MAX_ENTRYNO = 1000
  integer, parameter :: ODB_MAX_ROWS = 100000

  type t_odbtable
    character(len=maxvarlen)                         :: tablename
    integer(kind=JPIM)                               :: table_kind
    real(kind=JPRB), pointer                         :: data(:,:)
    character(len=MAX_STRING), dimension(:), pointer :: colnames
    integer(kind=JPIM), dimension(:), pointer        :: index
  end type t_odbtable
  type t_bufr2odb
    logical                                          :: LLcreate_odb_header
    type(C_PTR)                                      :: c_handle
    type(C_PTR)                                      :: c_odb_it
    integer(kind=JPIM)                               :: handle
    integer(kind=C_INT)                              :: c_ncolumns
    character(len=MAX_STRING), dimension(:), pointer :: ODBcolname
    integer(kind=JPIM), dimension(:), pointer        :: ODBcoltype
    integer(kind=JPIM)                               :: nrows
  end type t_bufr2odb

  type t_odb2odb1
    character(len=MAX_STRING)              :: tablename
    logical                                :: aligned_header
    logical                                :: aligned_body
    integer(kind=JPIM), pointer            :: ODB1index(:)
    character(len=MAX_STRING), pointer     :: colnames(:)
    logical, pointer                       :: col_is_len(:)
    logical, pointer                       :: child_is_body(:)
    type(t_odb2odb1), pointer              :: next
  end type t_odb2odb1


  type relative
     integer(kind=JPIM)                                   :: idx     ! index dans dbinfo%tables
     type(relative), pointer                              :: next
  end type relative

  type tableinfo
     character(len=MAX_STRING)                            :: ODBtablename  ! ODB table name @table i.e. contains "@"
     integer(JPIM)                                        :: ODBncols      ! number of ODB columns
     character(len=MAX_STRING), dimension(:), allocatable :: ODBcolname
     character(len=MAX_STRING), dimension(:), allocatable :: ODBtypename
     integer(kind=JPIM), dimension(:), allocatable        :: ODB2ODAindex
     integer(kind=JPIM)                                   :: ODAncols  ! if 0 it means this table is not present in ODA
     real(kind=JPRB), dimension(:,:), allocatable         :: ODBrow
     real(kind=JPRB), dimension(:), allocatable           :: ODBonerow
     integer(kind=JPIM)                                   :: ODBnrows
     integer(kind=JPIM)                                   :: nchildren
     logical                                              :: cut ! true if we need to cut
     logical                                              :: done ! true if already proceeded
     logical                                              :: ODBwrite ! true if we need to write in ODB
     integer(kind=JPIM)                                   :: len_idx ! index in ODBcol of LEN
     integer(kind=JPIM)                                   :: offset_idx ! index in ODBcol of OFFSET
     type(relative), pointer                              :: child
     type(relative), pointer                              :: aligned
     type(relative), pointer                              :: onelooper
     type(relative), pointer                              :: parent
  end type tableinfo

  type dbinfo
     character(len=MAX_STRING)                  :: dbname
     integer(kind = JPIM)                       :: handle
     integer(kind = JPIM)                       :: ntables
     integer(kind = JPIM)                       :: npools
     integer(kind = JPIM)                       :: current_poolno
     integer(kind = JPIM)                       :: current_seqno
     integer(kind = JPIM)                       :: idx_seqno ! idx in ODA
     integer(kind = JPIM)                       :: ODAncols ! total number of columns in ODA; can be less or greater than the number of column in ODB
     character(len=MAX_STRING), dimension(:), allocatable :: ODAcolname
     real(kind=JPRB)                            :: mdi
     integer(kind=JPIM)                         :: master       ! root table (desc)
     type(tableinfo), dimension(:), allocatable :: tables
  end type dbinfo

contains
function get_coltype(khandle, colnames, types, ftntypes, one_colname, itype) RESULT(idx_col)
  implicit none
  INTEGER(KIND=JPIM), intent(in)         :: khandle
  character(len=*),intent(in)           :: colnames(:)
  character(len=maxvarlen), intent(in)  :: types(:), ftntypes(:)
  character(len=*), intent(in)          :: one_colname
  integer(kind=C_INT), intent(out)      :: itype

  integer(kind=JPIM)                    :: idx_col, jc
  real(kind=JPRB)                       :: mdi

  REAL(KIND=JPRB)                       :: ZHOOK_HANDLE

  IF (LHOOK) CALL DR_HOOK('GET_COLTYPE',0,ZHOOK_HANDLE)
  mdi = abs(ODB_getval(khandle, '$mdi'))

  idx_col = -1
  do jc=1, size(colnames)
    if (trim(colnames(jc)) == trim(one_colname)) then
      idx_col = jc
      exit
    endif
  end do

  if ( ftntypes(idx_col)(1:7) == 'REAL(8)' .and.&
  &       types(idx_col)(1:7) == 'string ' ) then
      itype = ODB_STRING
  else if ( ftntypes(idx_col)(1:7) == 'CHAR(8)') then ! a future extension
      itype = ODB_STRING
  else if (ftntypes(idx_col)(1:5) == 'REAL(') then
      itype = ODB_REAL
  else if (ftntypes(idx_col)(1:8) == 'INTEGER(' .and. &
  &           types(idx_col)(1:8) == 'Bitfield') then
      itype = ODB_BITFIELD
  else if (ftntypes(idx_col)(1:8) == 'INTEGER(') then
      itype = ODB_INTEGER
  else
      itype = ODB_INTEGER
  endif
  IF (LHOOK) CALL DR_HOOK('GET_COLTYPE',1,ZHOOK_HANDLE)
end function get_coltype
SUBROUTINE odb_bitfield_definition(khandle, coln, bnames, bsizes)
implicit none
INTEGER(KIND=JPIM), intent(in)        :: khandle
character(len=*), intent(in)          :: coln
character(len=*), intent(out)         :: bnames, bsizes
character(len=maxvarlen)              :: tablename
integer(kind=jpim)                    :: nextnames, rc, jj, jg
character(len=maxvarlen), allocatable :: extnames(:), exttypes(:)

integer(kind=jpim)                    :: idx_at, idx_at_ext
REAL(KIND=JPRB)                       :: ZHOOK_HANDLE

IF (LHOOK) CALL DR_HOOK('ODB_BITFIELD_DEFINITION',0,ZHOOK_HANDLE)

idx_at = index(coln,'@')
tablename= coln(idx_at:)
nextnames = ODB_getnames(khandle, trim(tablename),'extname')
allocate(extnames(nextnames))
allocate(exttypes(nextnames))

rc = ODB_getnames(khandle, trim(tablename),'extname', extnames(1:nextnames))
rc = ODB_getnames(khandle, trim(tablename),'exttype', exttypes(1:nextnames))

jj=1


do while (extnames(jj)(1:idx_at) /= coln(1:idx_at-1)//'.')
  jj=jj+1
enddo
bnames=""
bsizes=""

do jg=jj,nextnames
  if  (extnames(jg)(1:idx_at-1) /= coln(1:idx_at-1)) exit

  idx_at_ext = index(extnames(jg),'@')
  bnames=trim(bnames)//extnames(jg)(idx_at+1:idx_at_ext-1)//':'
  bsizes=trim(bsizes)//trim(exttypes(jg)(4:))//':'
enddo

deallocate(extnames)
deallocate(exttypes)

IF (LHOOK) CALL DR_HOOK('ODB_BITFIELD_DEFINITION',1,ZHOOK_HANDLE)

END SUBROUTINE odb_bitfield_definition

subroutine create_header_table(b2o_info, jc, tablename, list_cols)
 use, intrinsic :: iso_c_binding
 use odb_c_binding
!*AF use varindex_module

implicit none
 type(t_bufr2odb), intent(inout)             :: b2o_info
 integer(kind=JPIM), intent(inout)           :: jc
 character(len=maxvarlen), intent(in)        :: tablename
 character(len=64) , dimension(:),intent(in) :: list_cols

 integer(kind=C_INT)                    :: cerr
 integer(kind=JPIM)                     :: jj
 integer(kind=JPIM)                     :: rc
 integer(kind=JPIM)                     :: idx_col
 integer(kind=JPIM)                     :: ncolumns
 integer(kind=C_INT)                    :: itype
 character(len=maxvarlen)               :: bitfield_names, bitfield_sizes
 character(len=maxvarlen)               :: full_colname
 character(len=maxvarlen), allocatable  :: types(:), ftntypes(:)
 character(len=maxvarlen), allocatable  :: colnames(:)
 REAL(KIND=JPRB)                        :: ZHOOK_HANDLE


  IF (LHOOK) CALL DR_HOOK('CREATE_HEADER_TABLE',0,ZHOOK_HANDLE)
  ncolumns = ODB_getnames(b2o_info%handle,tablename,'name')
  allocate(colnames(ncolumns))
  rc = ODB_getnames(b2o_info%handle,tablename,'name',colnames)
  allocate(types(ncolumns))
  allocate(ftntypes(ncolumns))
  rc = ODB_getnames(b2o_info%handle, tablename,'type', types(1:ncolumns))
  rc = ODB_getnames(b2o_info%handle, tablename,'ftntype', ftntypes(1:ncolumns))
  do jj=1, size(list_cols)
    full_colname = trim(list_cols(jj))//trim(tablename)
    idx_col = get_coltype(b2o_info%handle, colnames, types, ftntypes, full_colname, itype)

!*AF    write(0,*) 'column = ', trim(list_cols(jj)), itype
    if (itype /= ODB_BITFIELD) then
      if (len(list_cols(jj)) > 10 .and. list_cols(jj)(1:10) == 'linkoffset') then
! change linkoffset by linken because linkoffset will be recomputed
           full_colname(1:10) = '   linklen'
      endif
      full_colname = trim(adjustl(full_colname))
      b2o_info%ODBcolname(jc+1) = full_colname
      full_colname = trim(adjustl(full_colname))//C_NULL_CHAR
      b2o_info%ODBcoltype(jc+1) = itype
      cerr = odb_write_set_column(b2o_info%c_odb_it, jc, itype, full_colname)
    else
      full_colname = trim(adjustl(full_colname))
      b2o_info%ODBcolname(jc+1) = full_colname
      b2o_info%ODBcoltype(jc+1) = itype
      full_colname = trim(adjustl(full_colname))//C_NULL_CHAR
      call odb_bitfield_definition(b2o_info%handle,full_colname, bitfield_names, bitfield_sizes)
      bitfield_names=trim(bitfield_names)//C_NULL_CHAR
      bitfield_sizes=trim(bitfield_sizes)//C_NULL_CHAR
      cerr = odb_write_set_bitfield(b2o_info%c_odb_it, jc, itype, full_colname, bitfield_names, bitfield_sizes)
    endif
    jc = jc + 1
  enddo
  deallocate(colnames)
  deallocate(types)
  deallocate(ftntypes)
  IF (LHOOK) CALL DR_HOOK('CREATE_HEADER_TABLE',1,ZHOOK_HANDLE)
end subroutine create_header_table
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
subroutine init_one_row(b2o_info, one_row)
implicit none

 type(t_bufr2odb), intent(inout)             :: b2o_info
 real(kind=C_DOUBLE), intent(inout)          :: one_row(:)

 integer(kind=JPIM)                          :: i
 character(len=8)                            :: csgn=""
 REAL(KIND=JPRB)                             :: str_real8
 real(kind=JPRB)                             :: mdi


 mdi = abs(ODB_getval(b2o_info%handle, '$mdi'))
 do i=1, b2o_info%c_ncolumns
   select case (b2o_info%ODBcoltype(i))
      case(ODB_BITFIELD)
        one_row(i) = 0
      case(ODB_INTEGER)
        if (b2o_info%ODBcolname(i)(1:4) == 'link' .or. &
            b2o_info%ODBcolname(i)(1:4) == 'LINK') then

            one_row(i) = 0
        else
          one_row(i) = mdi
        endif
      case(ODB_REAL)
        one_row(i) = -mdi
      case(ODB_STRING)
        one_row(i) = transfer(csgn,str_real8)
    end select
 enddo

end subroutine init_one_row
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
subroutine delete_header_table(b2o_info)
implicit none

 type(t_bufr2odb), intent(inout)             :: b2o_info

 if (associated(b2o_info%ODBcolname)) deallocate(b2o_info%ODBcolname)
 if (associated(b2o_info%ODBcoltype)) deallocate(b2o_info%ODBcoltype)
end subroutine delete_header_table
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

end module odbi
