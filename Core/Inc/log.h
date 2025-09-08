#ifndef LOG_H
#define LOG_H

#include <stdio.h>

// 日志开关
#define LOG_ENABLE      0    

// 日志等级
#define LOG_LEVEL_INFO  1
#define LOG_LEVEL_WARN  1
#define LOG_LEVEL_ERROR 1

#if LOG_ENABLE

    #define LOG_INFO(fmt, ...)  do { if(LOG_LEVEL_INFO)  printf("[INFO] " fmt "\r\n", ##__VA_ARGS__); } while(0)
    #define LOG_WARN(fmt, ...)  do { if(LOG_LEVEL_WARN)  printf("[WARN] " fmt "\r\n", ##__VA_ARGS__); } while(0)
    #define LOG_ERROR(fmt, ...) do { if(LOG_LEVEL_ERROR) printf("[ERR]  " fmt "\r\n", ##__VA_ARGS__); } while(0)

#else

    #define LOG_INFO(fmt, ...)
    #define LOG_WARN(fmt, ...)
    #define LOG_ERROR(fmt, ...)

#endif

#endif
