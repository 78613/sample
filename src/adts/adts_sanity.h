#pragma once

#include <stdint.h>
#include <unistd.h>


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
    volatile int32_t busy;
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
adts_sanity_exit( adts_sanity_t *p_sanity )
{
    p_sanity->busy--;

    return;
} /* adts_sanity_exit() */

static inline void
adts_sanity_entry( adts_sanity_t *p_sanity )
{
    /* if !0 on entry, then there's clearly a serialization error on
     * behalf of the ADTS consumer */
    assert(0 == p_sanity->busy);

    p_sanity->busy++;
    /* memory barriers are an improvement to volatile, but... still need
     * to investigate how to portably and efficiently pull this capability
     * into the adts shared library */
    //wmb();


    return;
} /* adts_sanity_entry() */

