#include <stdio.h>
#include <string.h>
#include "board.h"

void delay_us(uint32_t us)
{
    uint32_t start = hpm_csr_get_core_cycle();
    while (hpm_csr_get_core_cycle() - start < us) {
        __asm__("nop");
    }
}

void delay_ms(uint32_t ms)
{
    uint32_t start = hpm_csr_get_core_cycle();
    while (hpm_csr_get_core_cycle() - start < ms * 1000) {
        __asm__("nop");
    }
}