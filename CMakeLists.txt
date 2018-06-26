PROJECT(xseed-utils)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

IF (UNIX)
    INCLUDE(GNUInstallDirs)
ENDIF(UNIX)
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

#TODO: check for thirdparty libraries (tinycbor)

# Some users Might want to build xseed as a static library
SET(XSEED_BUILD_SHARED ON CACHE BOOL "Build libxseed as a shared library
vs static")
# Tells ADD_LIBRARY to default to static or shared depending on
# XSEED_BUILD_SHARED
OPTION(BUILD_SHARED_LIBS "Build the shared library" ${XSEED_BUILD_SHARED})

FIND_PACKAGE(MSEED 3.0)
INCLUDE(CheckFunctionExists)
CHECK_FUNCTION_EXISTS(strnlen HAS_STRNLEN)
CHECK_FUNCTION_EXISTS(strndup HAS_STRNDUP)

IF (MSVC)
    MESSAGE(STATUS "Building for Visual Studio")
ENDIF(MSVC)

ADD_SUBDIRECTORY(libsrc)

INCLUDE(AddSources)
#build sources
ADD_SUBDIRECTORY(src)

#TODO: add stuff for documentation generation
#ADD_SUBDIRECTORY(doc)

# Integration testing is in top level test directory
SET(XSEED_TEST OFF CACHE BOOL "Enable integrated testing for libxseed")
IF(XSEED_TEST)
    ENABLE_TESTING()
#TODO: add integration tests
#    ADD_SUBDIRECTORY(test)
ENDIF(XSEED_TEST)

