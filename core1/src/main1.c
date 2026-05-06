/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "../inc/mbx1.h"
#include "compress.h"
#include "sd_fatfs.h"
#define APP_LOG_TAG "CORE1"
#include "../../src/log.h"


#define LED_FLASH_PERIOD_IN_MS 300
#define MBX_IRQ IRQn_MBX0B
#define CORE1_PACKET_BUFFER_SIZE 256U

int main(void)
{
    uint8_t packet[CORE1_PACKET_BUFFER_SIZE];
    uint32_t packet_length;

    board_init_core1();
    board_init_led_pins();

    board_timer_create(LED_FLASH_PERIOD_IN_MS, board_led_toggle);
    intc_m_enable_irq_with_priority(MBX_IRQ, 1);
    mbx1_init();
    LOG_DEBUG("core1 consumer init done\n");
    miniz_demo();
    sd_fatfs_init();
    while (1) {
        mbx1_process();
        if (mbx1_packet_available()) {
            packet_length = mbx1_read_packet(packet, sizeof(packet));
            if (packet_length > 0U) {
                LOG_INFO("core1 consumed %lu bytes\n", (unsigned long)packet_length);
                for (uint32_t i = 0; i < packet_length; i++) {
                    printf("%c", packet[i]);
                }
                printf("\n");
            }
        }
    }
}
