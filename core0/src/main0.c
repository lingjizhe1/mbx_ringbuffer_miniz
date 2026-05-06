/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_clock_drv.h"
#include "multicore_common.h"

#include "../inc/mbx0.h"

#define APP_LOG_TAG "CORE0"
#include "log.h"
#include "delay.h"

int main(void)
{
    board_init();
    clock_add_to_group(clock_mbx0, 0);

    intc_m_enable_irq_with_priority(IRQn_MBX0A, 1);
    multicore_release_cpu(HPM_CORE1, SEC_CORE_IMG_START);

    board_delay_ms(1000);
    mbx0_init();

    LOG_DEBUG("core0 producer init done\n");
    (void)mbx0_send_cstr_blocking("hello from core0");

    while (1) {

    }
    return 0;
}
