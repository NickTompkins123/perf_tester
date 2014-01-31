#ifndef LOG_H
#define LOG_H

#ifdef ANDROID
#include <android/log.h>
#define LOG_ERROR(x) __android_log_write(ANDROID_LOG_ERROR, "df", x) 
#define LOG_INFO(x) __android_log_write(ANDROID_LOG_INFO, "df", x) 
#else
#define LOG_ERROR(x)
#define LOG_INFO(x)
#endif

#endif