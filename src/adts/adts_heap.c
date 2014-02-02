
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
    void   *p_data;
    size_t  bytes;
} heap_elem_t;


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
typedef struct {
    size_t           elems_curr;
    size_t           elems_limit;
    heap_elem_t     *workspace;
    adts_sanity_t    sanity;
    adts_heap_type_t type;
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


//adts_heap_entries()
//adts_heap_peek()
//adts_heap_pop()
//adts_heap_push()
//adts_is_empty()
//adts_is_not_empty()


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
    size_t       elems       = 2; /* init and allow for dynamic resize */
    int32_t      rc          = 0;
    heap_t      *p_heap      = NULL;
    heap_elem_t *p_elems     = NULL;
    adts_heap_t *p_adts_heap = NULL;

    p_adts_heap = calloc(1, sizeof(*p_adts_heap));
    if (NULL == p_adts_heap) {
        rc = ENOMEM;
        goto exception;
    }

    p_elems = calloc(elems, sizeof(*p_elems));
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
