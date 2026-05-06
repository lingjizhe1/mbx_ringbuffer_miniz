#include <string.h>

#include "board.h" /* IWYU pragma: keep */
#include "hpm_mbx_drv.h"
#include "../inc/mbx1.h"

#define APP_LOG_TAG "MBX1"
#include "log.h"

static volatile bool can_read = false;

#define MBX HPM_MBX0B
#define MBX_IRQ IRQn_MBX0B

#ifndef MBX1_RX_ASSEMBLY_BUFFER_SIZE
#define MBX1_RX_ASSEMBLY_BUFFER_SIZE 256U
#endif

#ifndef MBX1_PACKET_BUFFER_SIZE
#define MBX1_PACKET_BUFFER_SIZE 256U
#endif

static uint8_t rx_buffer[MBX1_RX_ASSEMBLY_BUFFER_SIZE];
static uint8_t packet_buffer[MBX1_PACKET_BUFFER_SIZE];
static uint32_t rx_length;
static uint32_t packet_length;
static bool packet_ready;

static void mbx1_reset_rx_packet(void)
{
    rx_length = 0;
}

static void mbx1_commit_packet(void)
{
    if (packet_ready) {
        LOG_DEBUG("drop packet: previous packet not consumed\n");
        return;
    }

    if (rx_length > MBX1_PACKET_BUFFER_SIZE) {
        LOG_DEBUG("drop packet: too large\n");
        return;
    }

    memcpy(packet_buffer, rx_buffer, rx_length);
    packet_length = rx_length;
    packet_ready = true;
    LOG_DEBUG("packet ready: %lu bytes\n", (unsigned long)packet_length);
}

static void mbx1_unpack_word(uint32_t word)
{
    uint8_t marker = (uint8_t)(word >> 24);
    uint8_t valid_bytes;

    if (marker == 4U) {
        valid_bytes = 3U;
    } else if ((marker >= 1U) && (marker <= 3U)) {
        valid_bytes = marker;
    } else {
        LOG_DEBUG("drop packet: invalid marker %u\n", marker);
        mbx1_reset_rx_packet();
        return;
    }

    if ((rx_length + valid_bytes) > MBX1_RX_ASSEMBLY_BUFFER_SIZE) {
        LOG_DEBUG("drop packet: assembly buffer overflow\n");
        mbx1_reset_rx_packet();
        return;
    }

    for (uint8_t i = 0; i < valid_bytes; i++) {
        rx_buffer[rx_length++] = (uint8_t)(word >> (8U * i));
    }

    if (marker != 4U) {
        mbx1_commit_packet();
        mbx1_reset_rx_packet();
    }
}

SDK_DECLARE_EXT_ISR_M(MBX_IRQ, isr_mbx)
void isr_mbx(void)
{
    volatile uint32_t sr = MBX->SR;
    volatile uint32_t cr = MBX->CR;
    if ((sr & MBX_SR_RWMV_MASK) && (cr & MBX_CR_RWMVIE_MASK)) {
        mbx_disable_intr(MBX, MBX_CR_RWMVIE_MASK);
        can_read = true;
    } 
    if ((sr & MBX_SR_RFMA_MASK) && (cr & MBX_CR_RFMAIE_MASK)) {
        mbx_disable_intr(MBX, MBX_CR_RFMAIE_MASK);
        can_read = true;
    } 
}

void mbx1_init(void)
{
    mbx_init(MBX);
    mbx1_reset_rx_packet();
    packet_length = 0;
    packet_ready = false;
    can_read = false;
    mbx_enable_intr(MBX, MBX_CR_RFMAIE_MASK);
}

void mbx1_process(void)
{
    uint32_t msg[4];
    uint32_t rx_msg_count;
    hpm_stat_t status;

    if (!can_read) {
        return;
    }

    do {
        rx_msg_count = (MBX->SR & MBX_SR_RFVC_MASK) >> MBX_SR_RFVC_SHIFT;
        if (rx_msg_count > ARRAY_SIZE(msg)) {
            rx_msg_count = ARRAY_SIZE(msg);
        }
        if (rx_msg_count == 0U) {
            break;
        }

        status = mbx_retrieve_fifo(MBX, msg, rx_msg_count);
        if (status == status_success) {
            for (uint32_t i = 0; i < rx_msg_count; i++) {
                mbx1_unpack_word(msg[i]);
            }
        } else {
            LOG_DEBUG("retrieve fifo failed: %d\n", status);
            mbx1_reset_rx_packet();
            break;
        }
    } while (rx_msg_count > 0U);

    can_read = false;
    mbx_enable_intr(MBX, MBX_CR_RFMAIE_MASK);
}

bool mbx1_packet_available(void)
{
    return packet_ready;
}

uint32_t mbx1_read_packet(uint8_t *out, uint32_t out_size)
{
    uint32_t copy_length;

    if (!packet_ready || (out == NULL) || (out_size == 0U)) {
        return 0;
    }

    copy_length = packet_length;
    if (copy_length > out_size) {
        copy_length = out_size;
        LOG_DEBUG("packet truncated by user buffer\n");
    }

    memcpy(out, packet_buffer, copy_length);
    packet_ready = false;
    packet_length = 0;

    return copy_length;
}