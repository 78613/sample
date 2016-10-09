#pragma once

#include <string.h>
#include <stdbool.h>
#include <inttypes.h>

/**
 **************************************************************************
 * \details
 *
 **************************************************************************
 */
#define ADTS_GRAPH_BYTES      (64)
#define ADTS_GRAPH_NODE_BYTES (64)


/**
 **************************************************************************
 * \details
 *
 **************************************************************************
 */
typedef struct {
    const char reserved[ ADTS_GRAPH_BYTES ];
} adts_graph_t;

typedef struct {
    const char reserved[ ADTS_GRAPH_NODE_BYTES ];
} adts_graph_node_t;


void
utest_adts_graph( void );

