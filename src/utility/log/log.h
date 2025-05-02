#ifndef LOG_H
#define LOG_H

#include <stdio.h>

#define EZ_LOG_FATAL(message, ...)                                     \
    fprintf(stderr, "[EzWF FATAL] %s%s", message, "\n", ##__VA_ARGS__)
#define EZ_LOG_WARN(message, ...)                             \
    fprintf(stderr, "[EzWF WARN] %s", message, ##__VA_ARGS__)

#if EZ_LOG_DEBUG_ENABLED == 1
#define EZ_LOG_DEBUD(message, ...)                             \
    fprintf(stderr, "[EzWF DEBUG] %s", message, ##__VA_ARGS__)
#else
#define EZ_LOG_DEBUG(message, ...)
#endif

#endif // LOG_H
