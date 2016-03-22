#!/bin/ksh

set -ex

which odb2netcdf.x

rm -f odb2netcdf_1d.nc{,.dump}

odb2netcdf.x -i odb2netcdf_1d.odb -o odb2netcdf_1d.nc

ncdump odb2netcdf_1d.nc > odb2netcdf_1d.nc.dump

diff odb2netcdf_1d.cdl odb2netcdf_1d.nc.dump
