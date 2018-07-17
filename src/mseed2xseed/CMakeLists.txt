SET(SRCS main.c)

ADD_EXECUTABLE(mseed2xseed ${SRCS})
TARGET_LINK_LIBRARIES(mseed2xseed xseed-common)
