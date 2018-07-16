SET(SRCS main.c)

ADD_EXECUTABLE(xseed2json ${SRCS})
TARGET_LINK_LIBRARIES(xseed2json xseed-common)

