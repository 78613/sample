


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
#define ADTS_HASH_BYTES      (128)
#define ADTS_HASH_NODE_BYTES (32)



/**
 **************************************************************************
 * \details
 *
 **************************************************************************
 */
typedef struct {
    void   *p_data;
    size_t  bytes;
    void   *p_key;
} adts_hash_node_public_t;

typedef union {
    const char                    reserved[ ADTS_HASH_NODE_BYTES ];
    const adts_hash_node_public_t pub; /**< read only */
} adts_hash_node_t;




/**
 **************************************************************************
 * \details
 *   The following statistics are subject to change
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
    size_t resize_grow;
    size_t resize_shrink;
    size_t resize_error;
} adts_hash_stats_t;

#define ADTS_HASH_OPTS_NONE        (0) /**< Default */
typedef uint64_t adts_hash_options_t;

typedef size_t hash_idx_t;

typedef struct {
    adts_hash_options_t  options;  /**< options bitfield */
    hash_idx_t           (*p_func) (struct hash_s *p_hash,
                                    const void    *p_key);
} adts_hash_create_t;

typedef struct {
    size_t             elems_curr;  /**< current nodes in hash  */
    size_t             elems_limit; /**< hash slots available */
    adts_hash_stats_t  stats;       /**< Statistics */
} adts_hash_public_t;

typedef union {
    const char                reserved[ ADTS_HASH_BYTES ];
    const adts_hash_public_t  pub; /**< read only */
} adts_hash_t;



/**
 **************************************************************************
 * \details
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
 *
 **************************************************************************
 */
void
utest_adts_hash( void );

void
utest_adts_hash_public( void );


#endif /* _H_ADTS_HASH */

