SET(SRCS main.c parse_warn_options.c check_file.c check_header.c
    check_extra_headers.c check_payloads.c check_payload_text.c)

ADD_EXECUTABLE(xseedValidator ${SRCS})
TARGET_LINK_LIBRARIES(xseedValidator xseed-common)

