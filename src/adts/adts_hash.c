

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
#include <adts_services.h>


/*
 ****************************************************************************
 *  Future work items:
 *   x extend the hash to check for key validity on insert
 *   x extend to exppose the plublic data like the list services
 *
 *   - create a destroy sanity checker to inform of non-empty entries in table
 *   - allow for externally provided memory
 *
 *   x Extend the collision detection logic to indicate max collisions per
 *     entry and current deepest entry
 *
 *   - create a calc function to determine the distribution.  Optionally
 *     perform on insert and assert of bad distribution
 *
 *   x allow for user provided hash function
 *   x default to integer hash function if no input function presented
 *
 *   x statistics for inserts / removes / searches (hash_statis)
 *   x create hash stats display / report function
 *
 *   - create a collision depth warning / threshold option hash create
 *     - assert / error whenver the limit is exceeded
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
 *
 ****************************************************************************
 */
typedef struct hash_s {
    /**< public data  - consumer visible */
    adts_hash_public_t pub;

    /**< private data */
    hash_node_t      **workspace;
    adts_sanity_t      sanity;
    hash_idx_t         (*p_func) (struct hash_s *p_hash,
                                  const void    *p_key);
} hash_t;





/******************************************************************************
 * ####### #     # #     #  #####  #######   ###   ####### #     #  #####
 * #       #     # ##    # #     #    #       #    #     # ##    # #     #
 * #       #     # # #   # #          #       #    #     # # #   # #
 * #####   #     # #  #  # #          #       #    #     # #  #  #  #####
 * #       #     # #   # # #          #       #    #     # #   # #       #
 * #       #     # #    ## #     #    #       #    #     # #    ## #     #
 * #        #####  #     #  #####     #      ###   ####### #     #  #####
******************************************************************************/


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
static bool
hash_remove_collision( hash_t       *p_hash,
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
} /* hash_remove_collision() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
static int32_t
hash_insert_collision( hash_t      *p_hash,
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
} /* hash_insert_collision() */


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
adts_hash_display( adts_hash_t *p_adts_hash )
{
    size_t         elems    = 0;
    size_t         digits   = 0;
    hash_t        *p_hash   = (hash_t *) p_adts_hash;
    adts_sanity_t *p_sanity = &(p_hash->sanity);

    adts_sanity_entry(p_sanity);

    /* display the entire hash with dynamic width formatting */
    elems  = p_hash->pub.elems_limit;
    digits = adts_digits_decimal(elems);

    /* Walk the workspace displaying each entry */
    for (size_t idx = 0; idx < elems; idx++) {
        hash_node_t *p_node = p_hash->workspace[idx];

        if (NULL == p_node) {
            /* Sanity */
            printf("[%*d]  node: %p \n", digits, idx, p_node);
            continue;
        }

        while (p_node) {
            printf("[%*d]  node: %p  vaddr: %p  bytes: %d \
                    key: 0x%016llx %4lld  prev: %16p  next: %16p \n",
                    digits,
                    idx,
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

    adts_sanity_exit(p_sanity);

    return;
} /* adts_hash_display() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
int32_t
adts_hash_remove( adts_hash_t *p_adts_hash,
                  const void  *p_key )
{
    bool               remove_ok = false;
    hash_t            *p_hash    = (hash_t *) p_adts_hash;
    size_t             idx       = 0;
    int32_t            rc        = 0;
    hash_node_t       *p_node    = NULL;
    adts_sanity_t     *p_sanity  = &(p_hash->sanity);
    adts_hash_stats_t *p_stats   = &(p_hash->pub.stats);

    adts_sanity_entry(p_sanity);

    idx    = p_hash->p_func(p_hash, p_key);
    p_node = p_hash->workspace[idx];
    if (unlikely(NULL == p_node)) {
        rc = EINVAL;
        goto exception;
    }

    if (likely(NULL == p_node->p_next)) {
        remove_ok              = true;
        p_hash->workspace[idx] = 0;
    }else {
        remove_ok = hash_remove_collision(p_hash, p_key, idx);
    }

exception:
    if (likely(remove_ok)) {
        p_hash->pub.elems_curr--;
        p_stats->removes++;
    }

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
    size_t             idx      = 0;
    int32_t            rc       = 0;
    hash_node_t       *p_node   = (hash_node_t *) p_adts_hash_node;
    adts_sanity_t     *p_sanity = &(p_hash->sanity);
    adts_hash_stats_t *p_stats  = &(p_hash->pub.stats);

    adts_sanity_entry(p_sanity);

    /* Clear and populate consumers node structure as read-only mode */
    memset(p_node, 0, sizeof(*p_node));
    memcpy(&(p_node->pub), p_input, sizeof(p_node->pub));

    /* Hash and insert node */
    idx = p_hash->p_func(p_hash, p_node->pub.p_key);
    if (likely(0 == p_hash->workspace[idx])) {
        p_hash->workspace[idx] = p_node;
    }else {
        /* Collision detected */
        rc = hash_insert_collision(p_hash, p_node, idx);
        if (rc) {
            goto exception;
        }
    }

    p_hash->pub.elems_curr++;
    p_stats->inserts++;

exception:
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
    hash_t            *p_hash   = (hash_t *) p_adts_hash;
    size_t             idx      = 0;
    int32_t            rc       = 0;
    hash_node_t       *p_tmp    = NULL;
    hash_node_t       *p_node   = NULL;
    adts_sanity_t     *p_sanity = &(p_hash->sanity);
    adts_hash_stats_t *p_stats  = &(p_hash->pub.stats);

    adts_sanity_entry(p_sanity);

    idx   = p_hash->p_func(p_hash, p_key);
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
        case ADTS_HASH_OPTS_NONE:
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
adts_hash_t *
adts_hash_create( const adts_hash_create_t *p_op )
{
    hash_t       *p_hash      = NULL;
    int32_t       rc          = 0;
    hash_node_t  *p_elems     = NULL;
    adts_hash_t  *p_adts_hash = NULL;
    const size_t  limit       = adts_ptrs_per_page();
    //const size_t  elems       = adts_prime_ceiling(limit);
    //FIXME: Temporary Test...
    const size_t  elems       = 11;

    assert(p_op);
    assert(adts_is_prime(elems));

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

    p_hash                  = (hash_t *) p_adts_hash;
    p_hash->workspace       = p_elems;
    p_hash->pub.elems_limit = elems;
    p_hash->p_func          = p_op->p_func;

exception:
    if (rc) {
        if (p_elems) {
            free(p_elems);
        }

        if (p_adts_hash) {
            free(p_adts_hash);
        }
    }

    return p_hash;
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
 * \details
 *   this is clearly not the best hash function but it exists in the case
 ****************************************************************************
 */
static size_t
utest_hash_function( hash_t      *p_hash,
                     const void  *p_key )
{
    size_t idx = (int32_t) p_key % p_hash->pub.elems_limit;

    return idx;
} /* utest_hash_function() */

/*
 ****************************************************************************
 * test control
 *
 ****************************************************************************
 */
static void
utest_control( void )
{
    #define UTEST_ELEMS (32)

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: display struck sizes");
        utest_hash_bytes();
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: create destroy");
        adts_hash_t       *p_hash = NULL;
        adts_hash_create_t op     = {0};

        op.p_func = utest_hash_function;

        p_hash = adts_hash_create(&op);
        assert(p_hash);
        adts_hash_destroy(p_hash);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: display empty");
        adts_hash_t       *p_hash = NULL;
        adts_hash_create_t op     = {0};

        op.p_func = utest_hash_function;

        p_hash = adts_hash_create(&op);
        assert(p_hash);
        adts_hash_display(p_hash);
        adts_hash_destroy(p_hash);
    }


    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: remove empty");
        int32_t            rc     = 0;
        adts_hash_t       *p_hash = NULL;
        adts_hash_create_t op     = {0};

        op.p_func = utest_hash_function;

        p_hash = adts_hash_create(&op);
        assert(p_hash);
        rc = adts_hash_remove(p_hash, 5);
        assert(rc);
        adts_hash_destroy(p_hash);
    }


    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: insert one");
        int32_t                  rc     = 0;
        adts_hash_t             *p_hash = NULL;
        adts_hash_node_t         node   = {0};
        adts_hash_create_t       op     = {0};
        adts_hash_node_public_t  input  = {0};

        op.p_func = utest_hash_function;

        p_hash = adts_hash_create(&op);
        assert(p_hash);

        input.p_data = -1;
        input.bytes  = -1;
        input.p_key  = 5;
        rc = adts_hash_insert(p_hash, &(node), &(input));
        assert(0 == rc);

        adts_hash_display(p_hash);
        adts_hash_destroy(p_hash);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: duplicate keys");
        size_t                   key[]                = {5, 5, 5};
        int32_t                  rc                   = 0;
        adts_hash_t             *p_hash               = NULL;
        adts_hash_node_t         node[ UTEST_ELEMS ]  = {0};
        adts_hash_create_t       op                   = {0};
        adts_hash_node_public_t  input[ UTEST_ELEMS ] = {0};

        op.p_func = utest_hash_function;

        p_hash = adts_hash_create(&op);
        assert(p_hash);

        for (int32_t i = 0; i < 3; i++) {
            input[i].p_data = -1;
            input[i].bytes  = sizeof(node[i]);
            input[i].p_key  = key[i];

            rc = adts_hash_insert(p_hash, &(node[i]), input);
            if (0 < i) {
                /* Second entry should fail with duplicate detection */
                assert(rc);
            }
        }

        adts_hash_display(p_hash);
        adts_hash_destroy(p_hash);
    }


    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: generate collisions");
        size_t                   key[]                = {5, 16, 27};
        int32_t                  rc                   = 0;
        adts_hash_t             *p_hash               = NULL;
        adts_hash_node_t         node[ UTEST_ELEMS ]  = {0};
        adts_hash_create_t       op                   = {0};
        adts_hash_node_public_t  input[ UTEST_ELEMS ] = {0};

        op.p_func = utest_hash_function;

        p_hash = adts_hash_create(&op);
        assert(p_hash);
        for (int32_t i = 0; i < 3; i++) {
            input[i].p_data = -1;
            input[i].bytes  = sizeof(node[i]);
            input[i].p_key  = key[i];
            CDISPLAY(" ");
            CDISPLAY("%p %p %p",
                    input[i].p_data, input[i].bytes, input[i].p_key);

            rc = adts_hash_insert(p_hash, &(node[i]), &(input[i]));
            assert(0 == rc);
        }
        adts_hash_display(p_hash);
        adts_hash_destroy(p_hash);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: insert one -> remove one");
        int32_t                  rc     = 0;
        adts_hash_t             *p_hash = NULL;
        adts_hash_node_t         node   = {0};
        adts_hash_create_t       op     = {0};
        adts_hash_node_public_t  input  = {0};

        op.p_func = utest_hash_function;

        p_hash = adts_hash_create(&op);
        assert(p_hash);

        input.p_data = -1;
        input.bytes  = -1;
        input.p_key  = 5;
        rc = adts_hash_insert(p_hash, &(node), &(input));
        assert(0 == rc);

        adts_hash_display(p_hash);
        rc = adts_hash_remove(p_hash, 5);
        assert(0 == rc);

        printf("\n");
        adts_hash_display(p_hash);
        adts_hash_destroy(p_hash);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: generate collisions -> remove from collisions T -> H");
        size_t                   key[]                = {5, 16, 27};
        int32_t                  rc                   = 0;
        adts_hash_t             *p_hash               = NULL;
        adts_hash_node_t         node[ UTEST_ELEMS ]  = {0};
        adts_hash_create_t       op                   = {0};
        adts_hash_node_public_t  input[ UTEST_ELEMS ] = {0};

        op.p_func = utest_hash_function;

        p_hash = adts_hash_create(&op);
        assert(p_hash);

        for (int32_t i = 0; i < 3; i++) {
            input[i].p_data = -1;
            input[i].bytes  = sizeof(node[i]);
            input[i].p_key  = key[i];

            rc = adts_hash_insert(p_hash, &(node[i]), &(input[i]));
            assert(0 == rc);
        }
        adts_hash_display(p_hash);

        for (int32_t i = 0; i < 3; i++) {
            CDISPLAY("remove: %d", key[i]);
            rc = adts_hash_remove(p_hash, key[i]);
            assert(0 == rc);
            adts_hash_display(p_hash);
        }

        CDISPLAY("end");
        adts_hash_display(p_hash);
        adts_hash_destroy(p_hash);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: generate collisions -> remove from collisions H -> T");
        size_t                   key[]                = {5, 16, 27};
        int32_t                  rc                   = 0;
        adts_hash_t             *p_hash               = NULL;
        adts_hash_node_t         node[ UTEST_ELEMS ]  = {0};
        adts_hash_create_t       op                   = {0};
        adts_hash_node_public_t  input[ UTEST_ELEMS ] = {0};

        op.p_func = utest_hash_function;

        p_hash = adts_hash_create(&op);
        assert(p_hash);

        for (int32_t i = 0; i < 3; i++) {
            input[i].p_data = -1;
            input[i].bytes  = sizeof(node[i]);
            input[i].p_key  = key[i];

            rc = adts_hash_insert(p_hash, &(node[i]), &(input[i]));
            assert(0 == rc);
        }
        adts_hash_display(p_hash);

        for (int32_t i = 2; i >= 0; i--) {
            CDISPLAY("remove: %d", key[i]);
            rc = adts_hash_remove(p_hash, key[i]);
            assert(0 == rc);
            adts_hash_display(p_hash);
        }

        CDISPLAY("end");
        adts_hash_display(p_hash);
        adts_hash_destroy(p_hash);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: generate collisions -> remove from collisions MID");
        size_t                   key[]                = {5, 16, 27};
        int32_t                  rc                   = 0;
        adts_hash_t             *p_hash               = NULL;
        adts_hash_node_t         node[ UTEST_ELEMS ]  = {0};
        adts_hash_create_t       op                   = {0};
        adts_hash_node_public_t  input[ UTEST_ELEMS ] = {0};

        op.p_func = utest_hash_function;

        p_hash = adts_hash_create(&op);
        assert(p_hash);

        for (int32_t i = 0; i < 3; i++) {
            input[i].p_data = -1;
            input[i].bytes  = sizeof(node[i]);
            input[i].p_key  = key[i];

            rc = adts_hash_insert(p_hash, &(node[i]), &(input[i]));
            assert(0 == rc);
        }
        adts_hash_display(p_hash);

        CDISPLAY("remove: %d", key[1]);
        rc = adts_hash_remove(p_hash, key[1]);
        assert(0 == rc);

        CDISPLAY("end");
        adts_hash_display(p_hash);
        adts_hash_destroy(p_hash);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: find");
        size_t                   key[]                = {5, 16, 27};
        int32_t                  rc                   = 0;
        adts_hash_t             *p_hash               = NULL;
        adts_hash_node_t         node[ UTEST_ELEMS ]  = {0};
        adts_hash_node_t        *p_out                = NULL;
        adts_hash_create_t       op                   = {0};
        adts_hash_node_public_t  input[ UTEST_ELEMS ] = {0};

        op.p_func = utest_hash_function;

        p_hash = adts_hash_create(&op);
        assert(p_hash);

        for (int32_t i = 0; i < 3; i++) {
            input[i].p_data = -1;
            input[i].bytes  = sizeof(node[i]);
            input[i].p_key  = key[i];

            rc = adts_hash_insert(p_hash, &(node[i]), &(input[i]));
            assert(0 == rc);
        }
        adts_hash_display(p_hash);

        p_out = adts_hash_find(p_hash, key[1]);
        assert(p_out);
        assert(p_out->pub.p_key == key[1]);

        CDISPLAY("remove: %d", key[1]);
        rc = adts_hash_remove(p_hash, key[1]);
        assert(0 == rc);

        CDISPLAY("end");
        adts_hash_display(p_hash);
        adts_hash_destroy(p_hash);
    }

    return;
} /* utest_control() */



/*
 ****************************************************************************
 * test entrypoint
 *
 ****************************************************************************
 */
void
utest_adts_hash( void )
{
    utest_control();

    return;
} /* utest_adts_hash() */
