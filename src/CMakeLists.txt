#Build common code for all xseed-utils
init_target_source_dir(xseed-common)
ADD_SUBDIRECTORY(xseed-common)

INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}")
ADD_LIBRARY(xseed-common STATIC ${xseed-common_SRCS})
target_link_libraries(xseed-common ${MSEED_LIBRARIES})
SET_PROPERTY(TARGET xseed-common PROPERTY C_STANDARD 99)

ADD_SUBDIRECTORY(mseed2xseed)
ADD_SUBDIRECTORY(xseed2json)
ADD_SUBDIRECTORY(xseed2text)
ADD_SUBDIRECTORY(xseedValidator)
