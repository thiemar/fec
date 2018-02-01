#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void rs_encode(const uint8_t *data, uint8_t *buf);
size_t conv_get_encoded_len();
void conv_encode(const uint8_t *data, uint8_t *buf);
void conv_decode(const uint8_t *data, uint8_t *buf);
void polar_encode(const uint8_t *data, uint8_t *buf);
void polar_decode(const uint8_t *data, uint8_t *buf);

#ifdef __cplusplus
}
#endif
