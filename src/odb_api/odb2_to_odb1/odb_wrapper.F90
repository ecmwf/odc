!> @file   odb_wrapper.F90
!> @author Anne Fouilloux

!----------------------------------------------------------------------
function odb_no_of_local_pools_c(handle) bind(C, name="odb_no_of_local_pools_f90")
  use, intrinsic :: iso_c_binding
  use            :: odb_module

  implicit none

  integer(C_INT),VALUE                :: handle
  integer(C_INT)                      :: odb_no_of_local_pools_c

  integer(kind=JPIM)                  :: f90_handle
  integer(kind=JPIM)                  :: f90_nmypools

  f90_handle = handle

  f90_nmypools = ODB_poolinfo(f90_handle)

  odb_no_of_local_pools_c = f90_nmypools

end function odb_no_of_local_pools_c
!----------------------------------------------------------------------
function odb_get_local_pools_c(handle, local_pools) bind(C, name="odb_get_local_pools_f90")
  use, intrinsic :: iso_c_binding
  use            :: odb_module

  implicit none

  type, bind(c) :: c_int_array
    integer(C_INT)                     :: len
    type(C_PTR)                        :: array
  end type c_int_array

  integer(C_INT),VALUE                :: handle
  integer(C_INT)                      :: odb_get_local_pools_c
  type(c_int_array), intent(out)      :: local_pools
  integer(C_INT), pointer             :: c_local_pools(:)

  integer(kind=JPIM)                  :: f90_handle
  integer(kind=JPIM)                  :: f90_nmypools

  f90_handle = handle

  call C_F_POINTER(local_pools%array, c_local_pools, (/local_pools%len/))

  f90_nmypools = ODB_poolinfo(f90_handle, c_local_pools)

  odb_get_local_pools_c = f90_nmypools

end function odb_get_local_pools_c
!----------------------------------------------------------------------
function odb_open_c(dbname, dbname_length, mode, mode_length, npools) bind(C, name="odb_open_f90")
  use, intrinsic :: iso_c_binding
  use            :: odb_module
  use mpl_module

  implicit none

  character(kind=C_CHAR), dimension(*) :: dbname
  character(kind=C_CHAR), dimension(*) :: mode
  integer(C_INT)                       :: npools
  integer(C_INT),VALUE                 :: dbname_length
  integer(C_INT),VALUE                 :: mode_length
  integer(C_INT)                       :: odb_open_c

  character(len=64)                    :: f90_dbname
  character(len=64)                    :: f90_mode
  integer(kind=JPIM)                   :: f90_npools
  integer(kind=JPIM)                   :: f90_handle
  integer(kind=JPIM)                   :: f90_rc
  integer(kind=JPIM)                   :: i


  f90_npools = npools
  f90_dbname=""
  do i=1, dbname_length
    if (dbname(i) .eq. C_NULL_CHAR) exit
    f90_dbname(i:i)  = dbname(i)
  end do
  f90_mode=""
  do i=1, mode_length
    if (mode(i) .eq. C_NULL_CHAR) exit
    f90_mode(i:i)  = mode(i)
  end do

  f90_handle = ODB_open(f90_dbname, f90_mode, f90_npools)
  odb_open_c = f90_handle
! write flag files
  call cODB_print_flags_file(f90_dbname, MPL_MYRANK(), f90_rc)
end function odb_open_c
!----------------------------------------------------------------------
function odb_close_c(handle, save) bind(C, name="odb_close_f90")
  use, intrinsic                      :: iso_c_binding
  use odb_module

  implicit none

  integer(C_INT),VALUE                :: handle
  logical(C_BOOL), VALUE              :: save
  integer(C_INT)                      :: odb_close_c

  logical                             :: f90_save
  integer(kind=JPIM)                  :: f90_handle
  integer(kind=JPIM)                  :: f90_err

  f90_save = save
  f90_handle = handle
  write(0,*) 'ODB_close f90_save = ', f90_save, ' f90_handle = ', f90_handle
  f90_err = ODB_close(f90_handle, f90_save)
  odb_close_c = f90_err
end function odb_close_c
!----------------------------------------------------------------------
function odb_swapout_c(handle, dtname, dtname_length, poolno, save, repack) bind(C, name="odb_swapout_f90")
  use, intrinsic                      :: iso_c_binding
  use odb_module

  implicit none

  integer(C_INT),VALUE                 :: handle
  character(kind=C_CHAR), dimension(*) :: dtname
  integer(C_INT),VALUE                 :: dtname_length
  integer(C_INT),VALUE                 :: poolno
  logical(C_BOOL), VALUE               :: save
  logical(C_BOOL), VALUE               :: repack
  integer(C_INT)                       :: odb_swapout_c

  integer(kind=JPIM)                   :: f90_poolno
  integer(kind=JPIM)                   :: f90_handle
  character(len=64)                    :: f90_dtname
  logical                              :: f90_save, f90_repack
  integer(kind=JPIM)                   :: i
  integer(kind=JPIM)                   :: rc

  f90_dtname=""
  do i=1, dtname_length
    if (dtname(i) .eq. C_NULL_CHAR) exit
    f90_dtname(i:i)  = dtname(i)
  end do

  f90_handle = handle
  f90_poolno = poolno
  f90_save = save
  f90_repack = repack

  rc = ODB_swapout(f90_handle, f90_dtname, poolno=f90_poolno, save=f90_save, repack=f90_repack)

  odb_swapout_c = rc
end function odb_swapout_c
!----------------------------------------------------------------------
function odb_get_no_of_columns_c(handle, dtname, dtname_length) bind(C, name="odb_get_no_of_columns_f90")
  use, intrinsic                      :: iso_c_binding
  use odb_module

  implicit none

  integer(C_INT), VALUE                :: handle
  character(kind=C_CHAR), dimension(*) :: dtname
  integer(C_INT),VALUE                 :: dtname_length
  integer(C_INT)                       :: odb_get_no_of_columns_c

  integer(kind=JPIM)                   :: f90_ncols
  integer(kind=JPIM)                   :: f90_handle
  character(len=64)                    :: f90_dtname
  integer(kind=JPIM)                   :: i, at_offset

  if (dtname(1) /= '@') then
    at_offset = 1
    f90_dtname='@'
  else
    at_offset=0
    f90_dtname=""
  endif
  do i=1, dtname_length
    if (dtname(i) .eq. C_NULL_CHAR) exit
    f90_dtname(i+at_offset:i+at_offset)  = dtname(i)
  end do

  f90_handle = handle
  f90_ncols = ODB_getnames(f90_handle,f90_dtname,'name')
  odb_get_no_of_columns_c = f90_ncols
end function odb_get_no_of_columns_c
!----------------------------------------------------------------------
subroutine odb_api_get_colnames(odb_iterator, colnames, c_ncols, idx_seqno, masterKey)
  use, intrinsic                      :: iso_c_binding
  use odb_module
  use odb_c_binding

  implicit none

  TYPE(C_PTR)                               :: odb_iterator
  character(len=maxvarlen), dimension(*)    :: colnames
  integer(kind=C_INT)                       :: c_ncols
  integer(kind=JPIM), intent(out)           :: idx_seqno
  character(len=maxvarlen)                  :: masterKey

  integer(kind=JPIM)                   :: i, j, at_offset, at_idx

  type(C_PTR)                                   :: ptr_colname
  integer(kind=C_INT)                           :: cerr, size_name
  character(kind=C_CHAR), dimension(:), pointer :: f_ptr_colname
  character(len=256)                            :: colname

    idx_seqno =-1

    do i=1, c_ncols
      ptr_colname = C_NULL_PTR
      colname(:) = ""
      cerr = odb_read_get_column_name(odb_iterator, i-1, ptr_colname, size_name)
      call C_F_POINTER(CPTR=ptr_colname, FPTR=f_ptr_colname, shape=(/size_name/));
      do j=1, size_name
       if (f_ptr_colname(j) .eq. C_NULL_CHAR) exit  ! should be C_CHAR_NULL
       colname(j:j)  = f_ptr_colname(j)
      end do
      at_idx = index(colname, '@')
      if (at_idx > 0) then
        colnames(i) = colname(:at_idx-1)
      else
        colnames(i) = colname
      endif
      if (trim(colnames(i)) .eq. masterKey) then
        idx_seqno = i
      endif
    enddo
end subroutine odb_api_get_colnames
!----------------------------------------------------------------------
function odb_create_index_c(handle, dtname, dtname_length, odb_iterator, &
                            idx_odb, idx_size ) bind(C, name="odb_create_index_f90")
  use, intrinsic                      :: iso_c_binding
  use odb_module
  use odb_c_binding

  implicit none

  integer(C_INT), VALUE                :: handle
  character(kind=C_CHAR), dimension(*) :: dtname
  integer(C_INT),VALUE                 :: dtname_length
  TYPE(C_PTR)                          :: odb_iterator
  integer(kind=C_INT), dimension(*)    :: idx_odb
  integer(C_INT),VALUE                 :: idx_size
  integer(kind=C_INT)                  :: odb_create_index_c

  integer(kind=JPIM)                   :: f90_ncols
  integer(kind=JPIM)                   :: f90_handle
  character(len=64)                    :: f90_dtname
  integer(kind=JPIM)                   :: i, j, at_offset, at_idx

  INTEGER(kind=C_INT)                  :: c_ncols

  type(C_PTR)                                   :: ptr_colname
  integer(kind=C_INT)                           :: size_name
  character(kind=C_CHAR), dimension(:), pointer :: f_ptr_colname
  character(len=256)                            :: coltable, colname, temp(1)
  integer(kind=JPIM)                            :: f90_idx(1)
  integer(kind=JPIM)                            :: rc



  odb_create_index_c = 0
  if (dtname(1) /= '@') then
    at_offset = 1
    f90_dtname='@'
  else
    at_offset=0
    f90_dtname=""
  endif
  do i=1, dtname_length
    if (dtname(i) .eq. C_NULL_CHAR) exit
    f90_dtname(i+at_offset:i+at_offset)  = dtname(i)
  end do

  f90_handle = handle
  f90_ncols = ODB_getnames(f90_handle,f90_dtname,'name')

  odb_create_index_c = odb_read_get_no_of_columns(odb_iterator, c_ncols)

  if (idx_size == c_ncols) then
    do i=1, c_ncols
      odb_create_index_c = odb_read_get_column_name(odb_iterator, i-1, ptr_colname, size_name)
      call C_F_POINTER(CPTR=ptr_colname, FPTR=f_ptr_colname, shape=(/size_name/));
      do j=1, size_name
       if (f_ptr_colname(j) .eq. C_NULL_CHAR) exit  ! should be C_CHAR_NULL
       colname(j:j)  = f_ptr_colname(j)
      end do
      at_idx = index(colname, '@')
      if (at_idx > 0) then
        temp(1) = colname(:at_idx-1)
        coltable = colname(at_idx:)

      else
        temp(1) = colname
        coltable=""
      endif
      f90_idx=-1
      if (trim(coltable) == trim(f90_dtname)) then

      rc = ODB_varindex(f90_handle, f90_dtname, temp, f90_idx)
      if (f90_idx(1) > 0) then
        idx_odb(i)  = f90_idx(1)
      endif
      endif
    enddo
  endif

end function odb_create_index_c
!----------------------------------------------------------------------
function ODB_whatis(colname, flaglist)
  use, intrinsic                       :: iso_c_binding
  use odb_module

  implicit none
  character(len=*), intent(in)    :: colname
  type(C_PTR),        intent(in)  :: flaglist
  integer(kind=JPIM)              :: ODB_whatis
  integer(kind=JPIM)              :: idx_l, idx_r, i
  integer(kind=C_INT)             :: c_whatis, typeOfTable
  character(kind=C_CHAR),dimension(257)  :: c_colname

  ODB_whatis = ODB_NMDI

  c_colname(:)=' '
  do i=1,len(trim(colname))
    c_colname(i) = colname(i:i)
  enddo
  c_colname(i+1) = C_NULL_CHAR
  c_whatis = typeOfTable(c_colname, flaglist)
  ODB_whatis = c_whatis
  !write(0,*) 'ODB_whatis = ', trim(colname), ' ' ,  ODB_whatis
end function ODB_whatis
!----------------------------------------------------------------------
subroutine odb_fill_c(handle, filename, odb_hdr_array, odb_body_array, flaglist, c_masterKey, c_len, np) bind(C, name="odb_fill_f90")
  use, intrinsic                       :: iso_c_binding
  use odb_module
  use odb_c_binding
  use odbi

  implicit none

  type, bind(c) :: c_int_array
    integer(C_INT)                     :: len
    type(C_PTR)                        :: array
  end type c_int_array

  interface
     function ODB_whatis(colname, flaglist)
       use, intrinsic :: iso_c_binding
       use odb_module

       implicit none
       character(len=*), intent(in)    :: colname
       type(C_PTR), intent(in)         :: flaglist
       integer(kind=JPIM)              :: ODB_whatis
     end function ODB_whatis
  end interface


  integer(C_INT), VALUE                  :: handle
  character(kind=C_CHAR), dimension(*)   :: filename
  character(kind=C_CHAR), dimension(*)   :: c_masterKey
  type(C_PTR), intent(in)                :: flaglist
  integer(C_INT),VALUE                   :: c_len, np

  type(c_int_array), intent(in)          :: odb_hdr_array
  type(c_int_array), intent(inout)       :: odb_body_array
  integer(kind=JPIM)                     :: f90_handle
  integer(kind=JPIM)                     :: f90_np

  character(len=256), allocatable        :: tablenames(:)
  type(t_odbtable), allocatable          :: odbtables(:)
  integer(kind=JPIM)                     :: ntables
  integer(kind=JPIM)                     :: rc, i, j, f90_nrows, f90_ncols
  integer(kind=JPIM)                     :: idx_seqno, ihdr, ibody
  character(len=maxvarlen)               :: masterKey, cltable
  integer(C_INT), pointer                :: c_odb_hdr_array(:)
  integer(C_INT), pointer                :: c_odb_body_array(:)

  type(C_PTR)                                            :: odb_handler, odb_it
  character(kind=C_CHAR, len=64)                         :: config = C_NULL_CHAR
  integer(kind=C_INT)                                    :: cerr, new_dataset, ncols
  logical                                                :: LLfirstdataset, LLindexNotdone
  real(kind=C_DOUBLE), dimension(:), allocatable         :: one_row
  character(len=maxvarlen), allocatable                  :: colnames_in(:)
  character(len=64)                                      :: f90_filename
  integer(kind=JPIM)                                     :: previous_seqno

  f90_filename=""
  do i=1, maxvarlen
    if (filename(i) .eq. C_NULL_CHAR) exit
    f90_filename(i:i)  = filename(i)
  end do


  masterKey=""
  do i=1, c_len
    if (c_masterKey(i) .eq. C_NULL_CHAR) exit
    masterKey(i:i)  = c_masterKey(i)
  end do

! associate c_odb_hdr_array with an array allocated and filled in C++
  call C_F_POINTER( odb_hdr_array%array, c_odb_hdr_array, (/odb_hdr_array%len/))
  call C_F_POINTER( odb_body_array%array, c_odb_body_array, (/odb_body_array%len/))

  write(0,*) 'masterKey= ', trim(masterKey)
  write(0,*) 'number of BODY entries = ', odb_body_array%len
  write(0,*) 'number of HDR entries = ', odb_hdr_array%len
  f90_handle = handle
  f90_np = np

  ntables = ODB_getnames(f90_handle, '*', 'table')
  allocate(tablenames(ntables))
  ntables = ODB_getnames(f90_handle, '*', 'table', tablenames)
  allocate(odbtables(ntables))
  odbtables(:)%table_kind = -1
  do j=1,ntables
    NULLIFY(odbtables(j)%data)
    NULLIFY(odbtables(j)%index)
    NULLIFY(odbtables(j)%colnames)
    odbtables(j)%table_kind = ODB_whatis(trim(tablenames(j)), flaglist)
    if (odbtables(j)%table_kind == ODB_HDR_ALIGNED .or. odbtables(j)%table_kind  == ODB_BODY_ALIGNED) then
      odbtables(j)%tablename = trim(tablenames(j))
      cltable = odbtables(j)%tablename
      f90_ncols = ODB_getnames(f90_handle,cltable,'name')
      allocate(odbtables(j)%colnames(f90_ncols))
      rc = ODB_getnames(f90_handle,cltable,'name', outnames=odbtables(j)%colnames)
!*AF      write(0,*) trim(cltable), 'f90_ncols = ', f90_ncols
      if (odbtables(j)%table_kind  == ODB_HDR_ALIGNED) then
        allocate(odbtables(j)%data(int(odb_hdr_array%len),0:f90_ncols))
      else
        allocate(odbtables(j)%data(int(odb_body_array%len),0:f90_ncols))
      endif
      odbtables(j)%data(:,:) = 0.0
    endif
  enddo

! start to read the input file
  LLfirstdataset=.true.
  LLindexNotdone=.true.
  ihdr=1
  ibody=1
!*AF  write(0,*) 'LLfirstdataset=.true. ', LLfirstdataset, ' LLindexNotdone= ', LLindexNotdone
  odb_handler = odb_read_new(config, cerr)
  odb_it = odb_read_iterator_new(odb_handler, filename, cerr);
  cerr = odb_read_get_no_of_columns(odb_it, ncols)
  if (allocated(one_row)) deallocate(one_row)
  allocate(one_row(ncols))

  previous_seqno = -1
  do while (odb_read_get_next_row(odb_it, ncols, one_row, new_dataset) == 0)
    LLindexNotdone = (new_dataset > 0) .or. LLfirstdataset
    if (LLindexNotDone) then
      LLfirstdataset = .false.
! we must compute the list of index in ODB-1 valid in the current ODB-2 one_row
      allocate(colnames_in(ncols))
      colnames_in(:) = ""
      call odb_api_get_colnames(odb_it, colnames_in, ncols, idx_seqno, masterKey)
!*AF      do j=1, ncols
!*AF        write(0,*) 'colnames_in (odb-api) (', j,')= ', trim(colnames_in(j))
!*AF      enddo
      do j=1, ntables
        if (odbtables(j)%table_kind > 0) then
          allocate(odbtables(j)%index(ncols))
          rc = ODB_varindex(f90_handle,odbtables(j)%tablename , colnames_in, odbtables(j)%index)
          !write(0,*) 'table = ', trim(odbtables(j)%tablename), ' ', odbtables(j)%index
        endif
      enddo

      deallocate(colnames_in)

!*AF      write(0,*) 'new_dataset'
    endif
    do j=1, ntables
      if (odbtables(j)%table_kind > 0) then
       do i=1, ncols
         if (odbtables(j)%table_kind == ODB_HDR_ALIGNED .and. &
             one_row(idx_seqno) /= previous_seqno &
             .and. odbtables(j)%index(i) > 0) then
           !write(0,*) one_row(idx_seqno), c_odb_hdr_array(ihdr), ihdr, 'size odbtables ', j, trim(odbtables(j)%tablename), ' ', odbtables(j)%index(i)
           odbtables(j)%data(ihdr, odbtables(j)%index(i)) = one_row(i)
         else if (odbtables(j)%table_kind == ODB_BODY_ALIGNED .and. &
             odbtables(j)%index(i) > 0) then
           !write(0,*) ibody, 'size odbtables ', j, trim(odbtables(j)%tablename), ' ', odbtables(j)%index(i)
           odbtables(j)%data(ibody, odbtables(j)%index(i)) = one_row(i)
         endif
       enddo
      endif
    enddo
    c_odb_body_array(ibody) = one_row(idx_seqno)
    ibody = ibody + 1
    if (one_row(idx_seqno) /= previous_seqno) then
      previous_seqno = c_odb_hdr_array(ihdr)
      ihdr = ihdr + 1
    endif
  enddo
  if (allocated(one_row)) deallocate(one_row)
  cerr = odb_read_iterator_delete(odb_it)
  cerr = odb_read_delete(odb_handler)
! deallocation
 do j=1,ntables
    if (odbtables(j)%table_kind > 0) then
     f90_nrows = size(odbtables(j)%data,1)
     f90_ncols = size(odbtables(j)%data,2)
!*AF     write(0,*) 'ODB_put ', trim(odbtables(j)%tablename), f90_nrows, f90_ncols, f90_np
     !do i=1, f90_nrows
     !  write(*,*) i, 'row ', odbtables(j)%data(i,:)
     !enddo
     rc = ODB_put(f90_handle, odbtables(j)%tablename, odbtables(j)%data, &
        f90_nrows, f90_ncols, poolno = f90_np)
    endif
    if (associated(odbtables(j)%data)) deallocate(odbtables(j)%data)
    if (associated(odbtables(j)%index)) deallocate(odbtables(j)%index)
    if (associated(odbtables(j)%colnames)) deallocate(odbtables(j)%colnames)
 enddo
 deallocate(odbtables)
 deallocate(tablenames)
end subroutine odb_fill_c
!----------------------------------------------------------------------
function odb_put_one_row_c(handle, dtname, dtname_length, odb_row, idx_odb, ncols, np) bind(C, name="odb_put_one_row_f90")
  use, intrinsic                       :: iso_c_binding
  use odb_module
  use odb_c_binding

  implicit none

  integer(C_INT), VALUE                :: handle
  character(kind=C_CHAR), dimension(*) :: dtname
  integer(C_INT),VALUE                 :: dtname_length
  real(kind=C_DOUBLE), dimension(*)    :: odb_row
  integer(kind=C_INT), dimension(*)    :: idx_odb
  integer(C_INT),VALUE                 :: ncols
  integer(C_INT),VALUE                 :: np
  integer(kind=C_INT)                  :: odb_put_one_row_c

  integer(kind=JPIM)                   :: f90_ncols
  integer(kind=JPIM)                   :: f90_handle
  integer(kind=JPIM)                   :: f90_np
  character(len=64)                    :: f90_dtname
  integer(kind=JPIM)                   :: i, at_offset
  integer(kind=JPIM)                   :: nrows
  real(kind=JPRB), allocatable         :: f90_one_row(:,:)

  odb_put_one_row_c = 0
  nrows = 1

  if (dtname(1) /= '@') then
    at_offset = 1
    f90_dtname='@'
  else
    at_offset=0
    f90_dtname=""
  endif
  do i=1, dtname_length
    if (dtname(i) .eq. C_NULL_CHAR) exit
    f90_dtname(i+at_offset:i+at_offset)  = dtname(i)
  end do

  f90_handle = handle
  f90_ncols = ODB_getnames(f90_handle,f90_dtname,'name')

  f90_np = np
  allocate(f90_one_row(nrows,0:f90_ncols))

  f90_one_row=0
  do i=1, ncols
    f90_one_row(1,idx_odb(i)) = odb_row(i)
  enddo

  odb_put_one_row_c = ODB_put(f90_handle, f90_dtname, f90_one_row, nrows, f90_ncols, poolno = f90_np)
  deallocate(f90_one_row)

end function odb_put_one_row_c
!----------------------------------------------------------------------
subroutine compute_links(ilink, c_odb_len_array)
  use, intrinsic :: iso_c_binding
  use            :: odb_module

  implicit none

  integer(C_INT), intent(in)         :: c_odb_len_array(:)
  integer(KIND=JPIM), intent(inout)    :: ilink(:)

  integer(kind=JPIM)          :: i, j
  integer(kind=JPIM)          :: previous_seqno, bodylen

  !write(0,*) 'Size of ilink = ', size(ilink, dim=1), size(c_odb_len_array, dim=1)

  if (size(ilink, dim=1) == size(c_odb_len_array, dim=1)) then
    ilink=1
  else
    ilink=-1
    previous_seqno = c_odb_len_array(1)
    bodylen=0
    j=1
    do i=1, size(c_odb_len_array, dim=1)
!*AF      write(0,*) 'previous_seqno = ', previous_seqno, ' seqno = ', c_odb_len_array(i)
      if (previous_seqno /= c_odb_len_array(i)) then
! we have a different seqno so we can set the len of the previous body part
        ilink(j) = bodylen
!*AF        write(0,*) 'ilink(', j, ')=', ilink(j)
        bodylen=0
        j = j+1
        previous_seqno = c_odb_len_array(i)
      endif
      bodylen = bodylen + 1
    enddo
    if (previous_seqno == c_odb_len_array(size(c_odb_len_array, dim=1))) ilink(j) = bodylen
  endif
end subroutine compute_links
!----------------------------------------------------------------------
function odb_update_links_c(handle, dtname, dtname_length, poolno, odb_hdr_array, odb_body_array, flaglist) bind(C, name="odb_update_links_f90")
  use, intrinsic :: iso_c_binding
  use            :: odb_module

  implicit none

  interface
    subroutine compute_links(ilink, c_odb_len_array)
      use, intrinsic :: iso_c_binding
      use            :: odb_module
      integer(C_INT), intent(in)         :: c_odb_len_array(:)
      integer(KIND=JPIM), intent(inout)  :: ilink(:)
     end subroutine compute_links

     function ODB_whatis(colname, flaglist)
       use, intrinsic :: iso_c_binding
       use odb_module

       implicit none
       character(len=*), intent(in)    :: colname
       type(C_PTR), intent(in)  :: flaglist
       integer(kind=JPIM)              :: ODB_whatis
     end function ODB_whatis
  end interface

  type, bind(c) :: c_int_array
    integer(C_INT)                     :: len
    type(C_PTR)                        :: array
  end type c_int_array

  type(C_PTR), intent(in)              :: flaglist
  integer(C_INT),VALUE                 :: handle
  character(kind=C_CHAR), dimension(*) :: dtname
  integer(C_INT),VALUE                 :: dtname_length
  integer(C_INT),VALUE                 :: poolno
  type(c_int_array), intent(in)        :: odb_hdr_array
  type(c_int_array), intent(in)        :: odb_body_array
  integer(C_INT)                       :: odb_update_links_c

  integer(C_INT), pointer              :: c_odb_hdr_array(:)
  integer(C_INT), pointer              :: c_odb_body_array(:)

  character(len=64)                    :: f90_dtname
  integer(kind=JPIM)                   :: f90_poolno
  integer(kind=JPIM)                   :: f90_handle
  integer(kind=JPIM)                   :: i, j
  integer(kind=JPIM)                   :: rc
  integer(kind=JPIM)                   :: nrows, ncols, nra
  character(len=256), allocatable      :: colnames(:)
  integer(kind=JPIM), allocatable      :: col_aligned(:)
  character(len=256)                   :: colname
  real(KIND=JPRB), allocatable         :: x(:,:)
  integer(KIND=JPIM), allocatable      :: ilink_hdr(:)
  integer(KIND=JPIM), allocatable      :: ilink_body(:)
  integer(kind=JPIM)                   :: ioffset_hdr
  integer(kind=JPIM)                   :: ioffset_body

  odb_update_links_c = 0

! associate c_odb_hdr_array with an array allocated and filled in C++
  call C_F_POINTER( odb_hdr_array%array, c_odb_hdr_array, (/odb_hdr_array%len/))

! associate c_odb_body_array with an array allocated and filled in C++
  call C_F_POINTER( odb_body_array%array, c_odb_body_array, (/odb_body_array%len/))

  f90_handle = handle
  f90_poolno = poolno
  f90_dtname=""
  do i=1, dtname_length
    if (dtname(i) .eq. C_NULL_CHAR) exit
    f90_dtname(i:i)  = dtname(i)
  end do


  rc = ODB_addview(f90_handle, f90_dtname, abort = .false.)
  write(0,*) 'rc = ', rc, 'odb_update_links_c f90_handle = ', f90_handle, ' f90_poolno = ', f90_poolno, &
             ' f90_dtname = ', trim(f90_dtname), ' odb_hdr_array.len = ',size(c_odb_hdr_array), &
             ' odb_body_array.len = ',size(c_odb_body_array)

  if (rc > 0) then
! this SQL is registered and we can fecth data from ODB
    rc = ODB_select(f90_handle,f90_dtname,nrows,ncols,nra=nra,poolno=f90_poolno)
!*AF    write(0,*) 'odb_update_links_c ', trim(f90_dtname), ' nrows = ', nrows, ' ncols = ', ncols
    if (nrows > 0) then
      ALLOCATE(x(nra,0:ncols))
      ALLOCATE(ilink_hdr(nrows))
      ALLOCATE(ilink_body(nrows))
      ALLOCATE(colnames(ncols))
      ALLOCATE(col_aligned(ncols))
      rc = ODB_get(f90_handle,f90_dtname,x,nrows,ncols,poolno=f90_poolno)
      call compute_links(ilink_hdr, c_odb_hdr_array)
      call compute_links(ilink_body, c_odb_body_array)
      rc = ODB_getnames(f90_handle, f90_dtname, 'name', colnames)
      col_aligned(:)=ODB_NMDI
      do j=1, ncols
        colname = trim(colnames(j))
        if (len(colname) > 10 .and. colname(1:10) == 'LINKOFFSET') then
           col_aligned(j) = ODB_whatis(colname, flaglist) ! offset
           col_aligned(j+1) = col_aligned(j)              ! len
        endif
      enddo

      ioffset_hdr = 0
      ioffset_body = 0
       ! write(0,*) 'nrows = ', nrows, ' ncols = ', ncols
      do i=1, nrows
        do j=1, ncols
          colname = trim(colnames(j))
          if (len(colname) > 7 .and. colname(1:7) == 'LINKLEN') then
             if (col_aligned(j) == ODB_HDR_ALIGNED) then
               x(i,j) = ilink_hdr(i)
             else if (col_aligned(j)  == ODB_BODY_ALIGNED) then
               x(i,j) = ilink_body(i)
             else
               x(i,j) = 0
             endif
          else if (len(colname) > 10 .and. colname(1:10) == 'LINKOFFSET') then
             if (col_aligned(j)  == ODB_HDR_ALIGNED) then
               x(i,j) = ioffset_hdr
             else if (col_aligned(j) == ODB_BODY_ALIGNED) then
               x(i,j) = ioffset_body
             else
               x(i,j) = 0
             endif
          endif
        enddo
        ioffset_hdr = ioffset_hdr + ilink_hdr(i)
        ioffset_body = ioffset_body + ilink_body(i)
      enddo
      rc = ODB_put(f90_handle,f90_dtname,x, nrows,ncols,poolno=f90_poolno)
      DEALLOCATE(x)
      DEALLOCATE(ilink_hdr)
      DEALLOCATE(ilink_body)
      DEALLOCATE(colnames)
      DEALLOCATE(col_aligned)
    endif
    rc = ODB_cancel(f90_handle,f90_dtname,poolno=f90_poolno)
  else
   write(0,*) '******Warning: retrieval ', trim(f90_dtname), ' is not registered'
  endif

end function odb_update_links_c

!----------------------------------------------------------------------
