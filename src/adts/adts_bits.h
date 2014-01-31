
#ifndef _H_ADTS_BITS
#define _H_ADTS_BITS

#include <stdbool.h>
#include <inttypes.h>


bool
adts_bit_is_set( int32_t value,
                  int32_t bit );
bool
adts_bit_is_not_set( int32_t value,
                     int32_t bit );
int32_t
adts_bit_clear( int32_t value,
                int32_t bit );
int32_t
adts_bit_clear_msb_to_kth( int32_t value,
                           int32_t bit );
int32_t
adts_bit_clear_lsb_to_kth( int32_t value,
                           int32_t bit );
int32_t
adts_bit_toggle( int32_t value,
                 int32_t bit );
int32_t
adts_bit_clear_lsb( int32_t val );

int32_t
adts_bit_get_lsb( int32_t val );

int32_t
adts_bit_count( int32_t val );

uint32_t
adts_bit_next_largest( uint32_t val );

void
adts_bit_display_32( int32_t val );

void
adts_bit_display_64( int64_t val );

int32_t
adts_bit_reverse( int32_t val );

void
utest_services_bits( void );

#endif /* _H_ADTS_BITS */

