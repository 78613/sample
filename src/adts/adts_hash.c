

#include <errno.h>
#include <assert.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include <inttypes.h>

/* Toolbox */
#include <adts_math.h>
#include <adts_hash.h>
#include <adts_private.h>
#include <adts_hexdump.h>
#include <adts_services.h>


/*
 ****************************************************************************
 *  Future work items:
 *   - create a destroy sanity checker to inform of non-empty entries in table
 *   - allow for externally provided memory
 *   - add time instrumentation to calculate resize jitter cost
 *   - apply valgrind
 *   - review oprofile
 *   - IMPORTANT!!!
 *     - implement disable resize with consumer provided default limit.
 *
 ****************************************************************************
 */

/******************************************************************************
 #####  ####### ######  #     #  #####  ####### #     # ######  #######  #####
#     #    #    #     # #     # #     #    #    #     # #     # #       #     #
#          #    #     # #     # #          #    #     # #     # #       #
 #####     #    ######  #     # #          #    #     # ######  #####    #####
      #    #    #   #   #     # #          #    #     # #   #   #             #
#     #    #    #    #  #     # #     #    #    #     # #    #  #       #     #
 #####     #    #     #  #####   #####     #     #####  #     # #######  #####
******************************************************************************/


/*
 ****************************************************************************
 * \FIXME:
 *   On resize the collisions must be rehashed.  Investigate how to implement
 ****************************************************************************
 */
typedef struct hash_node_s {
    adts_hash_node_public_t  pub;    /**< public data - consumer visible */
    struct hash_node_s      *p_prev; /**< collision management */
    struct hash_node_s      *p_next; /**< collision management */
} hash_node_t;


/*
 ****************************************************************************
 *  \details
 *    default the elements to a prime value
 ****************************************************************************
 */
#define HASH_DEFAULT_ELEMS  (7)


/*
 ****************************************************************************
 * \details
 *   Triggers for resize operations
 ****************************************************************************
 */
#define HASH_LOAD_TRIGGER_SHRINK (.25)
#define HASH_LOAD_TRIGGER_GROW   (.75)


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
typedef enum {
    HASH_GROW   = 0x22222222,
    HASH_SHRINK = 0x55555555,
} hash_resize_op_t;


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
typedef struct hash_s {
    /**< public data  - consumer visible */
    adts_hash_public_t pub;

    /**< private data */
    adts_hash_create_t    params;
    volatile bool         resizing;
    hash_node_t         **workspace;
    adts_sanity_t         sanity;
} hash_t;


/*
 ****************************************************************************
 * \details
 *   Only define prototypes in file if absolutely necessary.  Ideally
 *   define the function before use such that it's not necessary in most cases
 ****************************************************************************
 */
static int32_t
hash_insert( hash_t                  *p_hash,
             hash_node_t             *p_node,
             adts_hash_node_public_t *p_input );



/******************************************************************************
 * ####### #     # #     #  #####  #######   ###   ####### #     #  #####
 * #       #     # ##    # #     #    #       #    #     # ##    # #     #
 * #       #     # # #   # #          #       #    #     # # #   # #
 * #####   #     # #  #  # #          #       #    #     # #  #  #  #####
 * #       #     # #   # # #          #       #    #     # #   # #       #
 * #       #     # #    ## #     #    #       #    #     # #    ## #     #
 * #        #####  #     #  #####     #      ###   ####### #     #  #####
******************************************************************************/


/**
 **************************************************************************
 * \details
 *   Internal / Private use only - serialization disabled
 *
 **************************************************************************
 */
#define hash_display( _p_hash, _p_message )   \
    do {                                      \
        bool             _private = true;     \
        adts_snapshot_t  _snap   = {0};       \
        adts_snapshot_t *_p_snap = &(_snap);  \
                                              \
        /* Get the call properties */         \
        adts_snapshot(_p_snap);               \
                                              \
        /* Perform the hexdump */             \
        hash_display_worker( _p_hash,         \
                             _p_message,      \
                             _p_snap,         \
                             _private );      \
    } while (0);


/**
 **************************************************************************
 * \details
 *
 **************************************************************************
 */
static void
hash_display_workspace( hash_t *p_hash )
{
    size_t elems  = 0;
    size_t digits = 0;

    /* display the entire hash with dynamic width formatting */
    elems  = p_hash->pub.elems_limit;
    digits = adts_digits_decimal(elems);

    /* Walk the workspace displaying each entry */
    for (size_t idx = 0; idx < elems; idx++) {
        char         chain  = ' ';
        hash_node_t *p_node = p_hash->workspace[idx];

        if (NULL == p_node) {
            /* Sanity */
            printf("[%*d]  node: %p \n", digits, idx, p_node);
            continue;
        }

        if (p_node->p_next || p_node->p_prev) {
            /* display chain identifier */
            chain = 'c';
        }

        while (p_node) {
            printf("[%*d]%c node: %p  vaddr: %p  bytes: %d \
                    key: 0x%016llx %4lld  prev: %16p  next: %16p \n",
                    digits,
                    idx,
                    chain,
                    p_node,
                    p_node->pub.p_data,
                    p_node->pub.bytes,
                    (int64_t) p_node->pub.p_key,
                    (int64_t) p_node->pub.p_key,
                    p_node->p_prev,
                    p_node->p_next);

            p_node = p_node->p_next;
        }
    }

    return;
} /* hash_display_workspace() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
static void
hash_display_worker( hash_t          *p_hash,
                     char            *p_msg,
                     adts_snapshot_t *p_snap,
                     bool             private )
{
    adts_hash_stats_t  *p_stats  = &(p_hash->pub.stats);
    adts_hash_resize_t *p_resize = &(p_hash->pub.resize);
    adts_hash_create_t *p_params = &(p_hash->params);

    printf("\n");
    printf("---------------------------------------------------------------\n");
    adts_snapshot_display(p_snap);
    if (p_msg) {
        printf(" Message: \"%s\"\n", p_msg);
    }
    printf("---------------------------------------------------------------\n");

    /* Public contents */

    printf("pub.stats.loadfactor    = %f\n", p_stats->loadfactor);
    printf("pub.stats.coll_curr     = %u\n", p_stats->coll_curr);
    printf("pub.stats.coll_max      = %u\n", p_stats->coll_max);
    printf("pub.stats.chains_curr   = %u\n", p_stats->chains_curr);
    printf("pub.stats.chains_depth  = %u\n", p_stats->chains_depth);
    printf("pub.stats.inserts       = %u\n", p_stats->inserts);
    printf("pub.stats.removes       = %u\n", p_stats->removes);
    printf("pub.stats.find_hits     = %u\n", p_stats->find_hits);
    printf("pub.stats.find_miss     = %u\n", p_stats->find_miss);

    printf("pub.resize.grow         = %u\n", p_resize->grow);
    printf("pub.resize.shrink       = %u\n", p_resize->shrink);
    printf("pub.resize.error        = %u\n", p_resize->error);

    printf("pub.elems_curr          = %i\n", p_hash->pub.elems_curr);
    printf("pub.elems_limit         = %i\n", p_hash->pub.elems_limit);

    if (private) {
        printf("p_hash->resizing        = %i\n", p_hash->resizing);
        printf("p_hash->workspace       = %i\n", p_hash->workspace);
        printf("p_hash->sanity.busy     = %i\n", p_hash->sanity.busy);

        printf("p_hash->params.options  = %i\n", p_params->options);
        printf("p_hash->params.p_func   = %i\n", p_params->p_func);
    }

    hash_display_workspace(p_hash);
    return;
} /* hash_display() */



/*
 ****************************************************************************
 *
 ****************************************************************************
 */
static inline float
hash_load_factor( hash_t *p_hash )
{
    return ((float) p_hash->pub.elems_curr / (float) p_hash->pub.elems_limit);
} /* hash_load_factor() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
static inline bool
hash_resize_disabled( hash_t *p_hash )
{
    return !!(ADTS_HASH_OPTS_DISABLE_RESIZE & p_hash->params.options);
} /* hash_resize_disabled() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
static inline bool
hash_resize_enabled( hash_t *p_hash )
{
    return !(hash_resize_disabled(p_hash));
} /* hash_resize_enabled() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
static void
hash_resize_rehash( hash_t       *p_new,
                    const hash_t *p_old )
{
    int32_t      rc    = 0;
    const size_t limit = p_old->pub.elems_limit;

    /* linear read of each old entry and rehash into new table.  Entries are
     * not removed from old table such that restore is possible if fail. */
    for (int32_t idx = 0; idx < limit; idx++) {
        /* Process each entry and chain encountered */
        hash_node_t  *p_node = NULL;

        p_node = p_old->workspace[idx];
        while (p_node) {
            hash_node_t             *next = p_node->p_next;
            adts_hash_node_public_t input = {0};

            memcpy(&input, &(p_node->pub), sizeof(input));
            rc = hash_insert(p_new, p_node, &input);
            if (rc) {
                /* Invariant violation */
                assert(0 == rc);
                goto exception;
            }
            p_node = next;
        }
    }

exception:
    return;
} /* hash_resize_rehash() */


/*
 ****************************************************************************
 * \details
 *   Given a starting input limit, round up to the next pow2.  Proced to
 *   grow or shrink to corresponding next pow2.  Then return largest prime
 *   within the new pow2 ceiling.
 *
 ****************************************************************************
 */
static size_t
hash_resize_limit( size_t            val,
                   hash_resize_op_t  op )
{
    size_t limit = adts_pow2_round_up(val);

    switch (op) {
        case HASH_GROW:
            limit *= 2;
            break;
        case HASH_SHRINK:
            limit /= 2;
            break;
        default:
            /* invalid op */
            assert(0);
    }

    return adts_prime_ceiling(limit);
} /* hash_resize_limit() */


/*
 ****************************************************************************
 * \details
 *   It is assumed that a resize is serialized from all other operations.
 *   This is enforced by having this function as static and the callers
 *   performing ADTS sanity verification.
 *
 ****************************************************************************
 */
static int32_t
hash_resize( hash_t           *p_hash,
             hash_resize_op_t  op )
{
    size_t             limit_new = 0;
    size_t             elems     = 0;
    size_t             bytes     = 0;
    hash_t             new       = {0};
    int32_t            rc        = 0;
    hash_node_t       *p_new     = NULL;

    p_hash->resizing = true;

    /* p_new used to handle error case and preserve the workspace */
    limit_new = hash_resize_limit(p_hash->pub.elems_limit, op);
    bytes     = limit_new * sizeof(p_hash->workspace[0]);
    p_new     = adts_mem_zalloc(bytes);
    if (NULL == p_new) {
        rc = ENOMEM;
        goto exception;
    }

    /* cpy old hashtbl properties into new temporary structure. */
    memcpy(&(new), p_hash, sizeof(new));

    /* set and clear new hashtbl properties */
    new.pub.elems_curr  = 0;
    new.pub.elems_limit = limit_new;
    new.workspace       = p_new;
    memset(&(new.pub.stats), 0, sizeof(new.pub.stats));

    /* rehash the contents into the new hashtbl, old hashtbl is preserved
     * since the error checks in this path are already validated via
     * previous recursive opereraitions. */
    hash_resize_rehash(&new, p_hash);

    /* clear and free the old hashtbl workspace */
    bytes = p_hash->pub.elems_limit * sizeof(p_hash->workspace[0]);
    memset(p_hash->workspace, 0, bytes);
    free(p_hash->workspace);

    /* all is good, transition new hashtbl into old hashtbl memspace */
    memcpy(p_hash, &(new), sizeof(*p_hash));

exception:
    p_hash->resizing = false;
    return rc;
} /* hash_resize() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
static inline int32_t
hash_resize_check_shrink( hash_t *p_hash )
{
    size_t              limit_new = 0;
    int32_t             rc        = 0;
    adts_hash_stats_t  *p_stats   = &(p_hash->pub.stats);
    adts_hash_resize_t *p_resize  = &(p_hash->pub.resize);
    hash_resize_op_t    op        = HASH_SHRINK;

    if (p_hash->resizing) {
        /* resize in progress */
        goto exception;
    }

    limit_new = hash_resize_limit(p_hash->pub.elems_limit, op);
    if (HASH_DEFAULT_ELEMS > limit_new) {
        /* Prevent shrink to less than min hashtbl slots */
        goto exception;
    }

    if (HASH_LOAD_TRIGGER_SHRINK > p_stats->loadfactor) {
        rc = hash_resize(p_hash, op);
        if (rc) {
            p_resize->error++;
            goto exception;
        }
        p_resize->shrink++;
    }

exception:
    return rc;
} /* hash_resize_check_shrink() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
static inline int32_t
hash_resize_check_grow( hash_t *p_hash )
{
    int32_t             rc       = 0;
    adts_hash_stats_t  *p_stats  = &(p_hash->pub.stats);
    adts_hash_resize_t *p_resize = &(p_hash->pub.resize);

    if (p_hash->resizing) {
        /* resize in progress */
        goto exception;
    }

    if (HASH_LOAD_TRIGGER_GROW < p_stats->loadfactor) {
        rc = hash_resize(p_hash, HASH_GROW);
        if (rc) {
            p_resize->error++;
            goto exception;
        }
        p_resize->grow++;
    }

exception:
    return rc;
} /* hash_resize_check_grow() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
static bool
hash_collision_remove( hash_t       *p_hash,
                       void         *p_key,
                       const size_t  idx )
{
    bool               remove_ok = false;
    hash_node_t       *p_tmp     = NULL;
    hash_node_t       *p_node    = p_hash->workspace[idx];
    adts_hash_stats_t *p_stats   = &(p_hash->pub.stats);

    /* Process the collision chain */
    while (p_node) {
        if (p_key == p_node->pub.p_key) {
            /* Match found. Remove this node. */
            remove_ok  = true;
            break;
        }
        p_node = p_node->p_next;
    }

    /* Logic error if no removal candidates present */
    assert(remove_ok);

    if (NULL == p_node->p_prev) {
        /* Remove from list head */
        p_node->p_next->p_prev = NULL;
        p_node                 = p_node->p_next;
        p_hash->workspace[idx] = p_node;
    }else {
        /* Remove from middle or list tail */
        if (p_node->p_prev) {
            p_node->p_prev->p_next = p_node->p_next;
        }

        if (p_node->p_next) {
            p_node->p_next->p_prev = p_node->p_prev;
        }
    }

    /* process collision statistics, */
    p_stats->coll_curr--;

    /* If no current chain, decrement the chain instances */
    p_tmp = p_hash->workspace[idx];
    p_stats->chains_curr -= (NULL == p_tmp->p_next) ? 1 : 0;

    return remove_ok;
} /* hash_collision_remove() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
static int32_t
hash_collision_insert( hash_t      *p_hash,
                       hash_node_t *p_node,
                       const size_t idx )
{
    size_t             depth   = 0;
    int32_t            rc      = 0;
    hash_node_t       *p_tmp   = NULL;
    adts_hash_stats_t *p_stats = &(p_hash->pub.stats);

    /* duplicate key sanity */
    p_tmp = p_hash->workspace[idx];
    while (p_tmp) {
        if (p_node->pub.p_key == p_tmp->pub.p_key) {
            rc = EINVAL;
        }

        /* travese the enire list for depth stats */
        depth++;
        p_tmp = p_tmp->p_next;
    }

    if (rc) {
        /* key error detected, clear node and exit */
        memset(p_node, 0, sizeof(*p_node));
        goto exception;
    }

    /* Prepend node to collision list */
    p_tmp                  = p_hash->workspace[idx];
    p_hash->workspace[idx] = p_node;
    p_node->p_next         = p_tmp;
    p_tmp->p_prev          = p_node;

    p_stats->coll_curr++;
    p_stats->coll_max = MAX(p_stats->coll_max, p_stats->coll_curr);

    /* Count chain instances, ignore if > 1 since it's already counted */
    p_stats->chains_curr += (1 == depth) ? 1 : 0;

    /* Max node collision _DEPTH_ in hashtable */
    p_stats->chains_depth = MAX(depth, p_stats->chains_depth);

exception:
    return rc;
} /* hash_collision_insert() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
static int32_t
hash_remove( hash_t     *p_hash,
             const void *p_key )
{
    bool                empty     = false;
    bool                remove_ok = false;
    size_t              idx       = 0;
    int32_t             rc        = 0;
    hash_node_t        *p_node    = NULL;
    adts_hash_stats_t  *p_stats   = &(p_hash->pub.stats);
    adts_hash_create_t *p_params  = &(p_hash->params);

    idx    = p_params->p_func(p_hash, p_key);
    p_node = p_hash->workspace[idx];
    if (unlikely(NULL == p_node)) {
        rc = EINVAL;
        goto exception;
    }

    if (likely(NULL == p_node->p_next)) {
        remove_ok              = true;
        empty             = true;
        p_hash->workspace[idx] = 0;
    }else {
        remove_ok = hash_collision_remove(p_hash, p_key, idx);
    }

exception:
    if (likely(remove_ok)) {
        p_hash->pub.elems_curr--;
        p_stats->removes++;
        p_stats->loadfactor = hash_load_factor(p_hash);

        /* resize candidacy only after accounting complete */
        if (unlikely(empty)) {
            /* Resize is relevant when empty elements exist */
            rc = hash_resize_check_shrink(p_hash);
            if (rc) {
                goto exception;
            }
        }
    }

    return rc;
} /* hash_remove() */



/*
 ****************************************************************************
 *
 ****************************************************************************
 */
static int32_t
hash_insert( hash_t                  *p_hash,
             hash_node_t             *p_node,
             adts_hash_node_public_t *p_input )
{
    bool                collision = false;
    size_t              idx       = 0;
    int32_t             rc        = 0;
    adts_hash_stats_t  *p_stats   = &(p_hash->pub.stats);
    adts_hash_create_t *p_params  = &(p_hash->params);

    /* Clear and populate consumers node structure as read-only mode */
    memset(p_node, 0, sizeof(*p_node));
    memcpy(&(p_node->pub), p_input, sizeof(p_node->pub));

    /* Hash and insert node */
    idx = p_params->p_func(p_hash, p_node->pub.p_key);
    if (likely(0 == p_hash->workspace[idx])) {
        p_hash->workspace[idx] = p_node;
    }else {
        collision = true;

        rc = hash_collision_insert(p_hash, p_node, idx);
        if (rc) {
            goto exception;
        }
    }

    p_hash->pub.elems_curr++;
    p_stats->inserts++;
    p_stats->loadfactor = hash_load_factor(p_hash);

    /* resize candidate only after full accounting */
    if (collision && hash_resize_enabled(p_hash)) {
        rc = hash_resize_check_grow(p_hash);
        if (rc) {
            goto exception;
        }
    }

    //hash_display(p_hash, NULL);

exception:
    return rc;
} /* hash_insert() */


/*
 ****************************************************************************
 * \details
 *   detect invalid inputs
 ****************************************************************************
 */
static int32_t
hash_create_sanity( const adts_hash_create_t *p_op )
{
    int32_t             rc   = 0;
    adts_hash_options_t opts = p_op->options;

    if (NULL == p_op->p_func) {
        rc = EINVAL;
        goto exception;
    }

    switch (opts) {
        case ADTS_HASH_OPTS_NONE:           /**< fall through */
        case ADTS_HASH_OPTS_DISABLE_RESIZE:
            break;
        default:
            rc = EINVAL;
    }

exception:
    return rc;
} /* hash_create_sanity() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
bool
adts_hash_is_empty( const adts_hash_t *p_adts_hash )
{
    hash_t *p_hash = (hash_t *) p_adts_hash;

    return (0 >= p_hash->pub.elems_curr);
} /* adts_hash_is_empty() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
bool
adts_hash_is_not_empty( const adts_hash_t *p_adts_hash )
{
    return !(adts_hash_is_empty(p_adts_hash));
} /* adts_hash_is_not_empty() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
size_t
adts_hash_entries( const adts_hash_t *p_adts_hash )
{
    hash_t *p_hash = (hash_t *) p_adts_hash;

    return p_hash->pub.elems_curr;
} /* adts_hash_entries() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
void
adts_hash_display_worker( adts_hash_t     *p_adts_hash,
                          char            *p_msg,
                          adts_snapshot_t *p_snap )
{
    bool           private  = false;
    hash_t        *p_hash   = (hash_t *) p_adts_hash;
    adts_sanity_t *p_sanity = &(p_hash->sanity);

    adts_sanity_entry(p_sanity);
    hash_display_worker(p_hash, p_msg, p_snap, private);
    adts_sanity_exit(p_sanity);

    return;
} /* adts_hash_display_worker() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
int32_t
adts_hash_remove( adts_hash_t *p_adts_hash,
                  const void  *p_key )
{
    hash_t            *p_hash    = (hash_t *) p_adts_hash;
    int32_t            rc        = 0;
    adts_sanity_t     *p_sanity  = &(p_hash->sanity);

    adts_sanity_entry(p_sanity);
    rc = hash_remove(p_hash, p_key);
    adts_sanity_exit(p_sanity);

    return rc;
} /* adts_hash_remove() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
int32_t
adts_hash_insert( adts_hash_t             *p_adts_hash,
                  adts_hash_node_t        *p_adts_hash_node,
                  adts_hash_node_public_t *p_input )
{
    hash_t            *p_hash   = (hash_t *) p_adts_hash;
    int32_t            rc       = 0;
    hash_node_t       *p_node   = (hash_node_t *) p_adts_hash_node;
    adts_sanity_t     *p_sanity = &(p_hash->sanity);

    adts_sanity_entry(p_sanity);
    rc = hash_insert(p_hash, p_node, p_input);
    adts_sanity_exit(p_sanity);

    return rc;
} /* adts_hash_insert() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
adts_hash_node_t *
adts_hash_find( adts_hash_t *p_adts_hash,
                const void  *p_key )
{
    hash_t             *p_hash   = (hash_t *) p_adts_hash;
    size_t              idx      = 0;
    int32_t             rc       = 0;
    hash_node_t        *p_tmp    = NULL;
    hash_node_t        *p_node   = NULL;
    adts_sanity_t      *p_sanity = &(p_hash->sanity);
    adts_hash_stats_t  *p_stats  = &(p_hash->pub.stats);
    adts_hash_create_t *p_params = &(p_hash->params);

    adts_sanity_entry(p_sanity);

    idx   = p_params->p_func(p_hash, p_key);
    p_tmp = p_hash->workspace[idx];

    /* Find a match in the hash table, processing chains_curr _IF_present */
    while (p_tmp) {
        if (p_key == p_tmp->pub.p_key) {
            /* match */
            p_node = p_tmp;
            break;
        }
        p_tmp = p_tmp->p_next;
    }

    if (p_node) {
        p_stats->find_hits++;
    }else {
        p_stats->find_miss++;
    }

    adts_sanity_exit(p_sanity);
    return (adts_hash_node_t *) p_node;
} /* adts_hash_find() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
void
adts_hash_destroy( adts_hash_t *p_adts_hash )
{
    hash_t        *p_hash   = (hash_t *) p_adts_hash;
    size_t         bytes    = 0;
    adts_sanity_t *p_sanity = &(p_hash->sanity);

    adts_sanity_entry(p_sanity);

    /* clear the workspace memory, note that this take into accoung a hashtbl
     * resize since we use the current elem count limit to determine the
     * bytes of the workspace */
    bytes = p_hash->pub.elems_limit * sizeof(p_hash->workspace[0]);
    memset(p_hash->workspace, 0, bytes);
    free(p_hash->workspace);

    /* Ensure proper cleanup to avoid false positives on accidental reuse */
    bytes = sizeof(*p_hash);
    memset(p_hash, 0, bytes);
    free(p_hash);

    /* No adts_sanity_exit() since we've freed the memory */

    return;
} /* adts_hash_destroy() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
adts_hash_t *
adts_hash_create( const adts_hash_create_t *p_op )
{
    hash_t       *p_hash      = NULL;
    size_t        elems       = 0;
    int32_t       rc          = 0;
    hash_node_t  *p_elems     = NULL;
    adts_hash_t  *p_adts_hash = NULL;

    assert(p_op);
    if (ADTS_HASH_OPTS_DISABLE_RESIZE & p_op->options) {
        elems = p_op->opts.disable_resize.elems;
    }else {
        size_t  dflt  = HASH_DEFAULT_ELEMS;
        size_t  limit = adts_pow2_round_up(dflt);

        elems = adts_prime_ceiling(limit);
        assert(adts_is_prime(elems));
    }

    rc = hash_create_sanity(p_op);
    if (rc) {
        goto exception;
    }

    p_adts_hash = adts_mem_zalloc(sizeof(*p_hash));
    if (NULL == p_adts_hash) {
        rc = ENOMEM;
        goto exception;
    }


    p_elems = adts_mem_zalloc(elems * sizeof(p_hash->workspace[0]));
    if (NULL == p_elems) {
        rc = ENOMEM;
        goto exception;
    }

    p_hash = (hash_t *) p_adts_hash;
    memcpy(&(p_hash->params), p_op, sizeof(*p_op));

    p_hash->workspace       = p_elems;
    p_hash->pub.elems_limit = elems;

exception:
    if (rc) {
        if (p_elems) {
            free(p_elems);
        }

        if (p_adts_hash) {
            free(p_adts_hash);
        }
    }

    return (adts_hash_t *) p_hash;
} /* adts_hash_create() */



/******************************************************************************
 * #     # #     #   ###   ####### ####### #######  #####  #######  #####
 * #     # ##    #    #       #       #    #       #     #    #    #     #
 * #     # # #   #    #       #       #    #       #          #    #
 * #     # #  #  #    #       #       #    #####    #####     #     #####
 * #     # #   # #    #       #       #    #             #    #          #
 * #     # #    ##    #       #       #    #       #     #    #    #     #
 *  #####  #     #   ###      #       #    #######  #####     #     #####
******************************************************************************/


/**
 **************************************************************************
 * \brief
 *   Compile time structure sanity
 *
 * \details
 *   Sanitize the abstract data type interface.  Enforced in header file so
 *   as to catch improper usage/include by unauthorized callers.
 *
 **************************************************************************
 */
static void
utest_hash_bytes( void )
{

    CDISPLAY("[%u]", sizeof(hash_t));
    CDISPLAY("[%u]", sizeof(adts_hash_t));

    _Static_assert(sizeof(hash_t) < sizeof(adts_hash_t),
        "Mismatch structs detected");

    CDISPLAY("[%u]", sizeof(hash_node_t));
    CDISPLAY("[%u]", sizeof(adts_hash_node_t));

    _Static_assert(sizeof(hash_node_t) < sizeof(adts_hash_node_t),
        "Mismatch structs detected");

    return;
} /* utest_hash_bytes() */


/*
 ****************************************************************************
 * test private entrypoint
 *
 ****************************************************************************
 */
static void
utest_adts_hash_private( void )
{
    utest_hash_bytes();

    return;
} /* utest_adts_hash() */


/*
 ****************************************************************************
 * test private entrypoint
 *
 ****************************************************************************
 */
void
utest_adts_hash( void )
{
    utest_adts_hash_private(); /**< private / whitebox tests */
    utest_adts_hash_public();  /**< public / blackbox tests */

    return;
} /* utest_adts_hash() */


