### libs

# these should be the only directories included
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

add_subdirectory( odblib  )

add_subdirectory( fortran )

### apps

add_subdirectory( odb )

add_subdirectory( migrator )
