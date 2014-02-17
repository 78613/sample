
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include <inttypes.h>

/* Toolbox */
#include <adts_stack.h>
#include <adts_private.h>
#include <adts_services.h>

/*
 ****************************************************************************
 *  Future work items:
 *    - probailistic failure reduction on resize growth.  Use a high-watermark
 *      to attempt grow operations to reduce likelyhood of malloc failures.
 *      the current implementaion may fail to grow because it waits till the
 *      stack if full before attempting.
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
 *
 ****************************************************************************
 */
typedef enum {
    STACK_GROW   = 0x22222222,
    STACK_SHRINK = 0x55555555,
} stack_resize_op_t;


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
typedef struct {
    void   *p_data;
    size_t  bytes;
} stack_node_t;


/*
 ****************************************************************************
 * \details
 *   The heap workspace is an array of stack_node_t, thus reserve as:
 *      32bit:
 *        512 entries = pagesize / stack_node_t size
 *
 *      64bit:
 *        256  entries = pagesize / stack_node_t size
 *
 ****************************************************************************
 */
#define STACK_DEFAULT_ELEMS (4096 / sizeof(stack_node_t))


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
typedef struct {
    size_t        elems_curr;
    size_t        elems_limit;
    stack_node_t *workspace;
    adts_sanity_t sanity;
} stack_t;




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
stack_resize( stack_t          *p_stack,
              stack_resize_op_t op )
{
    size_t        limit_new = p_stack->elems_limit * 2;
    size_t        bytes     = limit_new * sizeof(*(p_stack->workspace));
    int32_t       rc        = 0;
    stack_node_t *p_tmp     = NULL;

    switch (op) {
        case STACK_GROW:
            limit_new = p_stack->elems_limit * 2;
            break;
        case STACK_SHRINK:
            limit_new = p_stack->elems_limit / 2;
            break;
        default:
            /* invalid op */
            assert(0);
    }

    /* p_tmp used to handle error case and preserve the workspace */
    p_tmp = realloc(p_stack->workspace, bytes);
    if (NULL == p_tmp) {
        rc = ENOMEM;
        goto exception;
    }

    /* Set the new stack properties */
    p_stack->workspace   = p_tmp;
    p_stack->elems_limit = limit_new;

exception:
    return rc;
} /* stack_resize() */


/*
 ****************************************************************************
 * \details
 *   SIMPLE shrink candidacy logic to avoid excessive churn.
 *
 ****************************************************************************
 */
static inline bool
stack_resize_shrink_candidate( stack_t *p_stack )
{
    bool   rc      = false;
    size_t trigger = 0;

    if (STACK_DEFAULT_ELEMS < p_stack->elems_limit) {
        /* - growth beyond the default,
         * - to avoid resize thrashing, only make resize candidacy when
         *   the utilization is 25% of current, such that after resizing
         *   the usage is at most 50%, */
        trigger = p_stack->elems_limit / 4;
        if (p_stack->elems_curr < trigger) {
            /* Stack is under utilized based on current allocation */
            rc = true;
        }
    }

    return rc;
} /* stack_resize_shrink_candidate() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
bool
adts_stack_is_empty( adts_stack_t *p_adts_stack )
{
    stack_t *p_stack = (stack_t *) p_adts_stack;

    return (0 == p_stack->elems_curr);
} /* adts_stack_is_empty() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
bool
adts_stack_is_not_empty( adts_stack_t *p_adts_stack )
{
    return !(adts_stack_is_empty(p_adts_stack));
} /* adts_stack_is_not_empty() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
size_t
adts_stack_entries( adts_stack_t *p_adts_stack )
{
    stack_t *p_stack = (stack_t *) p_adts_stack;

    return p_stack->elems_curr;
} /* adts_stack_entries() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
void
adts_stack_display( adts_stack_t *p_adts_stack )
{
    size_t         elems    = 0;
    size_t         digits   = 0;
    stack_t       *p_stack  = (stack_t *) p_adts_stack;
    adts_sanity_t *p_sanity = &(p_stack->sanity);

    adts_sanity_entry(p_sanity);

    /* display the entire stack with dynamic width formatting */
    elems  = adts_stack_entries(p_adts_stack);
    digits = adts_digits_decimal(elems);
    for (size_t idx = 0; idx < elems; idx++) {
        printf("[%*d]  data: %16p  bytes: %8d \n",
                digits,
                idx,
                p_stack->workspace[idx].p_data,
                p_stack->workspace[idx].bytes);
    }

    adts_sanity_exit(p_sanity);

    return;
} /* adts_stack_display() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
void *
adts_stack_peek( adts_stack_t *p_adts_stack )
{
    stack_t       *p_stack  = (stack_t *) p_adts_stack;
    adts_sanity_t *p_sanity = &(p_stack->sanity);

    adts_sanity_entry(p_sanity);

    int32_t       idx     = p_stack->elems_curr - 1;
    stack_node_t *p_elem  = &(p_stack->workspace[idx]);
    void         *p_data  = p_elem->p_data;

    adts_sanity_exit(p_sanity);

    return p_data;
} /* adts_stack_peek() */


/*
 ****************************************************************************
 * \details
 *   Consider adding dynamic memory reduction logic for memory efficiency
 *
 ****************************************************************************
 */
void *
adts_stack_pop( adts_stack_t *p_adts_stack )
{
    stack_t       *p_stack  = (stack_t *) p_adts_stack;
    void          *p_data   = NULL;
    int32_t        idx      = 0;
    stack_node_t  *p_elem   = NULL;
    adts_sanity_t *p_sanity = &(p_stack->sanity);

    adts_sanity_entry(p_sanity);

    if (unlikely(0 >= p_stack->elems_curr)) {
        /* empty stack */
        goto exception;
    }

    if (stack_resize_shrink_candidate(p_stack)) {
        /* Do not error out.  Try again on next pop operation */
        (void) stack_resize(p_stack, STACK_SHRINK);
    }

    idx    = p_stack->elems_curr - 1;
    p_elem = &(p_stack->workspace[idx]);

    p_stack->elems_curr--;

    p_data = p_elem->p_data;
    memset(p_elem, 0, sizeof(*p_elem));

exception:
    adts_sanity_exit(p_sanity);
    return p_data;
} /* adts_stack_pop() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
int32_t
adts_stack_push( adts_stack_t *p_adts_stack,
                 void         *p_data,
                 size_t        bytes )
{
    int32_t        rc       = 0;
    int32_t        idx      = 0;
    stack_node_t  *p_elem   = NULL;
    stack_t       *p_stack  = (stack_t *) p_adts_stack;
    adts_sanity_t *p_sanity = &(p_stack->sanity);

    adts_sanity_entry(p_sanity);

    if (unlikely(p_stack->elems_curr == p_stack->elems_limit)) {
        rc = stack_resize(p_stack, STACK_GROW);
        if (rc) {
            goto exception;
        }
    }

    idx            = p_stack->elems_curr;
    p_elem         = &(p_stack->workspace[idx]);
    p_elem->p_data = p_data;
    p_elem->bytes  = bytes;

    p_stack->elems_curr++;

exception:
    adts_sanity_exit(p_sanity);
    return rc;
} /* adts_stack_push() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
void
adts_stack_destroy( adts_stack_t *p_adts_stack )
{
    stack_t       *p_stack  = (stack_t *) p_adts_stack;
    adts_sanity_t *p_sanity = &(p_stack->sanity);

    adts_sanity_entry(p_sanity);

    free(p_stack->workspace);
    free(p_stack);

    /* No adts_sanity_exit() since we've freed the memory */

    return;
} /* adts_stack_destroy() */


/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
adts_stack_t *
adts_stack_create( void )
{
    size_t        elems        = STACK_DEFAULT_ELEMS;
    int32_t       rc           = 0;
    stack_t      *p_stack      = NULL;
    stack_node_t *p_elems      = NULL;
    adts_stack_t *p_adts_stack = NULL;

    p_adts_stack = calloc(1, sizeof(*p_adts_stack));
    if (NULL == p_adts_stack) {
        rc = ENOMEM;
        goto exception;
    }

    p_elems = calloc(elems, sizeof(*p_elems));
    if (NULL == p_elems) {
        rc = ENOMEM;
        goto exception;
    }

    p_stack              = (stack_t *) p_adts_stack;
    p_stack->workspace   = p_elems;
    p_stack->elems_limit = elems;

exception:
    if (rc) {
        if (p_elems) {
            free(p_elems);
        }

        if (p_adts_stack) {
            free(p_adts_stack);
        }
    }

    return p_adts_stack;
} /* adts_stack_create() */



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
utest_stack_bytes( void )
{
    CDISPLAY("[%u]", sizeof(stack_t));
    CDISPLAY("[%u]", sizeof(adts_stack_t));

    _Static_assert(sizeof(stack_t) < sizeof(adts_stack_t),
        "Mismatch structs detected");

    return;
} /* utest_stack_bytes() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
static void
utest_control( void )
{
    size_t key[] = {-1,0,1,2,3,4,5,6,7,8,9};
    size_t elems = sizeof(key) / sizeof(key[0]);

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: simple func test");

        char          foo[64] = {0};
        adts_stack_t *p_stack = NULL;

        p_stack = adts_stack_create();
        (void) adts_stack_push(p_stack, &(foo), sizeof(foo));
        (void) adts_stack_peek(p_stack);
        adts_stack_display(p_stack);
        (void) adts_stack_pop(p_stack);
        adts_stack_destroy(p_stack);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: stack push");
        int32_t       rc      = 0;
        adts_stack_t *p_stack = NULL;

        p_stack = adts_stack_create();
        assert(p_stack);

        for (size_t idx = 0; idx < elems; idx++) {
            rc = adts_stack_push(p_stack, key[idx], sizeof(key[idx]));
            assert(0 == rc);
            adts_stack_display(p_stack);
        }

        adts_stack_destroy(p_stack);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: stack push -> peek validate");

        void         *val     = 0;
        int32_t       rc      = 0;
        adts_stack_t *p_stack = NULL;

        p_stack = adts_stack_create();
        assert(p_stack);

        for (size_t idx = 0; idx < elems; idx++) {
            rc = adts_stack_push(p_stack, key[idx], sizeof(key[idx]));
            assert(0 == rc);
        }

        adts_stack_display(p_stack);

        val = adts_stack_peek(p_stack);
        assert(key[elems - 1] == val);

        adts_stack_destroy(p_stack);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: stack push -> pop");

        void         *val     = 0;
        int32_t       rc      = 0;
        adts_stack_t *p_stack = NULL;

        p_stack = adts_stack_create();
        assert(p_stack);

        for (size_t idx = 0; idx < elems; idx++) {
            rc = adts_stack_push(p_stack, key[idx], sizeof(key[idx]));
            assert(0 == rc);
        }

        adts_stack_display(p_stack);

        for (int32_t idx = (elems - 1); idx >= 0; idx--) {
            CDISPLAY("POP -------------------------------------");
            (void) adts_stack_pop(p_stack);
            adts_stack_display(p_stack);
        }

        adts_stack_destroy(p_stack);
    }

    return;
} /* utest_control() */


/**
 **************************************************************************
 * \details
 *   Public test entrypoint
 *
 **************************************************************************
 */
void
utest_adts_stack( void )
{
    utest_control();

    return;
} /* utest_adts_stack() */
