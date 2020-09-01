#pragma once

#include <stdint.h>
#include <inttypes.h>
#include <adts_snapshot.h>


/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
void
utest_adts_matrix( void );



/**
 **************************************************************************
 * \details
 *  Follow the input format for passing 2D arrays.
 *
 *  https://www.geeksforgeeks.org/pass-2d-array-parameter-c/
 *
 **************************************************************************
 */
#define adts_matrix( _col, _row, _p_data )                           \
    do {                                                             \
        adts_snapshot_t  _snap   = {0};                              \
        adts_snapshot_t *_p_snap = &(_snap);                         \
                                                                     \
        /* Get the call properties */                                \
        adts_snapshot(_p_snap);                                      \
                                                                     \
        /* extract the matrix elem size and select decoder */        \
        switch (sizeof(_p_data[0][0])) {                             \
            case 1:                                                  \
                adts_matrix_private_8(_col, _row, _p_data, _p_snap); \
                break;                                               \
            default:                                                 \
                assert(0);                                           \
        }                                                            \
    } while (0);
