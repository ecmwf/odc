program solar_elevation
! Insert and calculate solar_elevation@conv from date, time, lat, lon
! Hans Hersbach, ECMWF  May 2013

  use odb2
  implicit none

  integer                        :: ioptval,getopt
  character*120                  :: carg
  character                      :: options*4,copt
  character(len=128)             :: ifile,ofile
  type (todb2)                   :: odbi,odbo
  character(len=128),allocatable :: cname(:)
  real*8,allocatable             :: xrow(:)
  integer                        :: ncol,nout,iread,ic
  integer                        :: idat,itim,ilat,ilon,isol
  integer                        :: yyyymmdd,hhmmss,mm,dd,hh
  real                           :: xlat,xlon,xsol

! 1. Crack options
! ----------------
  data options/'o:i:'/

! Set default values
  ifile="" ; ofile=""
  do
    ioptval=getopt(options,carg)
    carg=trim(carg)
    copt=char(ioptval)

    if (ioptval <=0) exit
    if (copt == 'o') ofile = trim(carg)
    if (copt == 'i') ifile = trim(carg)
  enddo

!-Open input file
  call odb_start()
  call odb2_open(odbi,ifile,status='r')
  call odb2_get(odbi,ncol=ncol)
     allocate(cname(ncol+1))
     allocate(xrow(ncol+1))
     call odb2_get(odbi,ncol=ncol,cname=cname)

  idat=find_column(odbi,            "date@hdr" )
  itim=find_column(odbi,            "time@hdr" )
  ilat=find_column(odbi,             "lat@hdr" )
  ilon=find_column(odbi,             "lon@hdr" )

!-Add my solar elevation
  ic=ncol
  ic=ic+1 ; cname(ic)="pk9real:solar_elevation@peter"; isol=ic
  nout=ic

!-Open output and create columns
  call odb2_open(odbo,ofile,status='w')
  call odb2_set_columns(odbo,nout,cname)

!-Loop through odb file
  iread=1
  do
    call odb2_read (odbi,xrow,ncol,iread) ; if(iread==0) exit

    xlat=xrow(ilat)
    xlon=xrow(ilon)

    yyyymmdd=int(xrow(idat))
      hhmmss=int(xrow(itim)) ; if(mod(hhmmss,100)==60) hhmmss=100*(hhmmss/100)+59

    mm=mod(yyyymmdd,10000)/100
    dd=mod(yyyymmdd,100)
    hh=hhmmss/10000

    call diurnal(mm,dd,hh,xlat,xlon,xsol)
    xrow(isol)=xsol

    call odb2_write(odbo,xrow,nout,1)
  enddo
  call odb2_close(odbi)
  call odb2_close(odbo)

!-Clean up
  deallocate(cname,xrow)

end program solar_elevation
