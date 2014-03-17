!-----------------------------------------------------------------------------------------
! Hans Hersbach ECMWF, November 2011
! 38r1 flag definitions

module odb2
!! Contains information in one odb-2 database
  use, intrinsic :: iso_c_binding
  use odb_c_binding
  use odb2_flag_definitions
  implicit none

  integer, parameter            :: mlen    = 1024
  real(kind=C_DOUBLE),parameter :: iundef  = 2147483647.0_C_DOUBLE
  real(kind=C_DOUBLE),parameter :: rundef  = -iundef
  real(kind=C_DOUBLE),parameter :: cundef  = transfer("????????",rundef)

  type todb2
     type(C_PTR)                     :: h,it
     character(kind=C_CHAR,len=mlen) :: file
     integer                         :: ncol,nrow
     character                       :: status              ! 'r' or 'w'
     character(len=mlen), pointer    :: cname(:) => NULL()  ! column name@table
     integer            , pointer    :: itype(:) => NULL()  ! column type
     real*8             , pointer    :: data(:,:)=> NULL()  ! odb2 contents
  end type todb2

  type tdef
     integer                      :: ndef=0
     character(len=1000),pointer  :: bdef(:) => NULL()
     character(len=1000),pointer  :: bits(:) => NULL()
  end type tdef
contains

!-----------------------------------------------------------------------------------------

function cstr(fstring)
! Convert Fortran string to C string by appending the "\0" character
  implicit none
  character(kind=C_CHAR, len=mlen) :: cstr
  character(len=*)                 :: fstring

  cstr=trim(fstring)//achar(0)
end function cstr

!-----------------------------------------------------------------------------------------

function find_column(o2,cname)
! Find matching column@table index"
  implicit none
  integer          :: find_column
  type (todb2)     :: o2
  character(len=*) :: cname

  integer :: i,j,k,l

  j=index(cname,":") +1
  k=len_trim(cname)

  do i=1,o2%ncol
     l=len_trim(o2%cname(i))
     if (cname(j:k)==o2%cname(i)(:l)) exit
  enddo
  if(i>o2%ncol) then
     i=0
     write(*,*)"Warning in find_column: cannot find column: ",trim(cname)
  endif
  find_column=i

end function find_column

!-----------------------------------------------------------------------------------------

function xval(x8,xundef)
  implicit none
  real   :: xval,xundef
  real*8 :: x8

  xval=x8
  if (x8==rundef) xval=xundef
end function xval

!-----------------------------------------------------------------------------------------

function s2i(c)
! Place 4-character string into integer*4
  implicit none
  integer*4   :: s2i
  character*4 :: c
  s2i=transfer(c,s2i)
end function s2i

!-----------------------------------------------------------------------------------------
function s2x(c)
! Place 8-character string into real*8
  implicit none
  real*8      :: s2x
  character*8 :: c
  s2x=transfer(c,s2x)
end function s2x

!-----------------------------------------------------------------------------------------

function x2s(x)
! Place real*8 into 8-character string
  implicit none
  character*8 :: x2s
  real*8      :: x
  x2s=transfer(x,x2s)
end function x2s

!-----------------------------------------------------------------------------------------

function s2a(s)
! Place 4-character string into concatenated ascii
! Right-adjust it first, so " AB " => "  AB"
! use 0 for blank ' ', rather than its ascii code 32
! all numbers are negative

! The fortran transfer function would also work, but
! would give less intuitive results

  implicit none
  integer*4   :: s2a
  character*4 :: s
  character   :: c

  integer :: i,ib

  s2a=0 ; ib=1
  do i=len_trim(s),1,-1
      c=s(i:i)
      if(c/=' ')s2a=s2a-ib*iachar(c)
     ib=ib*256
  enddo
end function s2a

function a2s(a)
! Place concatenated ascii back into string*4
  implicit none
  integer     :: a
  character*4 :: a2s

  integer*4 :: i,ib,ic,ia

  a2s="    " ; ib=256**3
  if (a>0) then
     write(*,*)"Warning a2s: unsuitable input ",a
     return
  endif

  ia=-a
  do i=1,4
     ic=ia/ib
     ia=ia-ic*ib
     if (ic<0 .or. ic>=256) write(*,*)"Warning a2s: unsuitable input ",a
     if (ic/=0) a2s(i:i)= achar(mod(ic,256))
     ib=ib/256
  enddo
end function a2s

!-----------------------------------------------------------------------------------------

subroutine odb2_delete(o2)
  implicit none
  type (todb2)   :: o2

  o2%ncol=0
  if(associated(o2%cname))deallocate(o2%cname)
  if(associated(o2%itype))deallocate(o2%itype)
  if(associated(o2%data ))deallocate(o2%data)

end subroutine odb2_delete

!-----------------------------------------------------------------------------------------

subroutine odb2_open(o2,odb2file,status)

  implicit none
  character(len=*)               :: odb2file
  type (todb2), intent(inout)    :: o2
  character, intent(in),optional :: status

  type(C_PTR)                     :: odb_it,odb_handle
  integer(kind=C_INT)             :: itype,cerr
  character(kind=C_CHAR,len=mlen) :: config=C_NULL_CHAR
  character                       :: stat='r'

! Start from fresh odb
  call odb2_delete(o2)

! Read, write, old?
  if (present(status)) stat=status
  if (stat=='R'      ) stat='r'
  if (stat=='W'      ) stat='w'
  o2%status=stat

! Open file and set ODB-2 pointers

! write(*,'(4A)')'Open ODB-2 file (',stat,'): ',trim(odb2file)
  o2%file=cstr(odb2file)

  if (o2%status=='w') then
     odb_handle = odb_write_new(config, cerr)
     odb_it     = odb_write_iterator_new(odb_handle, o2%file, cerr)
  endif

  if (o2%status=='r') then
     odb_handle = odb_read_new (config, cerr)
     odb_it     = odb_read_iterator_new (odb_handle, o2%file, cerr)
  endif

  if (o2%status=='a') then
     odb_handle = odb_write_new(config, cerr)
     odb_it     = odb_append_iterator_new(odb_handle, o2%file, cerr)
  endif

  o2%h = odb_handle
  o2%it= odb_it

end subroutine odb2_open

!-----------------------------------------------------------------------------------------

subroutine odb2_rewind(o2)

  implicit none
  type (todb2)                   :: o2

  integer(kind=C_INT)            :: cerr

  if(o2%status/='r') call abort()
  cerr  = odb_read_iterator_delete(o2%it)
  o2%it = odb_read_iterator_new(o2%h, o2%file, cerr)

end subroutine odb2_rewind

!-----------------------------------------------------------------------------------------

subroutine odb2_get(o2,ncol,cname,status)

  implicit none
  type (todb2)                    :: o2
  integer,         intent(inout)          :: ncol
  character(len=*),intent(  out),optional :: cname(ncol)
  character,       intent(  out),optional :: status

  integer(kind=C_INT)                                 :: cerr,c_ncolumns,c_nchar
  type(C_PTR)                                         :: ptr_colname
  character(kind=C_CHAR,len=1), dimension(:), pointer :: f_ptr_colname
  character(len=mlen)                                 :: ctype
  integer(kind=4)                                     :: i,j


  if (o2%ncol==0) then
     cerr = odb_read_get_no_of_columns(o2%it, c_ncolumns)
     o2%ncol=c_ncolumns
     if (.not.associated(o2%cname)) allocate(o2%cname(o2%ncol))
     if (.not.associated(o2%itype)) allocate(o2%itype(o2%ncol))
  endif

  ! read column names into o2 structure
  do j=1,o2%ncol
     cerr = odb_read_get_column_type(o2%it, j-1, o2%itype(j))
     cerr = odb_read_get_column_name(o2%it, j-1, ptr_colname,c_nchar)
     call C_F_POINTER(CPTR=ptr_colname, FPTR=f_ptr_colname, shape=(/c_nchar/));
     o2%cname(j)=""
     do i=1, c_nchar
        o2%cname(j)(i:i)  = f_ptr_colname(i)
     end do
  enddo

  if (present(cname)) then
     do j=1,min(o2%ncol,ncol)
        select case (o2%itype(j))
          case (ODB_INTEGER ) ; ctype="pk1int"
          case (ODB_REAL    ) ; ctype="pk9real"
          case (ODB_DOUBLE  ) ; ctype="pk9real"
          case (ODB_STRING  ) ; ctype="string"
          case (ODB_BITFIELD) ; call odb_find_ctype(o2%cname(j),ctype)
        end select
        cname(j)=""
        write(cname(j),'(3A)')trim(ctype),":",trim(o2%cname(j))
     enddo
  endif

  ncol=o2%ncol
  if (present(status))status=o2%status

end subroutine odb2_get

!-----------------------------------------------------------------------------------------

subroutine odb2_set_columns(o2, ncol, cname)
  implicit none

  type (todb2),     intent(inout) :: o2
  integer,          intent(inout) :: ncol
  character(len=*), intent(inout) :: cname(ncol)

  !type(C_PTR)                     :: odb_it
  integer(kind=C_INT)             :: itype, cerr, c_ncol
  character(kind=C_CHAR,len=mlen) :: c_cname, cbdef, cbits
  character(len=mlen)             :: ctype, bdef, bits
  integer                         :: i, ic

  c_ncol=ncol
  !odb_it=o2%it
  cerr = odb_write_set_no_of_columns(o2%it, c_ncol)

  do ic=1,ncol
     i=index(cname(ic),":")
     c_cname=cstr(cname(ic)(i+1:))
     ctype  =     cname(ic)(1:i-1)

     if (trim(ctype)=="pk1int" ) then
        itype=ODB_INTEGER
        cerr=odb_write_set_column(o2%it,ic-1,itype,c_cname)
        cerr=odb_write_set_missing_value(o2%it,ic-1,iundef)
        cycle
     endif

     if (trim(ctype)=="pk9real" ) then
        itype=ODB_REAL
        cerr=odb_write_set_column(o2%it, ic-1, itype, c_cname)
        cerr=odb_write_set_missing_value(o2%it, ic-1, rundef)
        cycle
     endif

     if (trim(ctype)=="string" ) then
        itype=ODB_STRING
        cerr=odb_write_set_column(o2%it, ic-1, itype, c_cname)
        cerr=odb_write_set_missing_value(o2%it, ic-1, cundef)
        cycle
     endif
 
     if (trim(ctype)=="datum_event2_t" .or. trim(ctype)=="report_event2_t" ) then
        itype=ODB_INTEGER
        cerr=odb_write_set_column(o2%it, ic-1, itype, c_cname)
        cerr=odb_write_set_missing_value(o2%it, ic-1, iundef)
        cycle
     endif

   !- Else assume bit field
     itype=ODB_BITFIELD
     call get_odb_flags(ctype,bdef,bits)
        if(trim(bdef)=="") call abort()
        cbdef=cstr(trim(bdef))
        cbits=cstr(trim(bits))
        cerr=odb_write_set_bitfield(o2%it, ic-1, itype, c_cname, cbdef, cbits)
        cerr=odb_write_set_missing_value(o2%it, ic-1, iundef)
        cycle

  enddo
  cerr = odb_write_header(o2%it)

! Let's find out what we really defined
  !call odb2_get(o2,ncol,cname=cname)

end subroutine odb2_set_columns

!-----------------------------------------------------------------------------------------

subroutine odb2_read(o2,xrow,ncol,nrow)

  implicit none
  type (todb2)           :: o2
  integer, intent(in   ) :: ncol
  integer, intent(inout) :: nrow
  real*8                 :: xrow(ncol,nrow)

  integer(kind=C_INT) :: c_ncol,cerr
  integer(kind=C_INT) :: c_ncolumns,idum
  character(len=8)    :: tmp_str,tmp_str2
  real*8              :: tmp_dbl
  integer             :: i,j,k
  real(kind=C_DOUBLE), dimension(:), allocatable:: one_row

  c_ncolumns=o2%ncol
  allocate(one_row(c_ncolumns))
  do i=1,nrow
      cerr = odb_read_get_next_row(o2%it, c_ncolumns, one_row, idum)
      if (cerr /= 0) exit
      xrow(1:ncol,i)=one_row(1:ncol)
  enddo
  deallocate(one_row)
  nrow=i-1

! Remove '\0' characters for strings
  do j=1,ncol
     if(o2%itype(j)/=ODB_STRING)cycle
     do i=1,nrow
        tmp_str=transfer(xrow(j,i),tmp_str)
        tmp_str2=""
        do k=1,len(tmp_str) ! not pretty, but doesn't seem slow
          if (tmp_str(k:k) .eq. char(0)) exit
          tmp_str2(k:k)=tmp_str(k:k)
        enddo
        xrow(j,i)=transfer(tmp_str2,tmp_dbl)
     enddo
  enddo

end subroutine odb2_read

!-----------------------------------------------------------------------------------------

subroutine odb2_read_all(o2,odbfile)
! place the entire odb2 data contents in o2%data: to be phased out
  implicit none
  type (todb2)     :: o2
  character(len=*) :: odbfile

  integer                         :: ncol,nread
  character(len=mlen),allocatable :: cname(:)

  call odb2_open(o2,odbfile,status='r')
  call odb2_get(o2,ncol)
  allocate(cname(ncol))
  call odb2_get(o2,ncol,cname=cname)

  o2%nrow=odb_count(o2%file)
  allocate(o2%data(ncol,o2%nrow))
  nread=o2%nrow

  call odb2_read(o2,o2%data,ncol,nread)
  if (nread/=o2%nrow) then
     write(*,*)"odb2_read_all: Problem with reading odb file: ",trim(odbfile),nread,o2%nrow
     call abort()
  endif

  deallocate(cname)

end subroutine odb2_read_all

!-----------------------------------------------------------------------------------------

subroutine odb2_write(o2,xrow,ncol,nrow)

  implicit none
  type (todb2) :: o2
  integer      :: ncol,nrow
  real*8       :: xrow(ncol,nrow)

  integer(kind=C_INT) :: c_ncol,cerr
  real(kind=C_DOUBLE) :: c_xrow(ncol)
  integer             :: i

  c_ncol=ncol
  do i=1,nrow
     c_xrow=xrow(:,i)
     cerr = odb_write_set_next_row(o2%it, c_xrow, c_ncol)
  enddo

end subroutine odb2_write

!-----------------------------------------------------------------------------------------

subroutine odb2_set_undefined(o2,xrow,ncol,nrow)

  implicit none
  type (todb2) :: o2
  integer      :: ncol,nrow
  real*8       :: xrow(ncol,nrow)
  integer :: icol

  do icol=1,min(ncol,o2%ncol)
     select case (o2%itype(icol))
        case (ODB_INTEGER ) ; xrow(icol,1:nrow)=iundef
        case (ODB_REAL    ) ; xrow(icol,1:nrow)=rundef
        case (ODB_STRING  ) ; xrow(icol,1:nrow)=cundef
        case (ODB_BITFIELD) ; xrow(icol,1:nrow)=iundef
        case default        ; call abort()
     end select
  enddo
end subroutine odb2_set_undefined

!-----------------------------------------------------------------------------------------

subroutine odb2_close(o2)

  implicit none
  type (todb2)        :: o2

  integer(kind=C_INT) :: cerr

  if (o2%status == 'w') then
     cerr = odb_write_iterator_delete(o2%it)
     cerr = odb_write_delete(o2%h)
  endif

  if (o2%status == 'r') then
     cerr = odb_read_iterator_delete(o2%it)
     cerr = odb_read_delete(o2%h)
  endif

  if (o2%status == 'a') then
     cerr = odb_write_iterator_delete(o2%it)
     cerr = odb_write_delete(o2%h)
  endif

end subroutine odb2_close

!-----------------------------------------------------------------------------------------

end module odb2

