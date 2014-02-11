
#ifndef _H_ADTS_TREE
#define _H_ADTS_TREE

#include <string.h>
#include <stdbool.h>
#include <inttypes.h>


/**
 **************************************************************************
 * \details
 *
 *************************************************************************
 */
#define ADTS_TREE_BYTES      (64)
#define ADTS_TREE_NODE_BYTES (32)


/**
 **************************************************************************
 * \details
 *
 *************************************************************************
 */
typedef enum {
    ADTS_TREE_AVL = 1,
} adts_tree_type_t;


/**
 **************************************************************************
 * \details
 *
 **************************************************************************
 */
typedef struct {
    const char reserved[ ADTS_TREE_BYTES ];
} adts_tree_t;

typedef struct {
    const char reserved[ ADTS_TREE_NODE_BYTES ];
} adts_tree_node_t;



/**
 **************************************************************************
 * \details
 *
 **************************************************************************
 */
void
utest_adts_tree( void );


#endif /* _H_ADTS_TREE */
