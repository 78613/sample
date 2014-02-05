
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

void
utest_adts_math( void );


#endif /* _H_ADTS_MATH */

