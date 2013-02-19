#!/usr/bin/awk -f

function printConfig(C) {
	print "/usr/local/apps/cmake/2.8.8/bin/cmake ../.. \\"
	for (k in C) print "  -D" k "=\"" C[k] "\" \\"
	print ""
}

function parse(r, s) {
	split(s, lines, "\n")
	for (i in lines) {
		n = split(lines[i], fields, "=")
		if (n == 2) {
			r[fields[1]] = fields[2]
		} else if (lines[i]) {
			print "*** Could not parse '" lines[i] "'"
		}
	}
}

BEGIN {

if (! linking) linking="static"
if (! host) host="opensuse113"

parse(r, "\
SRC=/tmp/p4/source/main\n\
CMAKE_BUILD_TYPE=production\n\
ODB_PATH=$ODB_ROOT\n\
CMAKE_PREFIX_PATH=/usr/local/apps/python/2.7\n\
CMAKE_C_COMPILER=gcc\n\
CMAKE_CXX_COMPILER=g++\n\
BUILD_SHARED_LIBS=ON\n\
CMAKE_INSTALL_PREFIX=/tmp/mak/odb_api/\n\
CMAKE_Fortran_COMPILER=/usr/local/apps/pgi/pgi-10.8/linux86-64/10.8/bin/pgf90\n\
ODB_API_MIGRATOR=ON\n\
ODB_API_PYTHON=ON \n\
CMAKE_CXX_FLAGS=-fPIC\n\
CMAKE_C_FLAGS=-fPIC\n\
ECLIB_SOURCE=$SRC/eclib\n\
BUILD_SHARED_LIBS=OFF\n\
CMAKE_MODULE_PATH=/usr/local/apps/ecbuild/1.0.10/cmake\n\
ODB_API_FORTRAN=ON\n\
LINKER_LANGUAGE=Fortran\n\
PGI_PATH=/usr/local/apps/pgi/pgi-10.8/linux86-64/current\n\
")

L["static"]="BUILD_SHARED_LIBS=OFF"
L["shared"]="BUILD_SHARED_LIBS=ON"

H["lxa"]="LINKER_LANGUAGE=Fortran"
H["c2a"]="\
LINKER_LANGUAGE=CXX\n\
XLF_PATH=/usr/local/lpp/xlf13109/usr\n\
CMAKE_Fortran_COMPILER=xlf90_r\n\
CMAKE_CXX_COMPILER=xlC_r\n\
CMAKE_C_COMPILER=xlc_r\n\
SWIG_EXECUTABLE=/usr/local/apps/swig/2.0.8/LP64/bin/swig\n\
BISON_EXECUTABLE=/usr/local/apps/bison/2.3/bin/bison\n\
FLEX_EXECUTABLE=/usr/bin/flex"
H["c2b"]=H["c2a"]
H["ecgate"]="\
BISON_EXECUTABLE=/usr/local/apps/bison/current/bin/bison\n\
"

parse(r, L[linking])
parse(r, H[host])
printConfig(r)

}
