#pragma once

#include <stdint.h>
#include <unistd.h>

/**
 **************************************************************************
 * \brief
 *   Portable definition for userspace branch prediction optimizations
 *
 **************************************************************************
 */
#ifdef __GNUC__
    #define likely(_x)       __builtin_expect(!!(_x), 1)
    #define unlikely(_x)     __builtin_expect(!!(_x), 0)
#else
    #define likely(_x)       (_x)
    #define unlikely(_x)     (_x)
#endif


/**
 **************************************************************************
 * \brief
 *   Min / Max macros
 *
 * \details
 **************************************************************************
 */
#ifndef MIN
    #define MIN( _a, _b )  (((_a) < (_b)) ? (_a) : (_b))
#endif

#ifndef MAX
    #define MAX( _a, _b )  (((_a) > (_b)) ? (_a) : (_b))
#endif





/*
 ****************************************************************************
 *  \details
 *    Number of pointers per system pagesize
 ****************************************************************************
 */
inline size_t
adts_ptrs_per_page( void )
{
    return getpagesize() / sizeof(void *);
} /* adts_ptrs_per_page() */

