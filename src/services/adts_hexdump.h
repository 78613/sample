


#ifndef _H_ADTS_HEXDUMP
#define _H_ADTS_HEXDUMP

#include <stdint.h>



/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
void
utest_adts_hexdump( void );


/**
 **************************************************************************
 * \details
 *
 *
 **************************************************************************
 */
#define adts_hexdump( _p_data, _bytes, _p_message ) \
    do {                                            \
        adts_snapshot_t  _snap   = {0};             \
        adts_snapshot_t *_p_snap = &(_snap);        \
                                                    \
        /* Get the call properties */               \
        adts_snapshot(_p_snap);                     \
                                                    \
        /* Perform the hexdump */                   \
        adts_hexdump_private( _p_data,              \
                              _bytes,               \
                              _p_message,           \
                              _p_snap );            \
    } while (0);


#endif /* _H_ADTS_HEXDUMP */

