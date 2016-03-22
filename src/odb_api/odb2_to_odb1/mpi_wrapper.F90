!
! Copyright 2011 ECMWF
!
! This software was developed at ECMWF for evaluation
! and may be used for academic and research purposes only.
! The software is provided as is without any warranty.
!
! This software can be used, copied and modified but not
! redistributed or sold. This notice must be reproduced
! on each copy made.
!

!> Interfaces to be called from C++ for MPI initialize/finalize
!> @author Anne Fouilloux

! ------------------------------------------------------------------------------

subroutine mpi_setup_c() bind(c,name='mpi_setup_f90')
use iso_c_binding
USE MPL_MODULE, ONLY  : MPLUSERCOMM, LMPLUSERCOMM
implicit none
include "mpif.h"

integer :: IER

CALL MPI_INIT(IER)

LMPLUSERCOMM = .TRUE.
MPLUSERCOMM = MPI_COMM_WORLD

end subroutine mpi_setup_c

! ------------------------------------------------------------------------------

subroutine mpi_finalize_c() bind(c,name='mpi_finalize_f90')
use iso_c_binding
implicit none

integer :: IER
logical :: LL

CALL MPI_FINALIZED(LL,IER)
IF (.not.LL) THEN
  CALL MPI_FINALIZE(IER)
ENDIF

end subroutine mpi_finalize_c

! ------------------------------------------------------------------------------

subroutine mpi_print_info_c() bind(c,name='mpi_print_info_f90')
use iso_c_binding
USE MPL_MODULE, ONLY  : MPLUSERCOMM, LMPLUSERCOMM
implicit none
include "mpif.h"

integer :: NP, ME, IER
CALL MPI_COMM_SIZE(MPI_COMM_WORLD,NP,IER)
CALL MPI_COMM_RANk(MPI_COMM_WORLD,ME,IER)
write(0,*) "MPI_SETUP_F90: rank,nprocs=",me,np
write(0,*) "MPI_SETUP_F90: MPLUSERCOMM=",MPLUSERCOMM

end subroutine mpi_print_info_c

! ------------------------------------------------------------------------------

subroutine set_err_trap_c() bind(c,name='set_err_trap_f90')
    use yomerrtrap
    call set_err_trap()
end subroutine
