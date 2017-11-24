#pragma once

#include <sched.h>        /* sched_getcpu() */
#include <stdio.h>        /* printf() */
#include <unistd.h>       /* getpid() */
#include <pthread.h>      /* pthread_self() */
#include <sys/types.h>    /* getpid() */
#include <sys/resource.h> /* getpriority() */


/**
 **************************************************************************
 * \details
 * the sched_getcpu() API is missing a prototype by design and causes
 * compiler warnings.  Add the prototype here to address compiler message.
 *
 **************************************************************************
 */
int32_t sched_getcpu(void);


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
    #define CDISPLAY(_format, ...)                                             \
    do {                                                                       \
        char               _buffer[256] = {0};                                 \
        pid_t              _process     = getpid();                            \
        size_t             _limit       = sizeof(_buffer) - 1;                 \
        int32_t            _core        = sched_getcpu();                      \
        int32_t            _policy      = 0;                                   \
        int32_t            _threadpri   = 0;                                   \
        int32_t            _processpri  = 0;                                   \
        pthread_t          _thread      = pthread_self();                      \
        struct sched_param _params      = {0};                                 \
                                                                               \
        _processpri = getpriority(PRIO_PROCESS, (id_t) _process);              \
        _threadpri  = pthread_getschedparam(_thread, &(_policy), &(_params));  \
                                                                               \
        snprintf(_buffer, _limit, _format, ## __VA_ARGS__);                    \
        printf("%3u 0x%-8.8x %2i 0x%-8.8lx %2i %s %5u %-25.25s %-30.30s %s\n", \
               _core,                                                          \
               _process,                                                       \
               _processpri,                                                    \
               _thread,                                                        \
               _threadpri,                                                     \
               "|",                                                            \
               __LINE__, __FILE__, __FUNCTION__,                               \
               _buffer);                                                       \
                                                                               \
        /* Serialize console output on exit/error */                           \
        fflush(stdout);                                                        \
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

