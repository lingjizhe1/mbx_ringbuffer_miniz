#include <string.h>

#include "board.h" /* IWYU pragma: keep */
#include "hpm_mbx_drv.h"
#include "../inc/mbx0.h"

#define APP_LOG_TAG "MBX0"
#include "../../src/log.h"

static volatile bool can_send;

#define MBX HPM_MBX0A
#define MBX_IRQ IRQn_MBX0A

SDK_DECLARE_EXT_ISR_M(MBX_IRQ, isr_mbx)
void isr_mbx(void)
{
    volatile uint32_t sr = MBX->SR;
    volatile uint32_t cr = MBX->CR;
    if ((sr & MBX_SR_TWME_MASK) && (cr & MBX_CR_TWMEIE_MASK)) {
        mbx_disable_intr(MBX, MBX_CR_TWMEIE_MASK);
        can_send = true;
    } 
    if ((sr & MBX_SR_TFMA_MASK) && (cr & MBX_CR_TFMAIE_MASK)) {
        mbx_disable_intr(MBX, MBX_CR_TFMAIE_MASK);
        can_send = true;
    } 
}

void mbx0_init(void)
{
    can_send = false;
    mbx_init(MBX);
}

int8_t mbx0_send_bytes_blocking(const uint8_t *data, uint32_t length)
{
    hpm_stat_t status;
    uint32_t msg[4];
    uint32_t tx_slot_count;
    uint32_t to_be_sent;
    uint32_t slot_total;
    uint32_t slot_index = 0;
    uint32_t offset = 0;

    if ((data == NULL) || (length == 0)) {
        LOG_DEBUG("invalid send args\n");
        return -1;
    }

    slot_total = (length + 2U) / 3U;

    while (slot_index < slot_total) {
        tx_slot_count = (MBX->SR & MBX_SR_TFEC_MASK) >> MBX_SR_TFEC_SHIFT;
        if (tx_slot_count == 0U) {
            can_send = false;
            mbx_enable_intr(MBX, MBX_CR_TFMAIE_MASK);
            while (!can_send) {
            }
            continue;
        }

        to_be_sent = slot_total - slot_index;
        if (to_be_sent > tx_slot_count) {
            to_be_sent = tx_slot_count;
        }
        if (to_be_sent > ARRAY_SIZE(msg)) {
            to_be_sent = ARRAY_SIZE(msg);
        }

        for (uint32_t i = 0; i < to_be_sent; i++) {
            bool is_last_word = (slot_index == (slot_total - 1U));
            uint8_t valid_bytes = is_last_word ? (uint8_t)(length - offset) : 3U;
            uint8_t marker = is_last_word ? valid_bytes : 4U;

            if (valid_bytes > 3U) {
                valid_bytes = 3U;
            }

            msg[i] = (uint32_t)marker << 24;
            for (uint8_t j = 0; j < valid_bytes; j++) {
                msg[i] |= (uint32_t)data[offset + j] << (8U * j);
            }

            offset += valid_bytes;
            slot_index++;
        }

        status = mbx_send_fifo(MBX, msg, to_be_sent);
        if (status != status_success) {
            LOG_DEBUG("send fifo failed: %d\n", status);
            return -1;
        }
    }

    LOG_DEBUG("sent %lu bytes\n", (unsigned long)length);
    return 0;
}

int8_t mbx0_send_cstr_blocking(const char *str)
{
    if (str == NULL) {
        LOG_DEBUG("invalid cstr arg\n");
        return -1;
    }

    return mbx0_send_bytes_blocking((const uint8_t *)str, (uint32_t)strlen(str) + 1U);
}
