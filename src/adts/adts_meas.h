#pragma once

#include <string.h>
#include <stdbool.h>
#include <inttypes.h>


typedef struct {
    uint64_t min;
    uint64_t max;
    uint64_t total;
    uint32_t entries;
    uint64_t entry[];
} meas_public_t;

#define ADTS_MEAS_MIN_ENTRIES (8)


/**
 **************************************************************************
 * \details
 *
 **************************************************************************
 */
void
utest_adts_meas( void );
