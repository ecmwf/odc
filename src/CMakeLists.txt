### libs

include_directories( odblib ${CMAKE_CURRENT_BINARY_DIR}/odblib ) # needed for yacc & flex
add_subdirectory( odblib  )

add_subdirectory( fortran )

### apps

add_subdirectory( odb )

# needed due to inclusions to odb code
# remove this once headers include their directories
include_directories( odb ) 

add_subdirectory( migrator )
