/***
* @copyright  2004-2020  Apache License, Version 2.0
* @FilePath: /uic_comm_sdk_linux_win_demo/src/uic_util.h
* @Description:
*/

#ifndef __ANTIMTP_UTILS__
#define __ANTIMTP_UTILS__

#if defined(WIN32) || defined(_WIN64)
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#define UTF8_STR_LEN 512
#define GBK_STR_LEN 512
#else
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#endif // endif defined(WIN32) || defined(_WIN64)


#if defined(WIN32) || defined(_WIN64)
#define LOG_ERROR printf
#define LOG_INFO printf
#define LOG_DEBUG printf
#else
#define LOG_BASE(log_level, in_file, in_line, ...)                                                   \
    {                                                                              \
        char time_buf[64];                                                         \
        get_time_now_str(time_buf);                                                \
        fprintf(stderr, "%s[%s][%s:%d]", time_buf, log_level, in_file, in_line); \
        fprintf(stderr, ##__VA_ARGS__);                                              \
        fprintf(stderr, "\n");                                                     \
    }

#define LOG_ERROR(args...)        \
    {                             \
        LOG_BASE("ERROR", __FILE__, __LINE__, ##args) \
    }
#define LOG_INFO(args...)        \
    {                            \
        LOG_BASE("INFO", __FILE__, __LINE__, ##args) \
    }
#define LOG_DEBUG(args...)        \
    {                            \
        LOG_BASE("DEBUG", __FILE__, __LINE__, ##args) \
    }
#endif

// 获取毫秒级的时间
unsigned int get_now_time_ms()
{
#if defined(WIN32) || defined(_WIN64)

    return GetTickCount();

#else

    int res;

    struct timespec sNow;
    res = clock_gettime(CLOCK_MONOTONIC, &sNow);
    if (res != 0)
    {
        /* error */
        printf("clock_gettime failed");
        return 0;
    }

    return sNow.tv_sec * 1000 + sNow.tv_nsec / 1000000; /* milliseconds */

#endif
}

// 获取微秒级的时间
unsigned int get_now_time_usec()
{

    int res;

    struct timespec sNow;
    res = clock_gettime(CLOCK_MONOTONIC, &sNow);
    if (res != 0)
    {
        /* error */
        printf("clock_gettime failed");
        return 0;
    }

    return sNow.tv_sec * 1000000 + sNow.tv_nsec / 1000; /* milliseconds */
}

void get_time_now_str(char *m_buf)
{

#if defined(WIN32) || defined(_WIN64)
    m_buf[0] = 0;
#else
    struct timeval now = { 0, 0 };
    gettimeofday(&now, NULL);
    time_t t = now.tv_sec;
    struct tm *sys_tm = localtime(&t);
    struct tm my_tm = *sys_tm;
    snprintf(m_buf, 48, "%d-%02d-%02d %02d:%02d:%02d.%06ld",
        my_tm.tm_year + 1900, my_tm.tm_mon + 1, my_tm.tm_mday,
        my_tm.tm_hour, my_tm.tm_min, my_tm.tm_sec, now.tv_usec);
#endif
}

void gen_random(char *s, const int len)
{
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < len; ++i)
    {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    s[len] = 0;
}

void anti_sleep(unsigned int sleep_ms)
{
    if (sleep_ms > 0)
    {
#if defined(WIN32) || defined(_WIN64)

        Sleep(sleep_ms);
#else
        usleep(sleep_ms * 1000); /* 5000 -> 5 ms */
#endif
    }
}

#if defined(WIN32) || defined(_WIN64)

// Convert GBK to UTF8
int ConvertGBK2UTF8(const char *szGBK, char *szUTF8, int nUTF8Size)
{
    if (!szGBK || !szUTF8)
        return 0;

    int nWcharCnt = MultiByteToWideChar(CP_ACP, 0, szGBK, -1, NULL, 0);
    wchar_t *pWcharTmp = new wchar_t[nWcharCnt];
    memset(pWcharTmp, 0, sizeof(wchar_t) * nWcharCnt);
    MultiByteToWideChar(CP_ACP, 0, szGBK, -1, pWcharTmp, nWcharCnt);

    int nMbyteLen = WideCharToMultiByte(CP_UTF8, 0, pWcharTmp, -1, NULL, 0, NULL, NULL);
    int nRet = 0;
    if (nMbyteLen <= nUTF8Size)
    {
        nRet = WideCharToMultiByte(CP_UTF8, 0, pWcharTmp, -1, szUTF8, nMbyteLen, NULL, NULL);
    }

    delete[] pWcharTmp;
    pWcharTmp = NULL;

    return nRet;
}

// Convert UTF8 to GBK
int ConvertUTF82GBK(const char *szUTF8, char *szGBK, int nGBKSize)
{
    if (!szUTF8 || !szGBK)
        return 0;

    int nWcharCnt = MultiByteToWideChar(CP_UTF8, 0, szUTF8, -1, NULL, 0);
    wchar_t *pWcharTmp = new wchar_t[nWcharCnt];
    memset(pWcharTmp, 0, sizeof(wchar_t) * nWcharCnt);
    MultiByteToWideChar(CP_UTF8, 0, szUTF8, -1, pWcharTmp, nWcharCnt);

    int nMbyteLen = WideCharToMultiByte(CP_ACP, 0, pWcharTmp, -1, NULL, 0, NULL, NULL);
    int nRet = 0;
    if (nMbyteLen <= nGBKSize)
    {
        nRet = WideCharToMultiByte(CP_ACP, 0, pWcharTmp, -1, szGBK, nMbyteLen, NULL, NULL);
    }

    delete[] pWcharTmp;
    pWcharTmp = NULL;

    return nRet;
}
#endif

#endif // endif __ANTIMTP_UTILS__