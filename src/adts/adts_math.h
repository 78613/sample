
#ifndef _H_ADTS_MATH
#define _H_ADTS_MATH

#include <string.h>
#include <stdbool.h>
#include <inttypes.h>


/**
 **************************************************************************
 * \details
 *
 **************************************************************************
 */
bool
adts_is_prime( const size_t prime );

bool
adts_is_not_prime( const size_t prime );

size_t
adts_prime_ceiling( const size_t limit );

size_t
adts_prime_floor( const size_t limit );

size_t
adts_pow2_round_up( const uint32_t input );

size_t
adts_pow2_round_down( const uint32_t input );

void
utest_adts_math( void );


#endif /* _H_ADTS_MATH */

