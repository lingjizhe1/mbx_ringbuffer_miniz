# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(mbx_example_core0)

sdk_app_inc(inc)
sdk_app_inc(../src)
sdk_app_inc(../../common)


set(APP_LOG_LEVEL "APP_LOG_LEVEL_INFO" CACHE STRING "Application log level macro for APP_LOG_LEVEL")
sdk_compile_definitions(-DAPP_LOG_LEVEL=${APP_LOG_LEVEL})

sdk_app_src(src/main0.c)
sdk_app_src(src/mbx0.c)
sdk_app_src(../../common/multicore_common.c)
sdk_app_src(../src/sec_core_img.c)


generate_ide_projects()
