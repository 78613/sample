#pragma once

#include <sched.h> /* sched_getcpu() */
#include <stdio.h> /* printf() */


/**
 **************************************************************************
 * \details
 *   Requires prorotype to prevent compiler warnings
 *
 **************************************************************************
 */
int sched_getcpu(void);


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
    #define CDISPLAY(_format, ...)                                      \
    do {                                                                \
        char _buffer[256] = {0};                                        \
                                                                        \
        sprintf(_buffer, _format, ## __VA_ARGS__);                      \
        printf("%3d %4d %-25.25s %-30.30s %s\n",                        \
            sched_getcpu(), __LINE__, __FILE__, __FUNCTION__, _buffer); \
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
static inline size_t
adts_digits_decimal( int32_t val )
{
    size_t digits = 0;

    while (val) {
        val /= 10;
        digits++;
    }

    return digits;
} /* adts_digits_decimal() */

