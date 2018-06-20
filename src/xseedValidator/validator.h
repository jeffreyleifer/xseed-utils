#ifndef __XSEEDVALIDATOR_VALIDATOR_H__
#define __XSEEDVALIDATOR_VALIDATOR_H__
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

bool check_file (FILE *input);
bool check_header(FILE *input_file, long file_len, long *file_pos, uint16_t *extra_header_len, uint32_t *payload_len, uint8_t *payload_fmt);
bool check_extra_headers(FILE *input, uint16_t extra_header_len);
bool check_payloads(FILE *input, uint32_t payload_len, uint8_t payload_fmt);
bool check_payload_text(uint32_t payload_len, char *buffer);

#endif /* __XSEEDVALIDATOR_VALIDATOR_H__ */