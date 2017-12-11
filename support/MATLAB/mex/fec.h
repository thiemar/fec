#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void rs_encode(const uint8_t *data, uint8_t *buf);
size_t conv_get_encoded_len(size_t len);
size_t conv_get_decoded_len(size_t len);
void conv_encode(const uint8_t *data, size_t len, uint8_t *buf);
void conv_decode(const uint8_t *data, size_t len, uint8_t *buf);
void polar_encode(const uint8_t *data, uint8_t *buf);

#ifdef __cplusplus
}
#endif
