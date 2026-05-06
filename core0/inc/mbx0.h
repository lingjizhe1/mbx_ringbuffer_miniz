#ifndef MBX0_H
#define MBX0_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void mbx0_init(void);
int8_t mbx0_send_bytes_blocking(const uint8_t *data, uint32_t length);
int8_t mbx0_send_cstr_blocking(const char *str);

#ifdef __cplusplus
}
#endif

#endif /* MBX0_H */
