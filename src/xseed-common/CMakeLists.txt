CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.h)

add_sources(xseed-common display_help.c generate_getop_options.c
    expand_array.c file_exists.c file_length.c)
