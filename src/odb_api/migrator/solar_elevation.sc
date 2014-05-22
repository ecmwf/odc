# ---------------------------------------------------------------------------------------

  flist="odb2_flag_definitions.f90 odb2.f90 diurnal.F90 solar_elevation.f90"
  WDIR=$SCRATCH/solar_elevation ; [[ ! -d $WDIR ]] && mkdir -p $WDIR
  SDIR=$PWD

  cd $SDIR ; cp $flist $WDIR

  prog=solar_elevation.x

  cd $WDIR
  [[ -f $prog ]] && rm -f $prog


  unset ODB_API_VERSION ;  export ODB_VERSION=CY38R2.001
  use odb
  use pgi-10.8
  F90="pgf90 -fPIC -r8 -O3"

  LINKS=" $ECLIB $ODB_FORTRAN_INCLUDE $ODB_FORTRAN_LIB $GRIB_API_INCLUDE $GRIB_API_LIB"
  $F90 $flist -o $prog $LINKS -rpath=$ODB_API_DIR/$ODB_API_VERSION/lib


  odbi=/scratch/er/er9/radiosondes.odb
  odbo=$WDIR/radiosondes_myse.odb
  time ./$prog -i $odbi -o $odbo
