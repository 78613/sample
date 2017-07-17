

#include <errno.h>
#include <assert.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include <inttypes.h>

/* Toolbox */
#include <adts_list.h>
#include <adts_sanity.h>
#include <adts_private.h>
#include <adts_display.h>


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
typedef struct list_node_s {
    /**< public data  - consumer visible */
    adts_list_node_public_t  pub;

    /**< private data */
    struct list_s           *p_list;
    struct list_node_s      *p_prev;
    struct list_node_s      *p_next;
} list_node_t;


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
typedef struct list_s {
    size_t        elems_curr;
    size_t        elems_max;
    list_node_t  *p_head;
    list_node_t  *p_tail;
    adts_sanity_t sanity;
} list_t;

typedef enum {
    LIST_REMOVE_HEAD,
    LIST_REMOVE_TAIL,
} list_op_remove_t;

typedef enum {
    LIST_APPEND,
    LIST_PREPEND,
} list_op_insert_t;



/******************************************************************************
 * ####### #     # #     #  #####  #######   ###   ####### #     #  #####
 * #       #     # ##    # #     #    #       #    #     # ##    # #     #
 * #       #     # # #   # #          #       #    #     # # #   # #
 * #####   #     # #  #  # #          #       #    #     # #  #  #  #####
 * #       #     # #   # # #          #       #    #     # #   # #       #
 * #       #     # #    ## #     #    #       #    #     # #    ## #     #
 * #        #####  #     #  #####     #      ###   ####### #     #  #####
******************************************************************************/

// adts_list_node_peek_next
// adts_list_node_peek_prev
// adts_list_node_append
// adts_list_node_prepend

// check ID of node for head or tail match
// expect the prev and next fields to be NULL prior to insertion.


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
static void
list_sanity( list_t *p_list )
{
    if ((NULL == p_list->p_head) ^ (NULL == p_list->p_tail)) {
        /* mismatched empty views of list */
        assert(0);
    }

    return;
} /* list_sanity() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
static void
list_node_sanity( list_node_t *p_node )
{
    if (NULL != p_node->p_list) {
        /* input node already belongs to a list */
        assert(0);
    }

    return;
} /* list_node_sanity() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
static adts_list_node_t *
list_remove( adts_list_t      *p_adts_list,
             list_op_remove_t  op )
{
    list_t        *p_list   = (list_t *) p_adts_list;
    list_node_t   *p_node   = NULL;
    adts_sanity_t *p_sanity = &(p_list->sanity);

    adts_sanity_entry(p_sanity);

    if (unlikely(adts_list_is_empty(p_adts_list))) {
        /* empty list */
        goto exception;
    }

    switch (op) {
        case LIST_REMOVE_HEAD:
            p_node = p_list->p_head;

            /* adjust referrences */
            p_list->p_head = p_list->p_head->p_next;
            if (p_list->p_head) {
                p_list->p_head->p_prev = NULL;
            }else {
                /* last element remove, update list tail */
                p_list->p_tail = NULL;
            }
            break;

        case LIST_REMOVE_TAIL:
            p_node = p_list->p_tail;

            /* adjust referrences */
            p_list->p_tail = p_list->p_tail->p_prev;
            if (p_list->p_tail) {
                p_list->p_tail->p_next = NULL;
            }else {
                /* last element remove, update list tail */
                p_list->p_head = NULL;
            }
            break;

        default:
            assert(0);
            break;
    }

    /* clear node external referrences */
    p_node->p_prev = NULL;
    p_node->p_next = NULL;
    p_node->p_list = NULL;

    p_list->elems_curr--;

exception:
    adts_sanity_exit(p_sanity);
    return (adts_list_node_t *) p_node;
} /* list_remove() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
static int32_t
list_insert( adts_list_t      *p_adts_list,
             adts_list_node_t *p_adts_list_node,
             list_op_insert_t  op )
{
    int32_t        rc       = 0;
    list_t        *p_list   = (list_t *) p_adts_list;
    list_node_t   *p_node   = (list_node_t *) p_adts_list_node;
    adts_sanity_t *p_sanity = &(p_list->sanity);

    adts_sanity_entry(p_sanity);
    list_node_sanity(p_node);

    p_node->p_list = p_list;
    memcpy(&(p_node->pub), &(p_adts_list_node->pub), sizeof(p_node->pub));

    if (unlikely(adts_list_is_empty(p_adts_list))) {
        p_list->p_head = p_node;
        p_list->p_tail = p_node;
    }else {
        switch (op) {
            case LIST_APPEND:
                p_list->p_tail->p_next = p_node;
                p_node->p_prev         = p_list->p_tail;
                p_list->p_tail         = p_list->p_tail->p_next;
                break;
            case LIST_PREPEND:
                p_node->p_next         = p_list->p_head;
                p_list->p_head->p_prev = p_node;
                p_list->p_head         = p_node;
                break;
            default:
                assert(0);
                break;
        }
    }

    p_list->elems_curr++;

    adts_sanity_exit(p_sanity);
    return rc;
} /* list_insert() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
bool
adts_list_is_empty( adts_list_t *p_adts_list )
{
    list_t *p_list = (list_t *) p_adts_list;

    return (0 == p_list->elems_curr);
} /* adts_list_is_empty() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
bool
adts_list_is_not_empty( adts_list_t *p_adts_list )
{
    return !(adts_list_is_empty(p_adts_list));
} /* adts_list_is_not_empty() */


/*
 ****************************************************************************
 *  validate head -> tail and tail -> head
 *
 *  invalid example:
 *    (T -> H traversal) != (H -> T traversal)
 *
 *   a -> b -> c -> e -> e
 *             ^
 *             |
 *   x -> y -> z
 ****************************************************************************
 */
bool
adts_list_is_invalid( adts_list_t *p_adts_list )
{
    bool           rc       = false;
    size_t         idx      = 0;
    size_t         elems    = 0;
    list_t        *p_list   = (list_t *) p_adts_list;
    int64_t       *a_comp   = NULL;
    list_node_t   *p_node   = NULL;
    adts_sanity_t *p_sanity = &(p_list->sanity);

    adts_sanity_entry(p_sanity);

    elems  = p_list->elems_curr;
    a_comp = calloc(elems, sizeof(int64_t));
    if (NULL == a_comp) {
        goto exception;
    }

    /* H -> T */
    p_node = p_list->p_head;
    while (p_node) {
        a_comp[idx] = p_node->pub.p_data;
        p_node      = p_node->p_next;
        idx++;
    }

    /* H <- T */
    p_node = p_list->p_tail;
    while (p_node) {
        idx--;
        if (a_comp[idx] != p_node->pub.p_data) {
            rc = true;
            break;
        }
        p_node = p_node->p_prev;
    }

exception:
    if (a_comp) {
        free(a_comp);
    }
    adts_sanity_exit(p_sanity);
    return rc;
} /* adts_list_is_invalid() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
bool
adts_list_is_valid( adts_list_t *p_adts_list )
{
    return !(adts_list_is_invalid(p_adts_list));
} /* adts_list_is_valid() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
size_t
adts_list_entries( adts_list_t *p_adts_list )
{
    list_t *p_list = (list_t *) p_adts_list;

    return p_list->elems_curr;
} /* adts_list_entries() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
size_t
adts_list_entries_max( adts_list_t *p_adts_list )
{
    list_t *p_list = (list_t *) p_adts_list;

    return p_list->elems_max;
} /* adts_list_entries_max() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
static void
adts_list_display( adts_list_t *p_adts_list )
{
    size_t         idx      = 0;
    size_t         elems    = 0;
    size_t         digits   = 0;
    list_t        *p_list   = (list_t *) p_adts_list;
    list_node_t   *p_tmp    = NULL;
    adts_sanity_t *p_sanity = &(p_list->sanity);

    adts_sanity_entry(p_sanity);

    printf("\nlist: %10p  curr: %3d  mac: %3d  head: %10p  tail %10p \n",
            p_list,
            p_list->elems_curr,
            p_list->elems_max,
            p_list->p_head,
            p_list->p_tail);

    elems  = p_list->elems_curr;
    digits = adts_digits_decimal(elems);
    p_tmp  = p_list->p_head;
    while (p_tmp) {
        printf("[%*d]  node: %10p  data: %d  prev: %10p  next: %10p \n",
                digits,
                idx,
                p_tmp,
                p_tmp->pub.p_data,
                p_tmp->p_prev,
                p_tmp->p_next);

        idx++;
        p_tmp = p_tmp->p_next;
    }

    adts_sanity_exit(p_sanity);
    return;
} /* adts_list_display() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
void
adts_list_reverse( adts_list_t *p_adts_list )
{
    list_t        *p_list   = (list_t *) p_adts_list;
    list_node_t   *pre      = NULL;
    list_node_t   *tmp      = NULL;
    list_node_t   *p_node   = NULL;
    adts_sanity_t *p_sanity = &(p_list->sanity);

    adts_sanity_entry(p_sanity);

    /* use a tmp swap var since prev / next is inherently in the struct */
    pre    = p_list->p_head;
    p_node = p_list->p_head;
    while (p_node) {
        tmp            = p_node->p_prev;
        p_node->p_prev = p_node->p_next;
        p_node->p_next = tmp;
        p_node         = p_node->p_prev;
    }
    p_list->p_head = tmp->p_prev;
    p_list->p_tail = pre;

    adts_sanity_exit(p_sanity);
    return;
} /* adts_list_reverse() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
adts_list_node_t *
adts_list_peek_head( adts_list_t *p_adts_list )
{
    list_t *p_list = (list_t *) p_adts_list;

    return (adts_list_node_t *) p_list->p_head;
} /* adts_list_peek_head() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
adts_list_node_t *
adts_list_peek_tail( adts_list_t *p_adts_list )
{
    list_t *p_list = (list_t *) p_adts_list;

    return (adts_list_node_t *) p_list->p_tail;
} /* adts_list_peek_tail() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
int32_t
adts_list_append( adts_list_t      *p_adts_list,
                  adts_list_node_t *p_adts_list_node )
{
    return list_insert(p_adts_list, p_adts_list_node, LIST_APPEND);
} /* adts_list_append() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
int32_t
adts_list_prepend( adts_list_t      *p_adts_list,
                   adts_list_node_t *p_adts_list_node )
{
    return list_insert(p_adts_list, p_adts_list_node, LIST_PREPEND);
} /* adts_list_prepend() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
adts_list_node_t *
adts_list_remove_head( adts_list_t *p_adts_list )
{
    return list_remove(p_adts_list, LIST_REMOVE_HEAD);
} /* adts_list_remove_head() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
adts_list_node_t *
adts_list_remove_tail( adts_list_t *p_adts_list )
{
    return list_remove(p_adts_list, LIST_REMOVE_TAIL);
} /* adts_list_remove_tail() */


/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
void
adts_list_destroy( adts_list_t *p_adts_list )
{
    list_t  *p_list = (list_t *) p_adts_list;

    free(p_list);

    return;
} /* adts_list_destroy() */


/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
adts_list_t *
adts_list_create( void )
{
    return adts_mem_zalloc(sizeof(adts_list_t));
} /* adts_list_create() */


/******************************************************************************
 * #     # #     #   ###   ####### ####### #######  #####  #######  #####
 * #     # ##    #    #       #       #    #       #     #    #    #     #
 * #     # # #   #    #       #       #    #       #          #    #
 * #     # #  #  #    #       #       #    #####    #####     #     #####
 * #     # #   # #    #       #       #    #             #    #          #
 * #     # #    ##    #       #       #    #       #     #    #    #     #
 *  #####  #     #   ###      #       #    #######  #####     #     #####
******************************************************************************/

/*
 ****************************************************************************
 * free / destroy all nodes in a list
 *
 ****************************************************************************
 */
static void
utest_list_destroy_nodes( list_node_t *p_head )
{
    list_node_t *p_tmp  = p_head;
    list_node_t *next   = NULL;

    while (p_tmp) {
        next = p_tmp->p_next;
        free(p_tmp);
        p_tmp = next;
    }

    return;
} /* utest_list_destroy_nodes() */

static void
utest_list_destroy( list_t *p_list )
{
    assert(p_list);

    utest_list_destroy_nodes(p_list->p_head);

    p_list->p_head = NULL;
    p_list->p_tail = NULL;

    return;
} /* utest_list_destroy() */


static list_node_t *
utest_list_create_nodes( size_t  elems,
                         int32_t sid )
{
    list_node_t *prev   = NULL;
    list_node_t *p_tmp  = NULL;
    list_node_t *p_head = NULL;
    size_t       bytes  = sizeof(*p_tmp);

    for (int32_t cnt = 0; cnt < elems; cnt++) {
        p_tmp = malloc(bytes);
        assert(p_tmp);
        memset(p_tmp, 0, bytes);

        /* Identify this node */
        p_tmp->pub.p_data = cnt + sid;

        if (NULL == p_head) {
            p_head = p_tmp;
            prev   = p_head;
        }else {
            prev->p_next  = p_tmp;
            p_tmp->p_prev = prev;
            prev          = p_tmp;
        }
    }

    return p_head;
} /* utest_list_create_nodes() */


static void
utest_list_create( list_t *p_list )
{
    assert(p_list);

    size_t       elems  = 10;
    int32_t      base   = 0;
    list_node_t *prev   = NULL;
    list_node_t *p_node = NULL;

    /* Assign head */
    p_list->p_head = utest_list_create_nodes(elems, base);

    /* Set the tail value */
    p_node = p_list->p_head;
    while (p_node) {
        prev   = p_node;
        p_node = p_node->p_next;
    }
    p_list->p_tail = prev;

    p_list->elems_curr = elems;
    p_list->elems_max  = elems;

    return;
} /* utest_list_create() */


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
utest_list_bytes( void )
{

    CDISPLAY("[%u]", sizeof(list_t));
    CDISPLAY("[%u]", sizeof(adts_list_t));

    _Static_assert(sizeof(list_t) <= sizeof(adts_list_t),
        "Mismatch structs detected");

    CDISPLAY("[%u]", sizeof(list_node_t));
    CDISPLAY("[%u]", sizeof(adts_list_node_t));

    _Static_assert(sizeof(list_node_t) <= sizeof(adts_list_node_t),
        "Mismatch structs detected");

    return;
} /* utest_list_bytes() */


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
        CDISPLAY("Test: display ");

        list_t  list   = {0};
        list_t *p_list = &(list);

        utest_list_create(p_list);
        adts_list_display(p_list);
        utest_list_destroy(p_list);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: list reverse ");

        list_t  list   = {0};
        list_t *p_list = &(list);

        utest_list_create(p_list);
        adts_list_display(p_list);

        adts_list_reverse(p_list);
        adts_list_display(p_list);

        utest_list_destroy(p_list);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: list valid detect ");

        bool    rc     = false;
        list_t  list   = {0};
        list_t *p_list = &(list);

        utest_list_create(p_list);
        adts_list_display(p_list);

        rc = adts_list_is_valid(p_list);
        assert(rc);

        utest_list_destroy(p_list);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: list remove head ");

        list_t  list   = {0};
        list_t *p_list = &(list);

        utest_list_create(p_list);
        adts_list_display(p_list);

        while (adts_list_is_not_empty(p_list)) {
            (void) adts_list_remove_head(p_list);
            adts_list_display(p_list);
        }

        utest_list_destroy(p_list);
    }


    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: list remove tail ");

        list_t  list   = {0};
        list_t *p_list = &(list);

        utest_list_create(p_list);
        adts_list_display(p_list);

        while (adts_list_is_not_empty(p_list)) {
            (void) adts_list_remove_tail(p_list);
            adts_list_display(p_list);
        }

        utest_list_destroy(p_list);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: list create -> append -> remove -> destroy ");

        #define LIST_NODE_ELEMS (5)
        size_t       elems                    = LIST_NODE_ELEMS;
        list_t      *p_list                   = NULL;
        int32_t      rc                       = 0;
        int32_t      idx                      = 0;
        int32_t      val[ LIST_NODE_ELEMS ]   = {5,6,7,8,9};
        list_node_t  nodes[ LIST_NODE_ELEMS ] = {0};

        p_list = adts_list_create();
        assert(p_list);

        for (idx = 0; idx < elems; idx++) {
            nodes[idx].pub.p_data = val[idx];
            nodes[idx].pub.bytes  = sizeof(val[idx]);

            rc = adts_list_append(p_list, &(nodes[idx]));
            assert(0 == rc);

            list_sanity(p_list);
            adts_list_display(p_list);
        }

        for (idx = idx - 1; idx >= 0; idx--) {
            (void) adts_list_remove_head(p_list);
            list_sanity(p_list);
            adts_list_display(p_list);
        }

        adts_list_destroy(p_list);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: list create -> prepend -> remove -> destroy ");

        #define LIST_NODE_ELEMS (5)
        size_t       elems                    = LIST_NODE_ELEMS;
        list_t      *p_list                   = NULL;
        int32_t      rc                       = 0;
        int32_t      idx                      = 0;
        int32_t      val[ LIST_NODE_ELEMS ]   = {5,6,7,8,9};
        list_node_t  nodes[ LIST_NODE_ELEMS ] = {0};

        p_list = adts_list_create();
        assert(p_list);

        for (idx = 0; idx < elems; idx++) {
            nodes[idx].pub.p_data = val[idx];
            nodes[idx].pub.bytes  = sizeof(val[idx]);

            rc = adts_list_prepend(p_list, &(nodes[idx]));
            assert(0 == rc);

            list_sanity(p_list);
            adts_list_display(p_list);
        }

        for (idx = idx - 1; idx >= 0; idx--) {
            (void) adts_list_remove_head(p_list);
            list_sanity(p_list);
            adts_list_display(p_list);
        }

        adts_list_destroy(p_list);
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
utest_adts_list( void )
{
    utest_control();

    return;
} /* utest_adts_list() */
