export ODB_CXX_INCLUDE="-I/usr/local/lib/metaps/lib/odalib/0.9.1/include"
export ODB_CXX_LIB="-L/usr/local/lib/metaps/lib/odalib/0.9.1/lib -lEc -lOdb"
xlC_r  -qsmp=omp -o test_omp test_omp.cc $ODB_CXX_INCLUDE $ODB_CXX_LIB 

