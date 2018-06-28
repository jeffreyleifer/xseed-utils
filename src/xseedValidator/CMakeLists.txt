add_sources(xseedValidator main.c parse_warn_options.c check_file.c
    check_header.c check_extra_headers.c check_payloads.c
    check_payload_text.c check_identifier.c)

ADD_EXECUTABLE(xseedValidator ${xseedValidator_SRCS})
TARGET_LINK_LIBRARIES(xseedValidator xseed-common)

