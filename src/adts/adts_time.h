#pragma once

#include <stdbool.h>
#include <inttypes.h>


/**
 **************************************************************************
 * \details
 *
 **************************************************************************
 */
#define ADTS_TIME_BYTES (256)


/**
 **************************************************************************
 * \details
 *
 **************************************************************************
 */
typedef struct {
    const char reserved[ ADTS_TIME_BYTES ];
} adts_time_t;



int64_t
adts_timestamp_approximate( void );

void
utest_adts_time( void );

