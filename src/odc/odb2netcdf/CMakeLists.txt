list( APPEND odb2netcdf_src_files
            Odb2NetCDF.cc 
            Odb2NetCDF.h 
)

ecbuild_add_executable(TARGET odb2netcdf.x
    SOURCES
            odb2netcdf_main.cc
            ${odb2netcdf_src_files}

    INCLUDES
            ${NETCDF_INCLUDE_DIRS}
            ${odc_INCLUDE_DIRS}
            ${ECKIT_INCLUDE_DIRS}

    LIBS
            ${NETCDF_LIBRARIES}
            ${odc_LIBRARIES}
            ${ECKIT_LIBRARIES}

    CONDITION HAVE_NETCDF)

ecbuild_add_library( TARGET     Odb2Netcdf
                     #INSTALL_HEADERS LISTED
                     #HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/odc
                     #COMPONENT  server
                     SOURCES    ${odb2netcdf_src_files}
                     PRIVATE_INCLUDES ${NETCDF_INCLUDE_DIRS}
                     LIBS       odccore odctools ${NETCDF_LIBRARIES}
                     CONDITION HAVE_NETCDF)

