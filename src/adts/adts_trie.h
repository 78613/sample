#pragma once

#include <string.h>
#include <stdbool.h>
#include <inttypes.h>



/**
 **************************************************************************
 * \details
 *
 *************************************************************************
 */
#define ADTS_TRIE_BYTES      (128)
#define ADTS_TRIE_NODE_BYTES (1024)



/**
 **************************************************************************
 * \details
 *
 **************************************************************************
 */
typedef struct {
    const char reserved[ ADTS_TRIE_BYTES ];
} adts_trie_t;

typedef struct {
    const char reserved[ ADTS_TRIE_NODE_BYTES ];
} adts_trie_node_t;



/**
 **************************************************************************
 * \details
 *
 **************************************************************************
 */
void
utest_adts_trie( void );

