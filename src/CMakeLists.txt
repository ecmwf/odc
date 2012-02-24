### libs

include_directories( odblib ${CMAKE_CURRENT_BINARY_DIR}/odblib ) # needed for yacc & flex
add_subdirectory( odblib  )

add_subdirectory( fortran )

### apps

add_subdirectory( odb )

add_subdirectory( odbmigrator )
