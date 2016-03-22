ecbuild_add_executable(TARGET odb2_to_odb1.x
    INCLUDES ${ODB_INCLUDE_DIRS}
    SOURCES
        Odb2Odb1.cc
        Odb2Odb1.h
        Odb2Odb1Main.cc
        mpi_wrapper.F90
        mpif.h
        odb_wrapper.F90
        odbi.F90
    LIBS Odb_fortran Odb eckit ${ODB_LIBRARIES}
    CONDITION HAVE_FORTRAN AND ODB_FOUND)

if(HAVE_FORTRAN AND ODB_FOUND)
    include(odb_link_schemas)
    odb_link_schemas(odb2_to_odb1.x ${ODB_SCHEMAS})
endif()
