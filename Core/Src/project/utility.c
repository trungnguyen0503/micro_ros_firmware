#include "project/utility.h"

#include "cmsis_os2.h"
#include "usart.h"

#include <inttypes.h>
#include <stdarg.h>

#define LOG_BUF_SIZE 1024

static uint8_t g_log_buffer[LOG_BUF_SIZE] = { 0 };

void Utility_Printf(const char *const fmt, ...) {
    va_list va;
    va_start(va, fmt);
    const int len = vsnprintf((char *)g_log_buffer, LOG_BUF_SIZE, fmt, va);
    if (len > 0) {
        // HAL_UART_Transmit(&huart1, g_log_buffer, (uint16_t)len, 100);
    }
    va_end(va);
}

void Utility_Log(enum Utility_LogLevel level, const char *fmt, ...) {
    char level_char = 'U';
    switch (level) {
        case Utility_LogInfo:
            level_char = 'I';
            break;
        case Utility_LogWarning:
            level_char = 'W';
            break;
        case Utility_LogError:
            level_char = 'E';
            break;
        case Utility_LogFatal:
            level_char = 'F';
            break;
        default:
            break;
    }
    const int header_len = snprintf(
        (char *)g_log_buffer, LOG_BUF_SIZE, "[%10" PRIu32 "][%c] ",
        osKernelGetTickCount(), level_char
    );
    va_list va = { 0 };
    va_start(va, fmt);
    const int body_len = vsnprintf(
        (char *)g_log_buffer + header_len, LOG_BUF_SIZE - (size_t)header_len,
        fmt, va
    );
    if (body_len >= 0) {
        int total_len = header_len + body_len;
        g_log_buffer[total_len++] = '\n';
        // HAL_UART_Transmit(&huart1, g_log_buffer, (uint16_t)total_len, 100);
    }
    va_end(va);
}
