

#ifndef _H_ADTS_GRAPH
#define _H_ADTS_GRAPH

#include <string.h>
#include <stdbool.h>
#include <inttypes.h>

/**
 **************************************************************************
 * \details
 *
 **************************************************************************
 */
#define ADTS_GRAPH_BYTES      (32)
#define ADTS_GRAPH_NODE_BYTES (32)


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


#endif /* _H_ADTS_GRAPH */

