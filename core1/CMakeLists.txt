# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(HPM_BUILD_TYPE "sec_core_img")
set(SEC_CORE_IMG_C_ARRAY_OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/../src/sec_core_img.c)
set(CONFIG_SDMMC 1)
set(CONFIG_FATFS 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(mbx_example_core1)


set(APP_LOG_LEVEL "APP_LOG_LEVEL_INFO" CACHE STRING "Application log level macro for APP_LOG_LEVEL")
sdk_compile_definitions(-DAPP_LOG_LEVEL=${APP_LOG_LEVEL})
sdk_compile_definitions(-DSD_FATFS_ENABLE=1)
sdk_compile_definitions(-DFF_CODE_PAGE=437)

sdk_compile_definitions(-DMINIZ_NO_STDIO
    -DMINIZ_NO_TIME
    -DMINIZ_NO_ARCHIVE_APIS
    -DMINIZ_NO_ARCHIVE_WRITING_APIS
    -DMINIZ_NO_MALLOC=1
    -DTDEFL_LESS_MEMORY=1
    -DMINIZ_USE_UNALIGNED_LOADS_AND_STORES=0)
sdk_app_inc(inc)
sdk_app_inc(../src)
sdk_inc(../third_party/inc)
sdk_app_src(src/main1.c)
sdk_app_src(src/mbx1.c)
sdk_app_src(src/compress.c)
sdk_app_src(../third_party/src/miniz_tdef.c)
sdk_app_src(../third_party/src/miniz_tinfl.c)
sdk_app_src(../third_party/src/miniz.c)
sdk_app_src(src/sd_fatfs.c)
generate_ide_projects()
