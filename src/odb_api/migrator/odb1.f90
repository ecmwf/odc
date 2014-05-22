!-----------------------------------------------------------------------------------------
! 
!  Some routines to read/manipulate 'old-style' odb-1 data
!  Hans Hersbach, ECMWF, October 2011
! 
!-----------------------------------------------------------------------------------------

module odb1
!! Contains information of an odb-1 database
   implicit none

   character(len= 8), parameter :: cblank8  = '        '
   character(len=12),parameter  :: cloffset = 'LINKOFFSET', tloffset = "linkoffset_t"
   character(len=12),parameter  :: clinklen = 'LINKLEN'   , tlinklen = "linklen_t"

   type odb1_table                ! Contents of one table
     character(len=128)          :: tname                ! Table name
     integer                     :: iparent              ! index of parent table
     integer                     :: ncol                 ! Number of columns
     integer                     :: nrow                 ! Number of rows
     character(len=128), pointer :: cname(: ) => NULL()  ! column names
     character(len=128), pointer :: ctype(:)  => NULL()  ! column data type
     real*8            , pointer :: data(:,:) => NULL()  ! data
   end type odb1_table

   type todb1                     ! Contents of odb1 file for database dbname
     integer                     :: iundef
     real*8                      :: rundef
     integer                     :: handle
     integer                     :: npools
     character(len=64)           :: dbname
     integer                     :: ntable
     type (odb1_table) , pointer :: tbl(:)   => NULL()  ! table
   end type todb1

contains

!-----------------------------------------------------------------------------------------

subroutine odb1_delete_odb1(db1)
  implicit none

  type (todb1)              :: db1
  integer                    :: i   

  do i=1,db1%ntable
     call odb1_delete_table(db1%tbl(i))
  enddo

  db1%handle =-1
  db1%dbname =""
  db1%ntable =0
  if(associated(db1%tbl))deallocate(db1%tbl)

end subroutine odb1_delete_odb1

!-----------------------------------------------------------------------------------------

subroutine odb1_delete_table(tbl)
  implicit none

  type (odb1_table) :: tbl

  tbl%iparent=0
  tbl%ncol   =0
  tbl%nrow   =0
  tbl%tname  =""
  if (associated(tbl%cname)) deallocate(tbl%cname)
  if (associated(tbl%ctype)) deallocate(tbl%ctype)
  if (associated(tbl%data )) deallocate(tbl%data )

end subroutine odb1_delete_table

!-----------------------------------------------------------------------------------------

subroutine odb1_create_table(tbl,tname,iparent,ncol,cname,ctype)
  implicit none

  type (odb1_table)  :: tbl
  integer            :: ncol, iparent
  character(len=128) :: tname,cname(ncol),ctype(ncol)

  integer :: i

  call odb1_delete_table(tbl)
  write(tbl%tname,'(A)')trim(tname)

  tbl%iparent=iparent
  tbl%ncol   =ncol
  allocate(tbl%cname(ncol))
  allocate(tbl%ctype(ncol))

  do i=1,ncol
     write(tbl%cname(i),'(A)')trim(cname(i))
     write(tbl%ctype(i),'(A)')trim(ctype(i))
  enddo

end subroutine odb1_create_table

!-----------------------------------------------------------------------------------------

subroutine odb1_alloc_data(tbl,nrow)
  implicit none

  type (odb1_table) :: tbl
  integer           :: nrow,j

  real*8 :: rblank8
  rblank8 = transfer(cblank8,rblank8)

  if (tbl%ncol<=0) call abort() 
      tbl%nrow=nrow
  if (associated(tbl%data)) deallocate(tbl%data)
     allocate(tbl%data(nrow,0:tbl%ncol))

  tbl%data(:,:)=0
  do j=1,tbl%ncol
     if(trim(tbl%ctype(j))=='string') tbl%data(:,j)=rblank8
  enddo

end subroutine odb1_alloc_data

!-----------------------------------------------------------------------------------------

subroutine odb1_add_block(tbl,data,irow,icol,nrow,ncol)
  implicit none

  type (odb1_table) :: tbl
  real*8            :: data(nrow,ncol)
  integer           :: irow,icol,ncol,nrow

  integer           :: irowe,icole

  irowe=irow+nrow-1 ; if (irowe>tbl%nrow) call abort()
  icole=icol+ncol-1 ; if (icole>tbl%ncol) call abort()

  tbl%data(irow:irowe,icol:icole)=data(1:nrow,1:ncol)

end subroutine odb1_add_block

!-----------------------------------------------------------------------------------------

subroutine odb1_header(db1)

  implicit none

  type (todb1)     :: db1
  
  type (odb1_table), pointer :: tbl
  integer                    :: i,j
  character(len=128)         :: crange
  character*8                :: cdum

  write(*,'(3A,I4)')"ODB-1 odb1: ",trim(db1%dbname),", handle: ",db1%handle
  do i=1,db1%ntable
     tbl => db1%tbl(i)
     do j=1,tbl%ncol
        crange=""
        if (trim(tbl%ctype(j))=='string') then
           write(crange,'(", First Value=<",A,">")')transfer(tbl%data(1,j),cdum)
!          write(crange,'(", First Value=<",A,">")')"Under construction"
        else
           write(crange,'(", Range=<",E15.8,",",E15.8,">")')&
           minval(tbl%data(:,j)),maxval(tbl%data(:,j))
        endif
        write(*,'(2I4,I8,2X,A,":",2A)') &
        & i,j,tbl%nrow,trim(tbl%ctype(j)),trim(tbl%cname(j)),trim(crange)
     enddo
  enddo

end subroutine odb1_header

!-----------------------------------------------------------------------------------------

function odb1_find_index(db1,table)

  implicit none

  integer :: odb1_find_index
  type (todb1)        :: db1
  character(len=128) :: table

  type (odb1_table), pointer :: tbl
  integer                    :: i

  do i=0,db1%ntable
     tbl => db1%tbl(i)
     if (trim(tbl%tname)==trim(table)) exit
  enddo
  odb1_find_index=i
! In case not found return ntable+1


end function odb1_find_index

!-----------------------------------------------------------------------------------------

subroutine odb1_reassign_seqno(db1,ioffset)
! reassign seqno@hdr from ioffset+1 to ioffset+nrow

  implicit none

  type (todb1)       :: db1
  integer            :: ioffset

  type (odb1_table), pointer :: tbl
  character(len=128)         :: column,table
  integer                    :: i,j,k

  column="seqno@hdr"
  table="@hdr"

  k=odb1_find_index(db1,table)
  tbl => db1%tbl(k)
  j=odb1_column_index(tbl,column)

  do i=1,tbl%nrow
     ioffset=ioffset+1
     tbl%data(i,j)=ioffset
  enddo
write(*,*)"odb1_reassign_seqno: k,j,tbl%nrow: ", k,j,tbl%nrow

end subroutine odb1_reassign_seqno

!-----------------------------------------------------------------------------------------

subroutine odb1_set_value(db1,cvalue)

  implicit none

  type (todb1)       :: db1
  character(len=128) :: cvalue

  type (odb1_table), pointer :: tbl
  character(len=128)         :: column,table,child
  real*8                     :: value
  integer                    :: i,j,k
  logical                    :: llink

  i=index(cvalue,"=") ; if(i<=1)return
    column="" ; write(column,'(A)')trim(cvalue(1:i-1))

  j=index(column,"@") ; if(j==1)return
    table=""  ; write(table,'(A)')trim(column(j:))
  
  llink=.false.
  k=index(column,".")
  if (k>0) then
     llink=.true.
     child="" ; write(child,'(A)')trim(column(1:k-1))
     if (column(k:j-1)==".len") then
       write(column,'(A,"(",A,")",A)')trim(clinklen),trim(child),trim(table)
     else
       write(*,*)"Only .len column can be set: ",trim(column)
       return
     endif
  endif

  k=odb1_find_index(db1,table)
  if (k>db1%ntable) then
     write(*,*)"table not found: ",trim(table)
     return
  endif

  tbl => db1%tbl(k)
  j=odb1_column_index(tbl,column)
  if (j==0) then
     write(*,*)"Column not found: ",trim(column)
     return
  endif
  
! Set value
  read(cvalue(i+1:),*)value
  write(*,*)'Set column: ',trim(column),value,tbl%nrow
  tbl%data(1:tbl%nrow,j)=value

! If required, update offset
  if (.not.llink) return
  j=j-1
  tbl%data(1,j)=0
  do k=2,tbl%nrow
     tbl%data(k,j)=tbl%data(k-1,j)+value
  enddo


end subroutine odb1_set_value

!-----------------------------------------------------------------------------------------

function odb1_find_parent(db1,table,parent)

  implicit none

  integer            :: odb1_find_parent
  type (todb1)        :: db1
  character(len=128) :: table,parent
  
  type (odb1_table), pointer :: tbl
  integer                    :: i,j,i1,i2

  odb1_find_parent=0  ! no parent found, so far
  parent          ="@"

  do i=1,db1%ntable
     tbl => db1%tbl(i)
     do j=1,tbl%ncol
        if (index(tbl%cname(j),trim(cloffset)) == 0) cycle
        i1=index(tbl%cname(j),"(")+1
        i2=index(tbl%cname(j),")")-1
        if (trim(table) == "@"//tbl%cname(j)(i1:i2) ) then
           write(parent,'(A)')trim(tbl%tname)
           odb1_find_parent=i
           exit
        endif
     enddo
  enddo

end function odb1_find_parent

!-----------------------------------------------------------------------------------------

function odb1_child_linkindex(tbl,child)

  implicit none
  integer odb1_child_linkindex
  type (odb1_table)  :: tbl
  character(len=128) :: child

  character(len=128) :: cname

  cname=""
  write(cname,'(A,"(",A,")",A)')trim(cloffset),trim(child(2:)),trim(tbl%tname)
  odb1_child_linkindex=odb1_column_index(tbl,cname)

end function odb1_child_linkindex

!-----------------------------------------------------------------------------------------

subroutine odb1_find_type(db1,cname,ctype)
! find ctype for cname as defined in odb1 file db1

  implicit none
  type (todb1)      :: db1
  character(len=128) :: cname,ctype

  type (odb1_table), pointer :: tbl
  integer                    :: i,j

  ctype=""
  do i=1,db1%ntable
     tbl => db1%tbl(i)
     do j=1,tbl%ncol
        if ( trim(cname) == trim(tbl%cname(j)) ) then
          write(ctype,'(A)')trim(tbl%ctype(j))
          return
        endif
     enddo
  enddo

end subroutine odb1_find_type

!-----------------------------------------------------------------------------------------

function odb1_column_index(tbl,cname)

  implicit none
  integer            :: odb1_column_index
  type (odb1_table)  :: tbl
  character(len=128) :: cname

  integer :: i

  odb1_column_index=0
  do i=1,tbl%ncol
     if ( trim(cname) == trim(tbl%cname(i)) ) then
          odb1_column_index=i 
          return
     endif
  enddo

end function odb1_column_index

!-----------------------------------------------------------------------------------------

subroutine odb1_open(db1,dbname,npools,myproc,nproc)

  use odb_module
  implicit none

  type (todb1)      :: db1
  character(len=64) :: dbname
  integer           :: npools,myproc,nproc

  type (odb1_table), pointer :: tbl

  integer :: iret, ihandle, io_method, ntables, ncols,nrows,i 
  character(len=20)  :: copen
  character(len=64)  :: chviewname
  real*8,allocatable :: data(:,:)

  character(len=128), allocatable :: ctable(:)

! Clean odb1 file
  call odb1_delete_odb1(db1)
  
! Open ODB-1
  write(*,*) '            '
  write(*,*) 'SET-UP ODB-1'
  iret  = ODB_init(myproc=myproc,nproc=nproc)
  write(*,'(A,2I6)')'odb1_open, myproc,nproc:',myproc,nproc

  copen = 'NEW' ; chviewname='*'
  ihandle = ODB_open   (dbname, copen, npools=npools)

  io_method = ODB_io_method(ihandle)

  db1%iundef=abs(odb_getval(ihandle,'$mdi'))
  db1%rundef=-db1%iundef

  ntables = ODB_getnames(ihandle,chviewname,'table'); allocate(ctable(ntables))
     db1%handle=ihandle
     db1%npools=npools
     db1%dbname=trim(dbname)
     db1%ntable=ntables
     allocate(db1%tbl(0:ntables))

  ntables = ODB_getnames(ihandle,chviewname,'table', ctable)
  do i=0, ntables
    tbl => db1%tbl(i)
    tbl%tname="@" ; tbl%ncol=0 ; tbl%nrow=0
    if(i==0) cycle
    tbl%tname=trim(ctable(i))
    ncols = ODB_getnames(ihandle,ctable(i),'name')
      tbl%ncol=ncols
      allocate(tbl%cname(ncols),tbl%ctype(ncols)) 
    ncols=ODB_getnames(ihandle,ctable(i),'name', tbl%cname)
    ncols=ODB_getnames(ihandle,ctable(i),'type', tbl%ctype)
  enddo

end subroutine odb1_open

!-----------------------------------------------------------------------------------------

subroutine odb1_merge(db1,db2)
! Merge contents of db1 into db2

  implicit none

  type (todb1)     :: db1,db2

  type (odb1_table), pointer :: tbl1,tbl2

  integer                    :: i1,i2,j1,j2,nrow,ncol,iret

! Link tables

! Loop over tables
  do i1=1,db1%ntable
     tbl1 => db1%tbl(i1)

   ! Match tables
     do i2=1,db2%ntable
        tbl2 => db2%tbl(i2)
        if(trim(tbl1%tname)==trim(tbl2%tname))exit
     enddo
     if(i2>db2%ntable) call abort()

   ! write(*,*)i1,i2,trim(tbl1%tname)

   ! Allocate data section for tbl2
     nrow=tbl1%nrow
     ncol=tbl1%ncol
     call odb1_alloc_data(tbl2,nrow)

   ! Match columns
     do j1=1,tbl1%ncol
        do j2=1,tbl2%ncol
           if( trim(tbl1%cname(j1)) == trim(tbl2%cname(j2)) ) exit
        enddo
        if(j1>ncol) call abort()
      ! write(*,*)j1,j2,trim(tbl1%cname(j1))

      ! Copy data contents
        call odb1_add_block(tbl2,tbl1%data(1:nrow,j1:j1),1,j2,nrow,1)
     enddo

  enddo

end subroutine odb1_merge

!-----------------------------------------------------------------------------------------

subroutine odb1_write(db1,ipool)

  use odb_module
  implicit none

  type (todb1)     :: db1
  integer          :: ipool

  type (odb1_table), pointer :: tbl
  integer                    :: i,j,i1,i2,iret,ihandle,nrows,ncols,ntable,ichild,ioffset
  character(len=128)         :: child

  integer,allocatable        :: noffset(:)

  ntable =db1%ntable
  ihandle=db1%handle

  ! Figure out how much has already been stored. Is needed to calculate offset for links
  allocate(noffset(ntable))
  do i=1,ntable
     tbl => db1%tbl(i)
     iret = ODB_getsize(ihandle, tbl%tname, nrows, ncols,poolno=ipool)
     noffset(i)=nrows
  enddo

  write(*,'(2A,I6)')"Write ODB-1 Database/poolno: ",trim(db1%dbname),ipool
  do i=1,ntable
     tbl => db1%tbl(i)
   ! Only do non-empty ones 
     if(tbl%nrow==0) cycle

     ! Find any links to children, and add offset
     do j=1,tbl%ncol
        if (index(tbl%cname(j),trim(cloffset)) == 0) cycle
        i1=index(tbl%cname(j),"(")+1
        i2=index(tbl%cname(j),")")-1
        child=""; write(child,'(2A)')"@",trim(tbl%cname(j)(i1:i2))
        ioffset=odb1_find_index(db1,child)
        tbl%data(:,j)=tbl%data(:,j)+noffset(ioffset)
     enddo

     iret = ODB_put    (ihandle, tbl%tname, tbl%data, tbl%nrow, poolno=ipool)
     iret = ODB_getsize(ihandle, tbl%tname, nrows, ncols,poolno=ipool)        ! diagnostics
     write(*,'(A,I6,A,I6,I4,2A)')'Pool: ',ipool,' PUT: ',nrows, ncols,' Rows/Cols in: ',trim(tbl%tname)
  enddo

  iret = ODB_release(db1%handle,poolno=ipool)

  deallocate(noffset)

end subroutine odb1_write

!-----------------------------------------------------------------------------------------

subroutine odb1_close(db1)
  use odb_module
  implicit none

  type (todb1)     :: db1
  integer          :: iret, ihandle

  ihandle=db1%handle
  iret = ODB_close(ihandle, save=.TRUE.)
  iret = ODB_end()
end subroutine odb1_close

!-----------------------------------------------------------------------------------------

end module odb1
