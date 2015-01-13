

#include <adts.h>
#include <utest.h>



























/**
 * \brief Queue with two stacks.
 *
 * Implement a queue with two stacks so that each queue operations takes a
 * constant amortized number of stack operations.
 *
 * \notes
 *   define a queue struct composed of two stacks
 *
 *   Use [0] for enqueue operations only
 *   use [1] for dequeue operations only
 *
 *   enqueue()
 *    - if [0] empty
 *      - for each node in [1]
 *        - pop [1] and push [0]
 *    - push [0]
 *
 *   dequeue()
 *    - if [1] empty
 *      - for each node in [0]
 *        - pop [0] and push [1]
 *    - pop [1]
 */


//TEST:
// - sequential enqueues
//   result: dequeue  and validate all appear in reversed order

//TEST:
// - mixed enqueue / dequeue
//   result: dequeue and validate interlaved enqueue / dequeue reversed order

typedef enum {
    ENQUEUE       = 0,
    DEQUEUE       = 1,
    SQUEUE_STACKS = 2,
} squeue_op_t;


typedef struct {
    squeue_op_t   op;
    adts_stack_t *p_stack[2];
} squeue_t;


/**
 * Time: 0(1) for sequential calls
 * Time: 0(n) for non-sequential calls
 */
void *
squeue_dequeue( squeue_t *p_squeue )
{
    void    *p_data = NULL;
    int32_t  rc     = 0;

    if (adts_stack_is_empty(p_squeue->p_stack[DEQUEUE])) {
        for (;;) {
            p_data = adts_stack_pop(p_squeue->p_stack[ENQUEUE]);
            if (NULL == p_data) {
                /* done */
                break;
            }
            rc = adts_stack_push(p_squeue->p_stack[DEQUEUE], p_data, NULL);
            assert(0 == rc);
        }
    }

    /* same as FIFO dequeue */
    p_data = adts_stack_pop(p_squeue->p_stack[DEQUEUE]);

    return p_data;
} /* squeue_dequeue() */


/**
 * Time: 0(1) for sequential calls
 * Time: 0(n) for non-sequential calls
 */
int32_t
squeue_enqueue( squeue_t *p_squeue,
                void     *p_data )
{
    void    *p_tmp = NULL;
    int32_t  rc    = 0;

    /* Always push to ENQUEUE pointer. If contents are in DEQUEUE then
     * move over to ENQUEUE */
    if (adts_stack_is_empty(p_squeue->p_stack[ENQUEUE])) {
        for (;;) {
            p_tmp = adts_stack_pop(p_squeue->p_stack[DEQUEUE]);
            if (NULL == p_tmp) {
                /* Done */
                break;
            }
            rc = adts_stack_push(p_squeue->p_stack[ENQUEUE], p_tmp, NULL);
            assert(0 == rc);
        }
    }

    /* Same as FIFO enqueue */
    rc = adts_stack_push(p_squeue->p_stack[ENQUEUE], p_data, NULL);

    return rc;
} /* squeue_enqueue() */


void
squeue_destroy( squeue_t *p_squeue )
{
    if (NULL == p_squeue) {
        goto exception;
    }

    for (int32_t i = 0; i < SQUEUE_STACKS; i++) {
        free(p_squeue->p_stack[i]);
    }
    free(p_squeue);

exception:
    return;
} /* squeue_destroy() */


squeue_t *
squeue_create( void )
{
    void     *p_tmp    = NULL;
    int32_t   rc       = 0;
    squeue_t *p_squeue = NULL;

    p_tmp = malloc(sizeof(*p_squeue));
    if (NULL == p_tmp) {
        rc = ENOMEM;
        goto exception;
    }
    memset(p_tmp, 0, sizeof(*p_squeue));
    p_squeue = p_tmp;

    for (int32_t i = 0; i < SQUEUE_STACKS; i++) {
        p_tmp = adts_stack_create();
        if (NULL == p_tmp) {
            rc = ENOMEM;
            goto exception;
        }
        p_squeue->p_stack[i] = p_tmp;
    }

exception:
    if (rc) {
        for (int32_t i = 0; i < SQUEUE_STACKS; i++) {
            if (p_squeue->p_stack[i]) {
                free(p_squeue->p_stack[i]);
            }
        }

        if (p_squeue) {
            free(p_squeue);
        }
    }

    return p_squeue;
} /* squeue_create() */












/**
 * \brief Stack with max.
 *
 * Create a data structure that efficiently supports the stack operations
 * (push and pop) and also a return-the-maximum operation.  Assume the
 * elements are reals numbers so that you can compare them.
 *
 * \notes
 *   - create a stack composed of two stacks
 *     - [0] is normal stack
 *     - [1] tracks max history
 *     ! doulicate handling not possible for interleaved cases
 *        - example: 2,3,4,5,5,6,7,3,3
 *          - last 3 is never tracked due to former larger value
 *
 *   push()
 *     - push [0]
 *     - if data >= [1]
 *       - push [1]
 *
 *   pop()
 *     - pop [0]
 *     - if [0] == [1]
 *       - pop [1] to reveal remaining largest elem
 */
typedef enum {
    MS_DATA  = 0,
    MS_MAX   = 1,
    MS_ELEMS = 2,
} m_stack_op_t;

typedef struct {
    adts_stack_t *p_stack[ MS_ELEMS ];
} mstack_t;


/**
 * Time O(1)
 */
void *
mstack_max( mstack_t *p_mstack )
{
    assert(p_mstack);
    return adts_stack_peek(p_mstack->p_stack[MS_MAX]);
} /* mstack_max() */


/**
 * Time O(1)
 */
void *
mstack_pop( mstack_t  *p_mstack )
{
    assert(p_mstack);

    void *p_data = NULL;

    p_data = adts_stack_pop(p_mstack->p_stack[MS_DATA]);
    if (NULL == p_data) {
        goto exception;
    }

    /* the poppped node value matches the max in the tracker, pop and discard
     * from max tracker to reveal the next lowest highest value */
    if (p_data == adts_stack_peek(p_mstack->p_stack[MS_MAX])) {
        (void) adts_stack_pop(p_mstack->p_stack[MS_MAX]);
    }

exception:
    return p_data;
}


/**
 * Time O(1)
 */
int32_t
mstack_push( mstack_t    *p_mstack,
             const void  *p_data )
{
    assert(p_mstack);
    assert(p_data);

    int32_t rc = 0;

    rc = adts_stack_push(p_mstack->p_stack[MS_DATA], p_data, NULL);
    if (rc) {
        goto exception;
    }

    if (p_data > adts_stack_peek(p_mstack->p_stack[MS_DATA])) {
        rc = adts_stack_push(p_mstack->p_stack[MS_MAX], p_data, NULL);
        assert(0 == rc);
    }

exception:
    return rc;
} /* mstack_push() */


void
mstack_destroy( mstack_t *p_mstack )
{
    if (NULL == p_mstack) {
        goto exception;
    }

    for (int32_t i = 0; i < MS_ELEMS; i++) {
        free(p_mstack->p_stack[i]);
    }

    free(p_mstack);

exception:
    return;
} /* mstack_destroy() */

mstack_t *
mstatck_create( void )
{
    void     *p_tmp    = NULL;
    int32_t   rc       = 0;
    mstack_t *p_mstack = NULL;

    p_tmp = malloc(sizeof(*p_mstack));
    if (NULL == p_tmp) {
        rc = ENOMEM;
        goto exception;
    }
    memset(p_tmp, 0, sizeof(*p_mstack));
    p_mstack = p_tmp;

    for (int32_t i = 0; i < MS_ELEMS; i++) {
        p_tmp = adts_stack_create();
        if (NULL == p_tmp) {
            rc = ENOMEM;
            goto exception;

        }
        p_mstack->p_stack[i] = p_tmp;
    }

exception:
    if (rc) {
        for (int32_t i = 0; i < MS_ELEMS; i++) {
            if (p_mstack->p_stack[i]) {
                free(p_mstack->p_stack[i]);
            }
        }

        if (p_mstack) {
            free(p_mstack);
            p_mstack = NULL;
        }
    }

    return p_mstack;
} /* mstatck_create() */















/**
 * \brief Detect cycle in a linked list.
 *
 * A singly-linked data structure is a data structure made up of nodes
 * where each node has a pointer to the next node (or a pointer to null).
 * Suppose that you have a pointer to the first node of a singly-linked
 * list data structure:
 *
 * Determine whether a singly-linked data structure contains a cycle. You
 * may use only two pointers into the list (and no other variables). The
 * running time of your algorithm should be linear in the number of nodes
 * in the data structure.
 *
 * If a singly-linked data structure contains a cycle, determine the
 * first node that participates in the cycle. you may use only a constant
 * number of pointers into the list (and no other variables). The running
 * time of your algorithm should be linear in the number of nodes in the
 * data structure.
 *
 * You may not modify the structure of the linked list
 */
typedef struct lnode_s {
    void           *p_data;
    struct lnode_s *p_prev;
    struct lnode_s *p_next;
} lnode_t;

static lnode_t *
list_cycle( lnode_t *p_node )
{
    assert(p_node);
    assert(p_node->p_next);

    lnode_t *p_slow = p_node;
    lnode_t *p_fast = p_node->p_next;

    /* If match, then cycle detected */
    while (p_slow != p_fast) {
        p_slow = p_slow->p_next;
        p_fast = p_fast->p_next;
        if (p_fast) {
            p_fast = p_fast->p_next;
        }
    }

    /* detect cycle start */
    p_slow = p_node;
    while (p_slow != p_fast) {
        p_slow = p_slow->p_next;
        p_fast = p_fast->p_next;
    }

    return p_slow;
} /* list_cycle() */

















static void
utest_control( void )
{
    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: 1");
    }



    return;
} /* utest_control() */



void
utest_coursera( void )
{
    utest_control();

    return;
} /* utest_coursera() */

