
#ifndef _H_ADTS_SERVICES
#define _H_ADTS_SERVICES

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
 * \brief
 *   Portable definition for branch prediction optimizations
 *
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
 *   Detect missing serialization
 *
 * \details
 *   Locking for ADTS must be provided by consumer.  This service allows for
 *   detecting lack thereof.
 *
 **************************************************************************
 */
typedef struct {
    int32_t busy;
} adts_sanity_t;



/**
 **************************************************************************
 * \brief
 *   Detect missing serialization
 *
 * \details
 *   Locking for ADTS must be provided by consumer.  This service allows for
 *   detecting lack thereof.
 *
 **************************************************************************
 */
static inline void
adts_sanity_exit( adts_sanity_t *p_snt )
{

    p_snt->busy--;
    wmb();

    return;
} /* adts_sanity_exit() */

static inline void
adts_sanity_entry( adts_sanity_t *p_snt )
{
    /* if !0 on entry, then there's clearly a serialization error on
     * behalf of the ADTS consumer */
    assert(0 == p_snt->busy);

    p_snt->busy++;
    wmb();

    return;
} /* adts_sanity_entry() */


#endif /* _H_ADTS_SERVICES */



