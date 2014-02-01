
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
} stack_elem_t;


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
typedef struct {
    size_t        elems_curr;
    size_t        elems_limit;
    stack_elem_t *workspace;
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
void *
adts_stack_peek( adts_stack_t *p_adts_stack )
{
    stack_t       *p_stack  = (stack_t *) p_adts_stack;
    adts_sanity_t *p_sanity = &(p_stack->sanity);

    adts_sanity_entry(p_sanity);

    int32_t       offset  = p_stack->elems_curr - 1;
    stack_elem_t *p_elem  = &(p_stack->workspace[offset]);
    void         *p_data  = p_elem->p_data;

    adts_sanity_exit(p_sanity);

    return p_data;
} /* adts_stack_peek() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
void *
adts_stack_pop( adts_stack_t *p_adts_stack )
{
    stack_t       *p_stack  = (stack_t *) p_adts_stack;
    void          *p_data   = NULL;
    int32_t        offset   = 0;
    stack_elem_t  *p_elem   = NULL;
    adts_sanity_t *p_sanity = &(p_stack->sanity);

    adts_sanity_entry(p_sanity);

    if (unlikely(0 == p_stack->elems_curr)) {
        /* empty stack */
        goto exception;
    }

    offset = p_stack->elems_curr - 1;
    p_elem = &(p_stack->workspace[offset]);
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
    int32_t        offset   = 0;
    stack_elem_t  *p_elem   = NULL;
    stack_t       *p_stack  = (stack_t *) p_adts_stack;
    adts_sanity_t *p_sanity = &(p_stack->sanity);

    adts_sanity_entry(p_sanity);

    assert(p_data);
    assert(bytes);

    if (unlikely(p_stack->elems_curr == p_stack->elems_limit)) {
        rc = ENOMEM;
        goto exception;
    }

    offset         = p_stack->elems_curr;
    p_elem         = &(p_stack->workspace[offset]);
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
adts_stack_create( size_t elems )
{
    int32_t       rc           = 0;
    stack_t      *p_stack      = NULL;
    stack_elem_t *p_elems      = NULL;
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
    _Static_assert(sizeof(stack_t) < sizeof(adts_stack_t),
        "Mismatch structs detected");

    CDISPLAY("[%u]", sizeof(stack_t));
    CDISPLAY("[%u]", sizeof(adts_stack_t));

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
    CDISPLAY("=========================================================");
    {
        char          foo[64] = {0};
        char         *p_foo   = &(foo);
        size_t        elems   = 256;
        adts_stack_t *p_stack = NULL;

        p_stack = adts_stack_create(elems);
        (void) adts_stack_push(p_stack, p_foo, sizeof(*p_foo));
        (void) adts_stack_peek(p_stack);
        (void) adts_stack_pop(p_stack);
        (void) adts_stack_destroy(p_stack);
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
