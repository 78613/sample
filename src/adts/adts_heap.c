
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include <inttypes.h>

/* Toolbox */
#include <adts_heap.h>
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
 *
 ****************************************************************************
 */
typedef struct {
    void   *p_data; /**< consumer datapointer */
    size_t  bytes;  /**< data bytes for p_data */
    int64_t key;    /**< Key used to perform min or max heap ordering */
} heap_node_t;


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
#define HEAP_DEFAULT_ELEMS (4096)
typedef struct {
    size_t            elems_curr;
    size_t            elems_limit;
    heap_node_t     **workspace;
    adts_sanity_t     sanity;
    adts_heap_type_t  type;
} heap_t;





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
bool
adts_heap_is_empty( adts_heap_t *p_adts_heap )
{
    heap_t *p_heap = (heap_t *) p_adts_heap;

    return (0 == p_heap->elems_curr);
} /* adts_heap_is_empty() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
bool
adts_heap_is_not_empty( adts_heap_t *p_adts_heap )
{
    return !(adts_heap_is_empty(p_adts_heap));
} /* adts_heap_is_not_empty() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
size_t
adts_heap_entries( adts_heap_t *p_adts_heap )
{
    heap_t *p_heap = (heap_t *) p_adts_heap;

    return p_heap->elems_curr;
} /* adts_heap_entries() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
adts_heap_node_t *
adts_heap_peek( adts_heap_t *p_adts_heap )
{
    heap_t      *p_heap = (heap_t *) p_adts_heap;
    size_t       idx    = adts_heap_entries(p_adts_heap) - 1;
    heap_node_t *p_node = p_heap->workspace[idx];

    return (adts_heap_node_t *) p_node;
} /* adts_heap_peek() */

/*
 ****************************************************************************
 * \details
 *   elems_limit is initalized to 2 on creation, each resize operation is
 *   simplistic as pow2.
 ****************************************************************************
 */
static int32_t
heap_resize( heap_t *p_heap )
{
    size_t       limit_new = p_heap->elems_limit * 2;
    size_t       bytes     = limit_new * sizeof(p_heap->workspace);
    int32_t      rc        = 0;
    heap_node_t *p_tmp     = NULL;

    /* p_tmp used to handle error case and preserve the workspace */
    p_tmp = realloc(p_heap->workspace, bytes);
    if (NULL == p_tmp) {
        rc = ENOMEM;
        goto exception;
    }

    /* Set the new heap properties */
    p_heap->workspace   = p_tmp;
    p_heap->elems_limit = limit_new;

exception:
    return rc;
} /* heap_resize() */


/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
static inline bool
heap_node_swap_candidate( heap_t      *p_heap,
                          heap_node_t *p_parent,
                          heap_node_t *p_child )
{
    bool             rc = false;
    adts_heap_type_t op = p_heap->type;

    switch (op) {
        case ADTS_HEAP_MAX:
            rc = (p_parent->key <= p_child->key);
            break;
        case ADTS_HEAP_MIN:
            rc = (p_parent->key >= p_child->key);
            break;
        default:
            assert(0); /* sanity */
    }

    return rc;
} /* heap_node_swap_candidate() */


/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
static void
heap_push_bubble_up( heap_t *p_heap )
{
    size_t       elems    = adts_heap_entries(p_heap);
    size_t       idx      = elems - 1;
    heap_node_t *p_child  = NULL;
    heap_node_t *p_parent = NULL;

    if (1 <= elems) {
        /* Nothing to do here */
        goto exception;
    }

    p_child  = p_heap->workspace[idx];
    p_parent = p_heap->workspace[idx/2];
    while ((1 < idx) && heap_node_swap_candidate(p_heap, p_parent, p_child)) {
        heap_node_t *p_tmp = NULL;

        /* swap parent and child array entries */
        p_tmp                    = p_heap->workspace[idx];
        p_heap->workspace[idx]   = p_heap->workspace[idx/2];
        p_heap->workspace[idx/2] = p_tmp;

        /* Get index of next parent and child to compare */
        idx     /= 2;
        p_child  = p_heap->workspace[idx];
        p_parent = p_heap->workspace[idx/2];
    }

exception:
    return;
} /* heap_push_bubble_up() */


/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
int32_t
adts_heap_push( adts_heap_t       *p_adts_heap,
                adts_heap_node_t  *p_adts_node_heap,
                void              *p_data,
                size_t             bytes,
                int64_t            key )
{
    heap_t        *p_heap   = (heap_t *) p_adts_heap;
    size_t         idx      = 0;
    int32_t        rc       = 0;
    heap_node_t   *p_node   = (heap_node_t *) p_adts_node_heap;
    adts_sanity_t *p_sanity = &(p_heap->sanity);

    adts_sanity_entry(p_sanity);

    /* Key not validated. Duplicates and 0 value allowed */
    assert(p_data);
    assert(bytes);

    if (unlikely(p_heap->elems_curr == p_heap->elems_limit)) {
        /* Full, perform dynamic resize operation */
        rc = heap_resize(p_heap);
        if (rc) {
            goto exception;
        }
    }

    /* Populate the consumers node structure */
    p_node->p_data = p_data;
    p_node->bytes  = bytes;
    p_node->key    = key;

    /* Insert this node at end of array */
    idx = p_heap->elems_curr;
    p_heap->workspace[idx] = p_node;

    p_heap->elems_curr++;

    heap_push_bubble_up(p_heap);

exception:
    adts_sanity_exit(p_sanity);
    return rc;
} /* adts_heap_push() */


/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
void
adts_heap_destroy( adts_heap_t *p_adts_heap )
{
    heap_t        *p_heap   = (heap_t *) p_adts_heap;
    adts_sanity_t *p_sanity = &(p_heap->sanity);

    adts_sanity_entry(p_sanity);

    free(p_heap->workspace);
    free(p_heap);

    /* No adts_sanity_exit() since we've freed the memory */

    return;
} /* adts_heap_destroy() */


/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
adts_heap_t *
adts_heap_create( adts_heap_type_t type )
{
    size_t       elems       = HEAP_DEFAULT_ELEMS;
    int32_t      rc          = 0;
    heap_t      *p_heap      = NULL;
    heap_node_t *p_elems     = NULL;
    adts_heap_t *p_adts_heap = NULL;

    p_adts_heap = calloc(1, sizeof(*p_adts_heap));
    if (NULL == p_adts_heap) {
        rc = ENOMEM;
        goto exception;
    }

    /* Array of pointers to heap_adts_node_t */
    p_elems = calloc(elems, sizeof(p_elems));
    if (NULL == p_elems) {
        rc = ENOMEM;
        goto exception;
    }

    p_heap              = (heap_t *) p_adts_heap;
    p_heap->type        = type;
    p_heap->workspace   = p_elems;
    p_heap->elems_limit = elems;

exception:
    if (rc) {
        if (p_elems) {
            free(p_elems);
        }

        if (p_adts_heap) {
            free(p_adts_heap);
        }
    }

    return p_adts_heap;
} /* adts_heap_create() */





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
utest_heap_bytes( void )
{

    CDISPLAY("[%u]", sizeof(heap_t));
    CDISPLAY("[%u]", sizeof(adts_heap_t));

    _Static_assert(sizeof(heap_t) < sizeof(adts_heap_t),
        "Mismatch structs detected");

    CDISPLAY("[%u]", sizeof(heap_node_t));
    CDISPLAY("[%u]", sizeof(adts_heap_node_t));

    _Static_assert(sizeof(heap_node_t) < sizeof(adts_heap_node_t),
        "Mismatch structs detected");

    return;
} /* utest_heap_bytes() */


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
        adts_heap_t      *p_heap = NULL;
        adts_heap_type_t  type   = ADTS_HEAP_MIN;

        p_heap = adts_heap_create(type);
        (void) adts_heap_destroy(p_heap);
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
utest_adts_heap( void )
{

    utest_control();

    return;
} /* utest_adts_heap() */
