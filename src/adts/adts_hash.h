


#ifndef _H_ADTS_HASH
#define _H_ADTS_HASH

#include <string.h>
#include <stdbool.h>
#include <inttypes.h>

/**
 **************************************************************************
 * \details
 *
 *************************************************************************
 */
#define ADTS_HASH_BYTES      (64)
#define ADTS_HASH_NODE_BYTES (32)


/**
 **************************************************************************
 * \details
 *
 **************************************************************************
 */
typedef struct {
    const char reserved[ ADTS_HASH_BYTES ];
} adts_hash_t;

typedef struct {
    const char reserved[ ADTS_HASH_NODE_BYTES ];
} adts_hash_node_t;


/**
 **************************************************************************
 * \details
 *
 **************************************************************************
 */
void
utest_adts_hash( void );


#endif /* _H_ADTS_HASH */

