#!/bin/ksh

set -ex

which odb2netcdf.x

rm -f odb2netcdf_2d.nc{,.dump}

odb2netcdf.x -i odb2netcdf_2d -2d -o odb2netcdf_2d.nc

ncdump odb2netcdf_2d.nc > odb2netcdf_2d.nc.dump

diff odb2netcdf_2d.cdl odb2netcdf_2d.nc.dump
