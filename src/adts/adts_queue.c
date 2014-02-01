
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include <inttypes.h>

/* Toolbox */
#include <adts_queue.h>
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
typedef struct queue_elem_s {
    void                *p_data;
    struct queue_elem_s *p_prev;
    struct queue_elem_s *p_next;
} queue_elem_t;


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
typedef struct {
    size_t        elems_curr;
    queue_elem_t *p_head;
    queue_elem_t *p_tail;
} queue_t;


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
 *
 ****************************************************************************
 */
bool
queue_is_empty( adts_queue_t *p_adts_queue )
{
    queue_t *p_queue = (queue_t *) p_adts_queue;

    return ((NULL == p_queue->p_head) && (NULL == p_queue->p_tail));
} /* queue_is_empty() */


/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
bool
queue_is_not_empty( adts_queue_t *p_adts_queue )
{
    return !(queue_is_empty(p_adts_queue));
} /* queue_is_not_empty() */


/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
void *
queue_dequeue( adts_queue_t *p_adts_queue )
{
    void         *p_data  = NULL;
    queue_t      *p_queue = (queue_t *) p_adts_queue;
    queue_elem_t *p_node  = NULL;

    /* Ensure we don't defer a null tail pointer */
    if (p_queue->p_tail) {
        p_data = p_queue->p_tail->p_data;
    }else {
        goto exception;
    }

    if (p_queue->p_head == p_queue->p_tail) {
        /* This is the last element */
        p_queue->p_head = NULL;
        p_queue->p_tail = NULL;
    }else {
        /* Remove from tail only */
        p_queue->p_tail         = p_queue->p_tail->p_prev;
        p_queue->p_tail->p_next = NULL;
    }

    /* Remove the node memory */
    free(p_node);

exception:
    return p_data;
} /* queue_dequeue() */


/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
int32_t
queue_enqueue( adts_queue_t *p_adts_queue,
               void         *p_data )
{
    int32_t       rc      = 0;
    queue_t      *p_queue = (queue_t *) p_adts_queue;
    queue_elem_t *p_node  = NULL;

    p_node = calloc(1, sizeof(*p_node));
    if (NULL == p_node) {
        rc = ENOMEM;
        goto exception;
    }
    p_node->p_data = p_data;

    if ((NULL == p_queue->p_head) &&
        (NULL == p_queue->p_tail)) {
        /* first addition to list */
        p_queue->p_head = p_node;
        p_queue->p_tail = p_node;
    }else {
        /* Add new nodes to head of list */
        p_node->p_next          = p_queue->p_head;
        p_queue->p_head->p_prev = p_node;
        p_queue->p_head         = p_node;
    }

exception:
    return rc;
} /* queue_enqueue() */



/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
void
queue_destroy( adts_queue_t *p_adts_queue )
{
    free(p_adts_queue);

    return;
} /* queue_destroy() */


/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
adts_queue_t *
adts_queue_create( void )
{
    adts_queue_t *p_adts_queue =  calloc(1, sizeof(*p_adts_queue));

    return p_adts_queue;
} /* adts_queue_create() */




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
utest_queue_bytes( void )
{
    _Static_assert(sizeof(queue_t) < sizeof(adts_queue_t),
        "Mismatch structs detected");

    CDISPLAY("[%u]", sizeof(queue_t));
    CDISPLAY("[%u]", sizeof(adts_queue_t));

    return;
} /* utest_queue_bytes() */


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
        adts_queue_t *p_queue = NULL;

        p_queue = adts_queue_create();
        queue_enqueue(p_queue, -1);
        (void) queue_dequeue(p_queue);
        queue_destroy(p_queue);
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
utest_adts_queue( void )
{

    utest_control();

    return;
} /* utest_adts_queue() */
