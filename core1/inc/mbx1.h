#ifndef MBX1_H
#define MBX1_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void mbx1_init(void);
void mbx1_process(void);
bool mbx1_packet_available(void);
uint32_t mbx1_read_packet(uint8_t *out, uint32_t out_size);

#ifdef __cplusplus
}
#endif

#endif /* MBX1_H */
