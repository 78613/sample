#pragma once

#include <sched.h>        /* sched_getcpu() */
#include <stdio.h>        /* printf() */
#include <pthread.h>      /* pthread_self() */
#include <sys/types.h>    /* getpid() */
#include <sys/resource.h> /* getpriority() */



/**
 **************************************************************************
 *  \brief
 *   preprocessor conditional printf formatter output
 *
 * \details
 *   Input arguments are equivalent to printf.  referrence printf man pages
 *   for api details
 *
 **************************************************************************
 */
#if defined(__ADTS_DISPLAY)
    #define CDISPLAY(_format, ...)                                        \
    do {                                                                  \
        char       _buffer[256] = {0};                                    \
        pid_t      _process     = getpid();                               \
        size_t     _limit       = sizeof(_buffer) - 1;                    \
        int32_t    _core        = sched_getcpu();                         \
        int32_t    _processpri  = getpriority(PRIO_PROCESS, _process);    \
        pthread_t  _thread      = pthread_self();                         \
                                                                          \
        snprintf(_buffer, _limit, _format, ## __VA_ARGS__);               \
        printf("%3u 0x%-8.8x %2i 0x%-8.8x %s %5u %-25.25s %-30.30s %s\n", \
               _core,                                                     \
               _process,                                                  \
               _processpri,                                               \
               _thread,                                                   \
               "|",                                                       \
               __LINE__, __FILE__, __FUNCTION__,                          \
               _buffer);                                                  \
                                                                          \
        /* Serialize console output on exit/error */                      \
        fflush(stdout);                                                   \
    } while(0);
#else
    #define CDISPLAY(_format, ...) /* compile disabled */
#endif


/**
 **************************************************************************
 * \details
 *   Count digits in decimal number
 *
 **************************************************************************
 */
inline size_t
adts_digits_decimal( int32_t val )
{
    size_t digits = 0;

    while (val) {
        val /= 10;
        digits++;
    }

    return digits;
} /* adts_digits_decimal() */

