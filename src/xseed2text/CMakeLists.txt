SET(SRCS main.c)

ADD_EXECUTABLE(xseed2text ${SRCS})
TARGET_LINK_LIBRARIES(xseed2text xseed-common)

