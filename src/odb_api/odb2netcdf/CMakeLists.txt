list( APPEND odb2netcdf_src_files
            Odb2NetCDF.cc 
            Odb2NetCDF.h 
            Odb2NetcdfModule.h
            Odb2NetcdfModule.cc
            ecml_verbs/Odb2NetcdfHandler.cc
            ecml_verbs/Odb2NetcdfHandler.h
)

ecbuild_add_executable(TARGET odb2netcdf.x
    SOURCES 
            odb2netcdf_main.cc
            ${odb2netcdf_src_files}

    INCLUDES 
            ${NETCDF_INCLUDE_DIRS} 
            ${ODB_API_INCLUDE_DIRS}
            ${ECKIT_INCLUDE_DIRS}

    LIBS    
            ${NETCDF_LIBRARIES} 
            ${ODB_API_LIBRARIES}
            ${ECKIT_LIBRARIES}

    CONDITION HAVE_NETCDF)

ecbuild_add_library( TARGET     Odb2Netcdf
                     #INSTALL_HEADERS LISTED
                     #HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/odb_api
                     #COMPONENT  server
                     SOURCES    ${odb2netcdf_src_files}
                     PRIVATE_INCLUDES ${NETCDF_INCLUDE_DIRS}
                     LIBS       Odb odbtools ${NETCDF_LIBRARIES}
                     CONDITION HAVE_NETCDF)

