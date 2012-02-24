message( STATUS "---------------------------------------------------------" )

message( STATUS " MARS_TAPE_MANAGERS : [${MARS_TAPE_MANAGERS}]" )

if(HPSS_FOUND)
  message( STATUS " HPSS include     : [${HPSS_INCLUDE_DIRS}]" )
  message( STATUS "      libs        : [${HPSS_LIBRARIES}]" )
endif()

if(ADSM_FOUND)
  message( STATUS " ADSM include     : [${ADSM_INCLUDE_DIRS}]" )
  message( STATUS "      libs        : [${ADSM_LIBRARIES}]" )
endif()

