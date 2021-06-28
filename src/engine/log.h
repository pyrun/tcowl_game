#ifndef ENGINE_LOG_H
#define ENGINE_LOG_H

#include <stdint.h>

#define ENGINE_LOG_MAX_LENGTH 512 //bytes

namespace engine {
    enum log_level : uint32_t {
        log_trace = 0,
        log_debug,
        log_info,
        log_warn,
        log_error,
        log_fatal,
    };
    
    void log( log_level level, const char *fmt, ...);
};

#endif