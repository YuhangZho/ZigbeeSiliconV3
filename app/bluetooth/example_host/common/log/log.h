#ifndef LOG_H
#define LOG_H

#include "stdio.h"
#include "log_config.h"

#if defined(LOG_ENABLE) && LOG_ENABLE
#define app_log(...)    printf(__VA_ARGS__)
#else // LOG_ENABLE
#define app_log(...)
#endif // LOG_ENABLE

#endif // LOG_H
