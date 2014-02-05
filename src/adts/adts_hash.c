

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
 * \FIXME
 *   Extend with more elaborate hash types as needed
 *
 ****************************************************************************
 */
typedef enum {
    HASH_TYPE_INTEGER = 1,
} hash_func_type_t;


/*
 ****************************************************************************
 * \FIXME:
 *   On resize the collisions must be rehashed.  Investigate how to implement
 ****************************************************************************
 */
typedef struct hash_node_s {
    void               *p_data;   /**< consumer datapointer */
    size_t              bytes;    /**< data bytes for  p_data */
    void               *p_key;    /**< Key used for hash */
    struct hash_node_s *p_prev;   /**< collision management */
    struct hash_node_s *p_next;   /**< collision management */
} hash_node_t;


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
typedef struct hash_s {
    size_t             elems_curr;
    size_t             elems_limit;
    size_t             collisions_curr;
    size_t             collisions_max;
    hash_node_t      **workspace;
    adts_sanity_t      sanity;
    hash_func_type_t   hashtype;
    size_t             (*p_func) (struct hash_s *p_hash,
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
static size_t
hash_func_default( hash_t      *p_hash,
                   const void  *p_key )
{
    size_t           idx  = 0;
    int64_t          val  = 0;
    hash_func_type_t type = p_hash->hashtype;

    switch (type) {
        case HASH_TYPE_INTEGER:
            idx = (int32_t) p_key % p_hash->elems_limit;
            break;
        default:
            assert(0);
    }

    return idx;
} /* hash_func_default() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
bool
adts_hash_is_empty( adts_hash_t *p_adts_hash )
{
    hash_t *p_hash = (hash_t *) p_adts_hash;

    return (0 >= p_hash->elems_curr);
} /* adts_hash_is_empty() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
bool
adts_hash_is_not_empty( adts_hash_t *p_adts_hash )
{
    return !(adts_hash_is_empty(p_adts_hash));
} /* adts_hash_is_not_empty() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
size_t
adts_hash_entries( adts_hash_t *p_adts_hash )
{
    hash_t *p_hash = (hash_t *) p_adts_hash;

    return p_hash->elems_curr;
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
    elems  = p_hash->elems_limit;
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
                    p_node->p_data,
                    p_node->bytes,
                    (int64_t) p_node->p_key,
                    (int64_t) p_node->p_key,
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
adts_hash_remove( adts_hash_t   *p_adts_hash,
                  const void    *p_key )
{
    bool           remove_ok = false;
    hash_t        *p_hash    = (hash_t *) p_adts_hash;
    size_t         idx       = 0;
    int32_t        rc        = 0;
    hash_node_t   *p_tmp     = NULL;
    hash_node_t   *p_node    = NULL;
    adts_sanity_t *p_sanity  = &(p_hash->sanity);

    adts_sanity_entry(p_sanity);

    idx    = p_hash->p_func(p_hash, p_key);
    p_node = p_hash->workspace[idx];
    if (unlikely(NULL == p_node)) {
        rc = EINVAL;
        goto exception;
    }

    if (NULL == p_node->p_next) {
        /* simple single entry removal, all is well */
        remove_ok              = true;
        p_hash->workspace[idx] = 0;
        goto exception;
    }

    /* Process the collision chain */
    while (p_node) {
        if (p_key == p_node->p_key) {
            /* Match found. Remove this node. */
            remove_ok  = true;
            break;
        }
        p_node = p_node->p_next;
    }

    /* Logic error if no removal candidates present */
    assert(remove_ok);

    if (NULL == p_node->p_prev) {
        /* Remove from front */
        p_node->p_next->p_prev = NULL;
        p_node                 = p_node->p_next;
        p_hash->workspace[idx] = p_node;
    }else {
        /* Remove from middle or end */
        if (p_node->p_prev) {
            p_node->p_prev->p_next = p_node->p_next;
        }

        if (p_node->p_next) {
            p_node->p_next->p_prev = p_node->p_prev;
        }
    }
    p_hash->collisions_curr--;

exception:
    if (remove_ok) {
        p_hash->elems_curr--;
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
adts_hash_insert( adts_hash_t      *p_adts_hash,
                  adts_hash_node_t *p_adts_hash_node,
                  void             *p_data,
                  size_t            databytes,
                  const void       *p_key )
{
    hash_t        *p_hash   = (hash_t *) p_adts_hash;
    size_t         idx      = 0;
    int32_t        rc       = 0;
    hash_node_t   *p_node   = (hash_node_t *) p_adts_hash_node;
    adts_sanity_t *p_sanity = &(p_hash->sanity);

    adts_sanity_entry(p_sanity);

    /* Key not validated. Duplicates and 0 value allowed */
    assert(p_node);
    assert(p_data);
    assert(databytes);

    /* Populate the consumers node structure */
    p_node->p_data = p_data;
    p_node->bytes  = databytes;
    p_node->p_key  = p_key;

    /* Hash and insert node */
    idx = p_hash->p_func(p_hash, p_key);
    if (0 == p_hash->workspace[idx]) {
        p_hash->workspace[idx] = p_node;
    }else {
        /* collision detected, perform chaining.  Note we allow duplicate
         * keys and leave the instance management up to the consumer. */
        hash_node_t *p_tmp = NULL;

        /* duplicate key sanity */
        p_tmp = p_hash->workspace[idx];
        while (p_tmp) {
            if (p_key == p_tmp->p_key) {
                rc = EINVAL;
                goto exception;
            }
            p_tmp = p_tmp->p_next;
        }

        /* Prepend node to collision list */
        p_tmp                  = p_hash->workspace[idx];
        p_hash->workspace[idx] = p_node;
        p_node->p_next         = p_tmp;
        p_tmp->p_prev          = p_node;

        p_hash->collisions_curr++;
        if (p_hash->collisions_max < p_hash->collisions_curr) {
            p_hash->collisions_max = p_hash->collisions_curr;
        }
    }

    p_hash->elems_curr++;

exception:
    adts_sanity_exit(p_sanity);
    return rc;
} /* adts_hash_insert() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
void
adts_hash_destroy( adts_hash_t *p_adts_hash )
{
    hash_t        *p_hash   = (hash_t *) p_adts_hash;
    adts_sanity_t *p_sanity = &(p_hash->sanity);

    adts_sanity_entry(p_sanity);

    free(p_hash->workspace);
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
adts_hash_create( const size_t     elems,
                  hash_func_type_t type )
{
    hash_t      *p_hash      = NULL;
    int32_t      rc          = 0;
    hash_node_t *p_elems     = NULL;
    adts_hash_t *p_adts_hash = NULL;

    assert(adts_is_prime(elems));

    p_adts_hash = calloc(1, sizeof(*p_hash));
    if (NULL == p_adts_hash) {
        rc = ENOMEM;
        goto exception;
    }

    p_elems = calloc(elems, sizeof(*(p_hash->workspace)));
    if (NULL == p_elems) {
        rc = ENOMEM;
        goto exception;
    }

    p_hash              = (hash_t *) p_adts_hash;
    p_hash->hashtype    = type;
    p_hash->workspace   = p_elems;
    p_hash->elems_limit = elems;
    p_hash->p_func      = hash_func_default;

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
 * test control
 *
 ****************************************************************************
 */
static void
utest_control( void )
{
    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: display struck sizes");

        utest_hash_bytes();
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: create destroy");

        size_t            elems  = 11;
        adts_hash_t      *p_hash = NULL;
        hash_func_type_t  type   = HASH_TYPE_INTEGER;

        p_hash = adts_hash_create(elems, type);
        assert(p_hash);
        adts_hash_destroy(p_hash);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: display empty");

        size_t            elems  = 11;
        adts_hash_t      *p_hash = NULL;
        hash_func_type_t  type   = HASH_TYPE_INTEGER;

        p_hash = adts_hash_create(elems, type);
        assert(p_hash);
        adts_hash_display(p_hash);
        adts_hash_destroy(p_hash);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: remove empty");

        size_t            elems  = 11;
        int32_t           rc     = 0;
        adts_hash_t      *p_hash = NULL;
        hash_func_type_t  type   = HASH_TYPE_INTEGER;

        p_hash = adts_hash_create(elems, type);
        assert(p_hash);
        rc = adts_hash_remove(p_hash, 5);
        assert(rc);
        adts_hash_destroy(p_hash);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: insert one");

        size_t            elems  = 11;
        int32_t           rc     = 0;
        adts_hash_t      *p_hash = NULL;
        hash_func_type_t  type   = HASH_TYPE_INTEGER;
        adts_hash_node_t  node   = {0};

        p_hash = adts_hash_create(elems, type);
        assert(p_hash);
        rc = adts_hash_insert(p_hash, &(node), -1, -1, 5);
        assert(0 == rc);
        adts_hash_display(p_hash);
        adts_hash_destroy(p_hash);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: duplicate keys");
        #define UTEST_ELEMS (32)
        size_t           key[]               = {5, 5, 5};
        adts_hash_node_t node[ UTEST_ELEMS ] = {0};

        size_t            elems  = 11;
        int32_t           rc     = 0;
        adts_hash_t      *p_hash = NULL;
        hash_func_type_t  type   = HASH_TYPE_INTEGER;

        p_hash = adts_hash_create(elems, type);
        assert(p_hash);
        for (int32_t i = 0; i < 3; i++) {
            rc = adts_hash_insert(p_hash,
                    &(node[i]), -1, sizeof(node[0]), key[i]);
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
        #define UTEST_ELEMS (32)
        size_t           key[]               = {5, 16, 27};
        adts_hash_node_t node[ UTEST_ELEMS ] = {0};

        size_t            elems  = 11;
        int32_t           rc     = 0;
        adts_hash_t      *p_hash = NULL;
        hash_func_type_t  type   = HASH_TYPE_INTEGER;

        p_hash = adts_hash_create(elems, type);
        assert(p_hash);
        for (int32_t i = 0; i < 3; i++) {
            rc = adts_hash_insert(p_hash,
                    &(node[i]), -1, sizeof(node[0]), key[i]);
            assert(0 == rc);
        }
        adts_hash_display(p_hash);
        adts_hash_destroy(p_hash);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: insert one -> remove one");

        size_t            elems  = 11;
        int32_t           rc     = 0;
        adts_hash_t      *p_hash = NULL;
        hash_func_type_t  type   = HASH_TYPE_INTEGER;
        adts_hash_node_t  node   = {0};

        p_hash = adts_hash_create(elems, type);
        assert(p_hash);
        rc = adts_hash_insert(p_hash, &(node), -1, -1, 5);
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
        #define UTEST_ELEMS (32)
        size_t           key[]               = {5, 16, 27};
        adts_hash_node_t node[ UTEST_ELEMS ] = {0};

        size_t            elems  = 11;
        int32_t           rc     = 0;
        adts_hash_t      *p_hash = NULL;
        hash_func_type_t  type   = HASH_TYPE_INTEGER;

        p_hash = adts_hash_create(elems, type);
        assert(p_hash);

        for (int32_t i = 0; i < 3; i++) {
            rc = adts_hash_insert(p_hash,
                    &(node[i]), -1, sizeof(node[0]), key[i]);
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
        #define UTEST_ELEMS (32)
        size_t           key[]               = {5, 16, 27};
        adts_hash_node_t node[ UTEST_ELEMS ] = {0};

        size_t            elems  = 11;
        int32_t           rc     = 0;
        adts_hash_t      *p_hash = NULL;
        hash_func_type_t  type   = HASH_TYPE_INTEGER;

        p_hash = adts_hash_create(elems, type);
        assert(p_hash);

        for (int32_t i = 0; i < 3; i++) {
            rc = adts_hash_insert(p_hash,
                    &(node[i]), -1, sizeof(node[0]), key[i]);
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
        #define UTEST_ELEMS (32)
        size_t           key[]               = {5, 16, 27};
        adts_hash_node_t node[ UTEST_ELEMS ] = {0};

        size_t            elems  = 11;
        int32_t           rc     = 0;
        adts_hash_t      *p_hash = NULL;
        hash_func_type_t  type   = HASH_TYPE_INTEGER;

        p_hash = adts_hash_create(elems, type);
        assert(p_hash);

        for (int32_t i = 0; i < 3; i++) {
            rc = adts_hash_insert(p_hash,
                    &(node[i]), -1, sizeof(node[0]), key[i]);
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
