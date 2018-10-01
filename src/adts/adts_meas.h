#pragma once

#include <string.h>
#include <stdbool.h>
#include <inttypes.h>


#define ADTS_MEAS_MIN_ENTRIES (8)

#define ADTS_MEAS_BYTES (64)
typedef struct {
    const char reserved[ ADTS_MEAS_BYTES ];
} adts_meas_t;


/**
 **************************************************************************
 * \details
 *
 **************************************************************************
 */
void
utest_adts_meas( void );
