#include "log.h"
#include <stdarg.h> /* va_end(), va_list, va_start(), vprintf() */
#include <stdio.h> /* vprintf() */

using namespace engine;

void engine::log( log_level level, const char *fmt, ...) {
    static char l_log_buffer[ENGINE_LOG_MAX_LENGTH];
    uint32_t l_length;
    va_list ap;
    va_start(ap, fmt);
    l_length = vsnprintf( l_log_buffer, ENGINE_LOG_MAX_LENGTH, fmt, ap);
    va_end(ap);

    printf( "[LOG|%d|%.*s]\n", (uint32_t)level, l_length, l_log_buffer);
}