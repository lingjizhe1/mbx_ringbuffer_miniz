#ifndef APP_LOG_H
#define APP_LOG_H

/*
 * Lightweight configurable logging helper.
 *
 * CMake examples:
 *   target_compile_definitions(${APP_ELF_NAME} PRIVATE APP_LOG_LEVEL=APP_LOG_LEVEL_DEBUG)
 *   target_compile_definitions(${APP_ELF_NAME} PRIVATE APP_LOG_USE_COLOR=0)
 *   target_compile_definitions(${APP_ELF_NAME} PRIVATE APP_LOG_OUTPUT=my_usb_printf)
 */

#ifndef APP_LOG_OUTPUT
#include <stdio.h> /* IWYU pragma: keep */
#define APP_LOG_OUTPUT(...) printf(__VA_ARGS__)
#endif

#define APP_LOG_LEVEL_NONE   0
#define APP_LOG_LEVEL_ERROR  1
#define APP_LOG_LEVEL_WARN   2
#define APP_LOG_LEVEL_INFO   3
#define APP_LOG_LEVEL_DEBUG  4
#define APP_LOG_LEVEL_TRACE  5

#ifndef APP_LOG_LEVEL
#define APP_LOG_LEVEL APP_LOG_LEVEL_INFO
#endif

#ifndef APP_LOG_USE_COLOR
#define APP_LOG_USE_COLOR 1
#endif

#ifndef APP_LOG_TAG
#define APP_LOG_TAG "APP"
#endif

#if APP_LOG_USE_COLOR
#define APP_LOG_COLOR_ERROR "\x1b[31m"
#define APP_LOG_COLOR_WARN  "\x1b[33m"
#define APP_LOG_COLOR_INFO  "\x1b[32m"
#define APP_LOG_COLOR_DEBUG "\x1b[36m"
#define APP_LOG_COLOR_TRACE "\x1b[90m"
#define APP_LOG_COLOR_RESET "\x1b[0m"
#else
#define APP_LOG_COLOR_ERROR ""
#define APP_LOG_COLOR_WARN  ""
#define APP_LOG_COLOR_INFO  ""
#define APP_LOG_COLOR_DEBUG ""
#define APP_LOG_COLOR_TRACE ""
#define APP_LOG_COLOR_RESET ""
#endif

#define APP_LOG_PRINT(color, level, fmt, ...)                                      \
    do {                                                                           \
        APP_LOG_OUTPUT("%s[%s][%s] " fmt "%s", color, APP_LOG_TAG, level,          \
                       ##__VA_ARGS__, APP_LOG_COLOR_RESET);                        \
    } while (0)

#if APP_LOG_LEVEL >= APP_LOG_LEVEL_ERROR
#define LOG_ERROR(fmt, ...) APP_LOG_PRINT(APP_LOG_COLOR_ERROR, "E", fmt, ##__VA_ARGS__)
#else
#define LOG_ERROR(fmt, ...) ((void)0)
#endif

#if APP_LOG_LEVEL >= APP_LOG_LEVEL_WARN
#define LOG_WARN(fmt, ...) APP_LOG_PRINT(APP_LOG_COLOR_WARN, "W", fmt, ##__VA_ARGS__)
#else
#define LOG_WARN(fmt, ...) ((void)0)
#endif

#if APP_LOG_LEVEL >= APP_LOG_LEVEL_INFO
#define LOG_INFO(fmt, ...) APP_LOG_PRINT(APP_LOG_COLOR_INFO, "I", fmt, ##__VA_ARGS__)
#else
#define LOG_INFO(fmt, ...) ((void)0)
#endif

#if APP_LOG_LEVEL >= APP_LOG_LEVEL_DEBUG
#define LOG_DEBUG(fmt, ...) APP_LOG_PRINT(APP_LOG_COLOR_DEBUG, "D", fmt, ##__VA_ARGS__)
#else
#define LOG_DEBUG(fmt, ...) ((void)0)
#endif

#if APP_LOG_LEVEL >= APP_LOG_LEVEL_TRACE
#define LOG_TRACE(fmt, ...) APP_LOG_PRINT(APP_LOG_COLOR_TRACE, "T", fmt, ##__VA_ARGS__)
#else
#define LOG_TRACE(fmt, ...) ((void)0)
#endif

#endif /* APP_LOG_H */
