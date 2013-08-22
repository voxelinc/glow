
# HIDAPI_FOUND
# HIDAPI_INCLUDE_DIR
# HIDAPI_LIBRARY

FIND_PATH(HIDAPI_INCLUDE_DIR hidapi.h
    $ENV{HIDAPI_HOME}/include
    $ENV{HIDAPIDIR}/include
    /usr/include
    /usr/local/include
    /sw/include
    /opt/local/include
    DOC "The directory where hidapi.h resides.")
    
FIND_LIBRARY(HIDAPI_LIBRARY
    NAMES hidapi
    PATHS
    $ENV{HIDAPI_DIR}/lib
    $ENV{HIDAPI_HOME}/lib
    /usr/lib64
    /usr/local/lib64
    /sw/lib64
    /opt/local/lib64
    DOC "The HDIAPI library")


IF(HIDAPI_INCLUDE_DIR)
  SET(HIDAPI_FOUND 1 CACHE STRING "Set to 1 if HIDAPI is found, 0 otherwise")
ELSE()
  SET(HIDAPI_FOUND 0 CACHE STRING "Set to 1 if HIDAPI is found, 0 otherwise")
  MESSAGE(WARNING "Note: an envvar HIDAPI_HOME assists this script to locate glm.")
ENDIF()

MARK_AS_ADVANCED(HIDAPI_FOUND)
