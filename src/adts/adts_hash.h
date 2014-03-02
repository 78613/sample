


#ifndef _H_ADTS_HASH
#define _H_ADTS_HASH

#include <string.h>
#include <stdbool.h>
#include <inttypes.h>

#include <adts_snapshot.h>

/**
 **************************************************************************
 * \details
 *
 *************************************************************************
 */
#define ADTS_HASH_BYTES      (128)
#define ADTS_HASH_NODE_BYTES (32)



/**
 **************************************************************************
 * \details
 *   Input parameters for node insertion
 *
 **************************************************************************
 */
typedef struct {
    void   *p_data;
    size_t  bytes;
    void   *p_key;
} adts_hash_node_public_t;


/**
 **************************************************************************
 * \details
 *   Public node READ ONLY contents
 *
 **************************************************************************
 */
typedef union {
    const char                    reserved[ ADTS_HASH_NODE_BYTES ];
    const adts_hash_node_public_t pub; /**< read only */
} adts_hash_node_t;


/**
 **************************************************************************
 * \details
 *   Volatile hash statistics.  Contents are recalculated on each
 *   resize operation
 *
 **************************************************************************
 */
typedef struct {
    float  loadfactor;    /**< number of slots in use */
    size_t coll_curr;     /**< current collisions */
    size_t coll_max;      /**< maximum collisions */
    size_t chains_curr;   /**< collision chains */
    size_t chains_depth;  /**< maximum collision chain depth */
    size_t inserts;
    size_t removes;
    size_t find_hits;
    size_t find_miss;
} adts_hash_stats_t;


/**
 **************************************************************************
 * \details
 *   lifetime resize statistics
 *
 **************************************************************************
 */
typedef struct {
    size_t grow;
    size_t shrink;
    size_t error;
} adts_hash_resize_t;


/**
 **************************************************************************
 * \details
 *   hash create options
 *
 **************************************************************************
 */
#define ADTS_HASH_OPTS_NONE                (0) /**< Default */
#define ADTS_HASH_OPTS_DISABLE_RESIZE (1 << 1)
typedef uint64_t adts_hash_options_t;


/**
 **************************************************************************
 * \details
 *   enforced type for consumer provided hash function.
 *
 **************************************************************************
 */
typedef size_t hash_idx_t;

typedef struct {
    adts_hash_options_t  options;  /**< options bitfield */
    hash_idx_t           (*p_func) (struct hash_s *p_hash,
                                    const void    *p_key);
    union {
        struct {
            size_t slots; /**< static number of entries - ideally prime */
        } disable_resize;
    } opts;
} adts_hash_create_t;


/**
 **************************************************************************
 * \details
 *   Public READ ONLY hash contents
 *
 **************************************************************************
 */
typedef struct {
    size_t             elems_curr;  /**< current nodes in hash  */
    size_t             elems_limit; /**< hash slots available */
    adts_hash_stats_t  stats;       /**< Statistics */
    adts_hash_resize_t resize;      /**< persistent resize stats */
} adts_hash_public_t;


/**
 **************************************************************************
 * \details
 *   Public READ ONLY hash control / alloc structure
 *
 **************************************************************************
 */
typedef union {
    const char                reserved[ ADTS_HASH_BYTES ];
    const adts_hash_public_t  pub; /**< read only */
} adts_hash_t;



/**
 **************************************************************************
 * \details
 *   hash table display srevice
 *
 **************************************************************************
 */
#define adts_hash_display( _p_hash, _p_message ) \
    do {                                         \
        adts_snapshot_t  _snap   = {0};          \
        adts_snapshot_t *_p_snap = &(_snap);     \
                                                 \
        /* Get the call properties */            \
        adts_snapshot(_p_snap);                  \
                                                 \
        /* Perform the hexdump */                \
        adts_hash_display_worker( _p_hash,       \
                                  _p_message,    \
                                  _p_snap );     \
    } while (0);


/**
 **************************************************************************
 * \details
 *   hash public prototypes
 *
 **************************************************************************
 */
bool
adts_hash_is_empty( const adts_hash_t *p_adts_hash );

bool
adts_hash_is_not_empty( const adts_hash_t *p_adts_hash );

size_t
adts_hash_entries( const adts_hash_t *p_adts_hash );

void
adts_hash_display_worker( adts_hash_t     *p_adts_hash,
                          char            *p_msg,
                          adts_snapshot_t *p_snap );
int32_t
adts_hash_remove( adts_hash_t *p_adts_hash,
                  const void  *p_key );
int32_t
adts_hash_insert( adts_hash_t             *p_adts_hash,
                  adts_hash_node_t        *p_adts_hash_node,
                  adts_hash_node_public_t *p_input );
adts_hash_node_t *
adts_hash_find( adts_hash_t *p_adts_hash,
                const void  *p_key );
void
adts_hash_destroy( adts_hash_t *p_adts_hash );

adts_hash_t *
adts_hash_create( const adts_hash_create_t *p_op );


/**
 **************************************************************************
 * \details
 *   Unit Test prototypes
 *
 **************************************************************************
 */
void
utest_adts_hash( void );

void
utest_adts_hash_public( void );


#endif /* _H_ADTS_HASH */

