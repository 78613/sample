
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
typedef enum {
    HEAP_GROW   = 0x22222222,
    HEAP_SHRINK = 0x55555555,
} heap_resize_op_t;

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
 * \details
 *   The heap workspace is an array of pointers, thus reserve as:
 *      32bit:
 *        1024 entries = pagesize / ptr size
 *
 *      64bit:
 *        512  entries = pagesize / ptr size
 *
 *   which is 0.5 4k page in 32bit mode and 1 4k page in 64bit mode.
 *
 ****************************************************************************
 */
#define HEAP_DEFAULT_ELEMS (4096 / sizeof(void *))


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
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
 * \details
 *   Dynamically grow or shrink the workspace.  ADTS consumer is
 *   responsible for serialization.
*
 ****************************************************************************
 */
static int32_t
heap_resize( heap_t           *p_heap,
             heap_resize_op_t  op )
{
    size_t        limit_new = p_heap->elems_limit;
    size_t        bytes     = 0;
    int32_t       rc        = 0;
    heap_node_t *p_tmp      = NULL;

    switch (op) {
        case HEAP_GROW:
            limit_new *= 2;
            break;
        case HEAP_SHRINK:
            limit_new /= 2;
            break;
        default:
            /* invalid op */
            assert(0);
    }

    /* p_tmp used to handle error case and preserve the workspace */
    bytes = limit_new * sizeof(p_heap->workspace[0]);
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
 * \details
 *   SIMPLE shrink candidacy logic to avoid excessive churn.
 *
 ****************************************************************************
 */
static inline bool
heap_resize_shrink_candidate( heap_t *p_heap )
{
    bool   rc      = false;
    size_t trigger = 0;

    if (HEAP_DEFAULT_ELEMS < p_heap->elems_limit) {
        /* - growth beyond the default,
         * - to avoid resize thrashing, only make resize candidacy when
         *   the utilization is 25% of current, such that after resizing
         *   the usage is at most 50%, */
        trigger = p_heap->elems_limit / 4;
        if (p_heap->elems_curr < trigger) {
            /* Stack is under utilized based on current allocation */
            rc = true;
        }
    }

    return rc;
} /* heap_resize_shrink_candidate() */


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
heap_adjust_up( heap_t *p_heap )
{
    size_t       elems    = p_heap->elems_curr;
    size_t       idx      = elems - 1;
    heap_node_t *p_child  = NULL;
    heap_node_t *p_parent = NULL;

    if (unlikely(1 >= elems)) {
        /* Nothing to do here */
        goto exception;
    }

    p_child  = p_heap->workspace[idx];
    p_parent = p_heap->workspace[idx/2];
    while ((1 <= idx) && heap_node_swap_candidate(p_heap, p_parent, p_child)) {
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
} /* heap_adjust_up() */


/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
static void
heap_adjust_down( heap_t *p_heap )
{
    size_t           idx   = 0;
    int32_t          rc    = 0;
    const size_t     elems = p_heap->elems_curr;
    adts_heap_type_t op    = p_heap->type;

    if (1 >= elems) {
        /* nothing to do here */
        goto exception;
    }

    for (;;) {
        size_t        idxc     = 0;      /* child index */
        heap_node_t  *p_tmp    = NULL;
        heap_node_t  *p_left   = NULL;
        heap_node_t  *p_right  = NULL;
        const size_t  left     = (idx * 2) + 1;
        const size_t  right    = (idx * 2) + 2;

        p_left   = (left  < elems) ? p_heap->workspace[left]  : NULL;
        p_right  = (right < elems) ? p_heap->workspace[right] : NULL;
        if (likely(p_left && p_right)) {
            switch (op) {
                case ADTS_HEAP_MIN:
                    idxc = (p_left->key < p_right->key) ? left : right;
                    break;
                case ADTS_HEAP_MAX:
                    idxc = (p_left->key > p_right->key) ? left : right;
                    break;
                default:
                    assert(0); /* sanity */
            }
        }else if (p_left) {
            idxc = left;
        }else {
            goto exception;
        }

        /* swap parent and child array entries */
        p_tmp                   = p_heap->workspace[idx];
        p_heap->workspace[idx]  = p_heap->workspace[idxc];
        p_heap->workspace[idxc] = p_tmp;

        idx = idxc;
    }

exception:
    return;
} /* heap_adjust_down() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
bool
adts_heap_is_empty( adts_heap_t *p_adts_heap )
{
    heap_t *p_heap = (heap_t *) p_adts_heap;

    return (0 >= p_heap->elems_curr);
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
void
adts_heap_display( adts_heap_t *p_adts_heap )
{
    size_t         elems    = 0;
    size_t         digits   = 0;
    heap_t        *p_heap   = (heap_t *) p_adts_heap;
    adts_sanity_t *p_sanity = &(p_heap->sanity);

    adts_sanity_entry(p_sanity);

    /* display the entire heap with dynamic width formatting */
    elems  = p_heap->elems_curr;
    digits = adts_digits_decimal(elems);

    /* The heap implementation uses an array representation of a binary tree
     * therefore we simply perform a display / decode of each entry as array
     * format.  Alternately we can perform a BFS display as future extension */
    for (size_t idx = 0; idx < elems; idx++) {
        heap_node_t *p_node = p_heap->workspace[idx];
        printf("[%*d]  node: %p  vaddr: %p  bytes: %d  key: 0x%016llx %-lld \n",
                digits,
                idx,
                p_node,
                p_node->p_data,
                p_node->bytes,
                p_node->key,
                p_node->key );
    }

    adts_sanity_exit(p_sanity);

    return;
} /* adts_heap_display() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
adts_heap_node_t *
adts_heap_peek( adts_heap_t *p_adts_heap )
{
    heap_t      *p_heap = (heap_t *) p_adts_heap;
    heap_node_t *p_node = p_heap->workspace[0];

    return (adts_heap_node_t *) p_node;
} /* adts_heap_peek() */


/*
 ****************************************************************************
 *
 *  FIXME: Add resize shrink capability
 *
 ****************************************************************************
 */
adts_heap_node_t *
adts_heap_pop( adts_heap_t *p_adts_heap )
{
    size_t         idx      = 0;
    heap_t        *p_heap   = (heap_t *) p_adts_heap;
    heap_node_t   *p_node   = NULL;
    const size_t   elems    = p_heap->elems_curr;
    adts_sanity_t *p_sanity = &(p_heap->sanity);

    adts_sanity_entry(p_sanity);

    if (unlikely(0 >= elems)) {
        /* empty heap */
        goto exception;
    }

    if (heap_resize_shrink_candidate(p_heap)) {
        /* Do not error out.  Try again on next pop operation */
        (void) heap_resize(p_heap, HEAP_SHRINK);
    }

    /* Pop root from tree, and overwrite root with last node in tree */
    p_node               = p_heap->workspace[0];
    idx                  = elems - 1;
    p_heap->workspace[0] = p_heap->workspace[idx];

    p_heap->elems_curr--;

    heap_adjust_down(p_heap);

exception:
    adts_sanity_exit(p_sanity);
    return p_node;
} /* adts_heap_pop() */


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
    assert(p_node);
    assert(p_data);
    assert(bytes);

    if (unlikely(p_heap->elems_curr == p_heap->elems_limit)) {
        /* Full, perform dynamic resize operation */
        rc = heap_resize(p_heap, HEAP_GROW);
        if (rc) {
            goto exception;
        }
    }

    /* Populate the consumers node structure */
    p_node->p_data = p_data;
    p_node->bytes  = bytes;
    p_node->key    = key;

    /* Insert this node at end of array */
    idx                    = p_heap->elems_curr;
    p_heap->workspace[idx] = p_node;

    p_heap->elems_curr++;

    heap_adjust_up(p_heap);

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
			p_elems = NULL;
        }

        if (p_adts_heap) {
            free(p_adts_heap);
			p_adts_heap = NULL;
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
    #define UTEST_ELEMS (32)
    size_t           key[]               = {40,18,20,15,13,9,19,1,3,8};
    adts_heap_node_t node[ UTEST_ELEMS ] = {0};
    size_t           elems               = sizeof(key) / sizeof(key[0]);

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: Create -> Detroy");

        adts_heap_t      *p_heap = NULL;
        adts_heap_type_t  type   = ADTS_HEAP_MIN;

        p_heap = adts_heap_create(type);
        (void) adts_heap_destroy(p_heap);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: display empty heap");

        adts_heap_t      *p_heap = NULL;
        adts_heap_type_t  type   = ADTS_HEAP_MIN;

        p_heap = adts_heap_create(type);
        adts_heap_display(p_heap);
        (void) adts_heap_destroy(p_heap);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: pop empty heap");

        adts_heap_t      *p_heap = NULL;
        adts_heap_type_t  type   = ADTS_HEAP_MIN;

        p_heap = adts_heap_create(type);
        adts_heap_pop(p_heap);
        (void) adts_heap_destroy(p_heap);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: MAX heap push");

        adts_heap_t      *p_heap = NULL;
        adts_heap_type_t  type   = ADTS_HEAP_MAX;

        printf("\n");
        printf("\n");
        p_heap = adts_heap_create(type);
        for (size_t i = 0; i < elems; i++) {
            CDISPLAY("Add: -------------------------------------------------");
            adts_heap_push(p_heap, &(node[i]), -1, sizeof(key[i]), key[i]);
            adts_heap_display(p_heap);
            printf("\n");
        }
        (void) adts_heap_destroy(p_heap);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: MIN heap push");

        adts_heap_t      *p_heap = NULL;
        adts_heap_type_t  type   = ADTS_HEAP_MIN;

        printf("\n");
        printf("\n");
        p_heap = adts_heap_create(type);
        for (size_t i = 0; i < elems; i++) {
            CDISPLAY("Add: -------------------------------------------------");
            adts_heap_push(p_heap, &(node[i]), -1, sizeof(key[i]), key[i]);
            adts_heap_display(p_heap);
            printf("\n");
        }
        (void) adts_heap_destroy(p_heap);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: MAX heap pop");

        adts_heap_t      *p_heap = NULL;
        adts_heap_type_t  type   = ADTS_HEAP_MAX;

        p_heap = adts_heap_create(type);
        for (size_t i = 0; i < elems; i++) {
            CDISPLAY("Add: -------------------------------------------------");
            adts_heap_push(p_heap, &(node[i]), -1, sizeof(key[i]), key[i]);
        }
        adts_heap_display(p_heap);

        printf("\n");
        printf("\n");
        for (size_t i = elems; i > 0; i--) {
            CDISPLAY("Remove: ----------------------------------------------");
            adts_heap_pop(p_heap);
            adts_heap_display(p_heap);
            printf("\n");

        }

        (void) adts_heap_destroy(p_heap);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: MIN heap pop");

        adts_heap_t      *p_heap = NULL;
        adts_heap_type_t  type   = ADTS_HEAP_MIN;

        p_heap = adts_heap_create(type);
        for (size_t i = 0; i < elems; i++) {
            CDISPLAY("Add: -------------------------------------------------");
            adts_heap_push(p_heap, &(node[i]), -1, sizeof(key[i]), key[i]);
        }
        adts_heap_display(p_heap);

        printf("\n");
        printf("\n");
        for (size_t i = elems; i > 0; i--) {
            CDISPLAY("Remove: ----------------------------------------------");
            adts_heap_pop(p_heap);
            adts_heap_display(p_heap);
            printf("\n");

        }

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
