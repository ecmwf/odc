
program odb2_to_odb1_era

  use odb1
  use odb2
  implicit none

  integer,parameter     :: nmaxsopt=1000 ! Quite generous, but please adapt if required
  integer               :: ioptval,getopt
  character*120         :: carg
  character             :: options*14,copt

  integer            :: ipool,npools,iproc,nproc,iodb2,nodb2,nsetval,isetval,iseqoff,iseqfirst
  integer,allocatable:: ioff(:)
  character(len=128) :: table,croot,dbname
  character(len=128) :: odb2file(nmaxsopt),csetval(nmaxsopt)
  type (todb1)       :: sch1,sch2       ! odb-1 schema files
  type (todb2)       :: db2             ! odb-2 

  integer :: itable

! 1. Crack options
! ----------------

  data options/'i:r:l:s:w:f:o:'/

! Set default values
  npools=1; nsetval=0
  nodb2=0 ; odb2file(:)="" ; croot="" ; csetval(:)=""; dbname="ECMA"
  iseqoff=0 ; iseqfirst=0
  do
    ioptval=getopt(options,carg)
    carg=trim(carg)
    copt=char(ioptval)
 
    if (ioptval <=0) exit
    if (copt == 's') read(carg,*)npools
    if (copt == 'f') read(carg,*)iseqfirst
    if (copt == 'o') read(carg,*)iseqoff
    if (copt == 'i') nodb2            = nodb2+1
    if (copt == 'w') nsetval          = nsetval+1
                     if (max(nodb2,nsetval)>nmaxsopt) then
                         write(6,*)'Please increase nmaxsopt:',nmaxsopt 
                         call abort
                     endif
    if (copt == 'i') odb2file(nodb2)  = trim(carg)
    if (copt == 'w') csetval(nsetval) = trim(carg)
    if (copt == 'r') croot            = trim(carg)
    if (copt == 'l') dbname           = trim(carg)
  enddo

! Add "@" to croot in case not present
  if((croot/="").and.(index(croot,"@"))==0)croot="@"//trim(croot)

  if (nodb2==0) then
     write(6,*)'Usage: odb2_to_odb1_era -i odb2file [-l dbname -r @mytableroot -s npools -o iseqno_offset -w column@table=my_value ]'
     return
  endif

  write(6,*)       "Welcome to odb2_to_odb1_era:"
  do iodb2=1,nodb2
     write(6,'(2A)')  "ODB-2 input file:",trim(odb2file(iodb2)) 
  enddo
  write(6,'(2A)')  "ODB-1   database:",trim(dbname) 
  write(6,'(2A)')  "ODB-1 root table:",trim(croot) 
  write(6,'(A,I4)')"Number of pools :",npools


! 2. Process
! ----------

! Open ODB-1, read schema and place its contents in sch1
  call odb1_open(sch1,dbname,npools,iproc,nproc)
  write(6,'(A,I4 )')"Number of procs :",nproc
  write(6,'(A,I4 )')"Index  of proc  :",iproc
  write(6,'(A,I12)')"First Seqno     :",iseqfirst
  write(6,'(A,I12)')"Seqno  ioffset  :",iseqoff

! Setup ODB-2 libraries
  call odb_start()

! Do the work
  allocate(ioff(npools))
  do ipool=iproc,npools,nproc           ! my pools only
     ioff(ipool)= iseqfirst + (ipool-1)*iseqoff
     do iodb2=ipool,nodb2,npools        ! my files only, if any

     ! Read ODB-2 file and place contents in db2
       call odb2_read_all(db2,odb2file(iodb2))
     ! call odb2_header(db2)  ! Print summary

       call odb22odb1(sch2,db2,sch1,croot)
     ! call odb1_header(sch2)  ! Print summary

       call odb1_merge(sch2,sch1)

     ! Overwrite some columns, if any
       do isetval=1,nsetval
           call odb1_set_value(sch1,csetval(isetval))
       enddo
       call odb1_reassign_seqno(sch1,ioff(ipool))

       call odb1_write(sch1,ipool)
     enddo

  enddo

! Close the ODB-1 database
  call odb1_close(sch1)
  deallocate(ioff)

contains

!-----------------------------------------------------------------------------------------

subroutine odb22odb1(db1,db2,template1,croot)
! convert odb2 db2 database to odb1 db1, using the odb1 template
  implicit none
  type (todb2) :: db2
  type (todb1) :: template1,db1

  character(len=128)             :: croot

  call odb12_dress_columns(db2,template1)          ! add any missing @table info from template1 in db2
  call create_odb1_schema(db1,db2,template1,croot) ! Figure out ODB-1 table structure
  call odb12_compress(db1,db2)                     ! Compress ODB-2 table to ODB-1 tables

end subroutine odb22odb1

!-----------------------------------------------------------------------------------------

subroutine odb12_dress_columns(db2,db1)
! In case missing: add @table to db2 columns, from info in db1 schema
  implicit none
  type (todb2) :: db2
  type (todb1) :: db1

  type (odb1_table), pointer :: tbl
  character(len=128)         :: col2,col1
  integer                    :: i1,i2,j,itable

  do i2=1,db2%ncol
     col2=""; write(col2,'(A)')trim(db2%cname(i2))
     if(index(col2,"@")/=0) cycle                        ! it's o.k.

     do itable=1,db1%ntable
        tbl => db1%tbl(itable)
        do i1=1,tbl%ncol
           col1="" ; write(col1,'(A)')trim(tbl%cname(i1))
           j=index(col1,"@")-1
           if ( trim(col2)==trim(col1(1:j)) ) then
              write(*,*)"Dress table: ",trim(col2)," --> ",trim(col1)
              col2="" ; write(col2,'(A)')trim(col1)
              exit
           endif
        enddo
     enddo
     db2%cname(i2)="" ; write(db2%cname(i2),'(A)')trim(col2)
  enddo

end subroutine odb12_dress_columns

!-----------------------------------------------------------------------------------------

subroutine create_odb1_schema(db1,db2,template1,croot)

  implicit none
  type (todb2)        :: db2
  type (todb1)        :: template1,db1
  character(len=128) :: croot

  type (odb1_table), pointer     :: tbl,dbl
  logical,allocatable            :: ltable(:),ltall(:)
  integer,allocatable            :: iparent(:),itree(:),imap(:)
  character(len=128),allocatable :: tname(:),cname(:),ctype(:)
  integer                        :: i,j,k,l,it,ip,nt,ntable,ncol,iroot
  character*128                  :: tab,parent
  logical                        :: lkeep

  nt=template1%ntable
  allocate(ltall  (0:nt)) ; ltall  (0:nt)=.false.
  allocate(ltable (0:nt)) ; ltable (0:nt)=.false.
  allocate(iparent(0:nt)) ; iparent(0:nt)=0
  allocate(itree  (0:nt)) ; itree  (0:nt)=0

  iroot=odb1_find_index(template1,croot)
  if(trim(croot)=="")iroot=-1

! Find all tables, plus missing link tables up to "@"
  do i=1,db2%ncol
     j=index(db2%cname(i),"@")
     tab=""; write(tab,'(A)')trim(db2%cname(i)(j:))

     it=odb1_find_index(template1,tab)
     if (ltable(it)) cycle
         ltable(it)=.true.
     do
       ltall(it)=.true.
       if(trim(tab)=="@") exit ! we've reached root
       ip=odb1_find_parent(template1,tab,parent)
       iparent(it)=ip
       it=ip
       tab="";write(tab,'(A)')trim(parent)
     enddo
  enddo

! Make a selection, depending on the contents of croot
  do i=0,nt  ! disregard tables that are ancestor from croot
    if(.not.ltable(i)) cycle
    if(iroot<0) cycle   ! take everything on board
    j=i
    lkeep=(j==iroot)
    do
      j=iparent(j)
      if (j==iroot) lkeep=.true. 
      if (j==0) exit
    enddo
    ltable(i)=lkeep
  enddo

  ntable=0
  do i=1,nt ! check whether any ancestors belong to the ODB2 table
    if(.not.ltall(i)) cycle
    j=i
    lkeep=ltable(j)
    do
      j=iparent(j)
      if (j==0) exit
      if (ltable(j)) lkeep=.true.
      itree(i)=itree(i)+1
    enddo
    ltall(i)=lkeep
    if(.not.lkeep )cycle
    ntable=ntable+1
  enddo

! Unsorted Table names ; only used to find childs later on
  allocate(tname(ntable)) ; tname(:)=""
  k=0
  do i=1,nt ; if(.not.ltall(i)) cycle
     k=k+1
     tbl => template1%tbl(i)
     write(tname(k),'(A)')trim(tbl%tname)
  enddo

! Sort Tables, such that parent(i) = table(j) ==> i>j
  allocate(imap(0:nt)); imap(:)=0
  allocate(cname(db2%ncol),ctype(db2%ncol)) ! Safe upper limit

  db1%ntable=ntable
  allocate(db1%tbl(0:ntable))

  write(db1%tbl(0)%tname,'(A)')"@" ; db1%tbl(0)%ncol=0
  k=0
  do j=0,maxval(itree) 
     do i=1,nt
        if(.not.ltall(i)) cycle
        if(itree(i)/=j) cycle
        k=k+1

        tbl => template1%tbl(i)
        tab=""; write(tab,'(A)')trim(tbl%tname)
        imap(i)=k
        ip=imap(iparent(i))

        ncol=0 ; cname(:)="" ; ctype(:)=""
        do l=1,db2%ncol                 ! Find all regular columns
           it=index(db2%cname(l),"@")
           if( trim(db2%cname(l)(it:))/=trim(tab) ) cycle
           ncol=ncol+1
           write(cname(ncol),'(A)')trim(db2%cname(l))
           call odb1_find_type(template1,cname(ncol),ctype(ncol))
        enddo

        do l=1,ntable                   ! Add links to children
           if(odb1_child_linkindex(tbl,tname(l))==0)cycle

           ncol=ncol+1
           write(ctype(ncol),'(A)')trim(tloffset)
           write(cname(ncol),'(A,"(",A,")",A)')&
           &    trim(cloffset),trim(tname(l)(2:)),trim(tab)
           ncol=ncol+1
           write(ctype(ncol),'(A)')trim(tlinklen)
           write(cname(ncol),'(A,"(",A,")",A)')&
           &    trim(clinklen),trim(tname(l)(2:)),trim(tab)
        enddo 

        call odb1_create_table(db1%tbl(k),tab,ip,ncol,cname(1:ncol),ctype(1:ncol))
     enddo
  enddo
  deallocate(ltall,ltable,iparent,itree,imap,tname,cname,ctype)

end subroutine create_odb1_schema

!-----------------------------------------------------------------------------------------

subroutine odb12_compress(db1,db2)
! Fill and compress data contents of ODB2 db2, in schema-matched ODB1 db1
! group aligned tables, and compress if contents of a 'super' row has not changed

  use aligned
  implicit none
  type (todb1) :: db1
  type (todb2) :: db2

  type (odb1_table), pointer :: tbl,tbp
  integer                    :: k,i,it,ic,i1,i2,irow,j,ntable,ncol,ncol2,nrow2,nrowc,nrowp,kp,nalign
  integer,allocatable        :: icmp(:,:),icol(:),ialign(:)
  real*8 ,allocatable        :: data(:,:)
  character(len=128)         :: tab
  
  ntable=db1%ntable
  nrow2 =db2%nrow
  ncol2 =db2%ncol
 
  allocate(icol(ncol2))              ! map between odb2 column and odb1 column
  allocate(ialign(ntable))           ! list of tables that are aligned
  allocate(icmp(0:nrow2+1,0:ntable)) ! index of compressed table;
           icmp(0:nrow2+1,0:ntable)=-1

  write(*,*)'ODB-2 data compression:'
  do k=1,ntable
     tbl => db1%tbl(k)

     tab="" ; write(tab,'(A)')trim(tbl%tname)
     kp=tbl%iparent

     if (icmp(1,k)<0) then    ! If it's 0 we've already dome this column
        ialign(:)=0 ; nalign=0
        ncol=0 ; icol(:)=0
        do i=k,ntable
           tbp => db1%tbl(i)
           if(.not.odb1_aligned(tab,tbp%tname)) cycle
           nalign=nalign+1 ; ialign(nalign)=i

          ! Find odb-2 columns that relate with table tbp
            do j=1,db2%ncol
               it=index(db2%cname(j),"@")
               if( trim(db2%cname(j)(it:))/=trim(tbp%tname) ) cycle
               ncol=ncol+1 ; icol(ncol)=j
            enddo

        enddo

      ! Determine the level of compression for each table
      ! Info ends up in the icmp(nrow2,ntable) array
        allocate(data(2,0:ncol))
        i1=1; 
        nrowc=0 
        do j=1,nalign
           icmp(1,ialign(j))=0 ! Always take the first row
        enddo

        do i=1,nrow2
           i2=1+mod(i1,2)
           data(i2,0)=icmp(i,kp)        ! to check whether parent changed
           do j=1,ncol
              data(i2,j)=db2%data(icol(j),i)
           enddo
           if(i>1) then                 ! contents changed?
              do j=0,ncol
                  if(data(i2,j)/=data(i1,j)) exit
              enddo
              if(j/=ncol+1) nrowc=nrowc+1
              do j=1,nalign
                 icmp(i,ialign(j))=nrowc
              enddo
           endif
           i1=i2
        enddo
        do j=1,nalign
           icmp(nrow2+1,ialign(j))=nrowc+1 ! convenient extension to calculate differences
        enddo
        deallocate(data)
     endif

   ! Now we know the # of rows, allocate space and fill
     nrowc=icmp(nrow2+1,k)
     call odb1_alloc_data(tbl,nrowc)

   ! Find odb-2 columns that relate with table tbl
     ncol=0 ; icol(:)=0
     do j=1,db2%ncol
        it=index(db2%cname(j),"@")
        if( trim(db2%cname(j)(it:))/=trim(tab) ) cycle
        ncol=ncol+1 ; icol(ncol)=j
     enddo

     allocate(data(2,ncol))
     irow=0
     do i=1,nrow2
        if (icmp(i,k)==icmp(i-1,k)) cycle  ! new compressed row?
        do j=1,ncol
          data(1,j)=db2%data(icol(j),i)
        enddo
        irow=irow+1
        call odb1_add_block(tbl,data(1:1,1:ncol),irow,1,1,ncol)
     enddo
     deallocate(data)

     write(*,*)'# Columns/Children/Rows ',ncol,(tbl%ncol-ncol)/2,nrowc,trim(tab)

   ! Sort out the links to its parent
     if(kp==0) cycle
     tbp => db1%tbl(kp)

     nrowp=tbp%nrow
     allocate(data(nrowp+1,2))

     irow=0
     do i=1,nrow2+1    
        if (icmp(i,kp)==icmp(i-1,kp)) cycle
        irow=irow+1 
        data(irow,1)=icmp(i,k)                   ! Offset
     enddo

     do irow=1,nrowp
        data(irow,2)=data(irow+1,1)-data(irow,1) ! Length
     enddo

     ic=odb1_child_linkindex(tbp,tab)            ! index of link table
     call odb1_add_block(tbp,data(1:nrowp,1:2),1,ic,nrowp,2)

     deallocate(data)

  enddo
  deallocate(icol,icmp)
end subroutine odb12_compress

!-----------------------------------------------------------------------------------------

end program odb2_to_odb1_era
