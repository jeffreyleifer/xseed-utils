# - Find the native MSEED includes and library
#

# This module defines
#  MSEED_INCLUDE_DIR, where to find libmseed.h, etc.
#  MSEED_LIBRARIES, the libraries to link against to use libmseed.
#  MSEED_DEFINITIONS, You should ADD_DEFINITONS(${MSEED_DEFINITIONS}) before compiling code that includes libmseed library files.
#  MSEED_FOUND, If false, do not try to use fftw3.
#  MSEED_VERSION, The found libmseed version

#valid for cmake 2.8.3

INCLUDE(CheckFunctionExists)

SET(MSEED_ROOT /usr/ CACHE PATH "MSeed location")
FIND_PATH(MSEED_INCLUDE_DIR 
    NAMES libmseed.h
    PATHS ${MSEED_ROOT}
    PATH_SUFFIXES  include)

SET(MSEED_NAMES ${MSEED_NAMES} mseed)
FIND_LIBRARY(MSEED_LIBRARY
    NAMES ${MSEED_NAMES}
    PATHS ${MSEED_ROOT}
    PATH_SUFFIXES lib)

IF (MSEED_LIBRARY AND MSEED_INCLUDE_DIR)
    SET(CMAKE_REQUIRED_INCLUDES ${MSEED_INCLUDE_DIR})

    #Get version information from libmseed
    FILE(STRINGS "${MSEED_INCLUDE_DIR}/libmseed.h" TEST_VERSION
        REGEX "^[ \t]*#define[ \t\n]+LIBMSEED_VERSION"
        #REGEX "^[ \t]*#define[ \t\n]+LIBMSEED[ \t]+(.*)$"
        )
    STRING(REGEX MATCH "[0-9]+(\\.[0-9]+(\\.[0-9]+)?)?" MSEED_VERSION "${TEST_VERSION}")

ENDIF (MSEED_LIBRARY AND MSEED_INCLUDE_DIR)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MSEED FOUND_VAR MSEED_FOUND
    REQUIRED_VARS MSEED_LIBRARY MSEED_INCLUDE_DIR
    VERSION_VAR MSEED_VERSION)


MARK_AS_ADVANCED(MSEED_LIBRARY MSEED_INCLUDE_DIR)
IF (MSEED_FOUND)
    SET(MSEED_LIBRARIES ${MSEED_LIBRARY} "-lm")
    SET(MSEED_INCLUDE_DIRS ${MSEED_INCLUDE_DIR})
    MARK_AS_ADVANCED(MSEED_ROOT)
ENDIF (MSEED_FOUND)
