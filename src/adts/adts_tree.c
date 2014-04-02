

#include <errno.h>
#include <assert.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include <inttypes.h>

/* Toolbox */
#include <adts_tree.h>
#include <adts_stack.h>
#include <adts_queue.h>
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


/**
 **************************************************************************
 *
 *************************************************************************
 */
typedef struct tree_node_s {
    void               *p_data;
    size_t              bytes;
    struct tree_s      *p_tree;
    struct tree_node_s *p_left;
    struct tree_node_s *p_right;
    size_t              balance;
} tree_node_t;


/**
 **************************************************************************
 *
 *************************************************************************
 */
typedef struct tree_s {
    size_t            elems_curr;
    size_t            elems_max;
    tree_node_t      *p_root;
    adts_sanity_t     sanity;
    adts_tree_type_t  type;
} tree_t;



/******************************************************************************
 * ####### #     # #     #  #####  #######   ###   ####### #     #  #####
 * #       #     # ##    # #     #    #       #    #     # ##    # #     #
 * #       #     # # #   # #          #       #    #     # # #   # #
 * #####   #     # #  #  # #          #       #    #     # #  #  #  #####
 * #       #     # #   # # #          #       #    #     # #   # #       #
 * #       #     # #    ## #     #    #       #    #     # #    ## #     #
 * #        #####  #     #  #####     #      ###   ####### #     #  #####
******************************************************************************/

/**
 **************************************************************************
 *
 *************************************************************************
 */
static inline void
tree_node_display( tree_node_t *p_node )
{
    printf("[%p]  data: %11c  bytes: %4d \
            tree: %11p  left: %11p  right: %11p\n",
            p_node,
            p_node->p_data,
            p_node->bytes,
            p_node->p_tree,
            p_node->p_left,
            p_node->p_right);

    return;
} /* tree_node_display() */


/**
 **************************************************************************
 *
 *************************************************************************
 */
void
adts_tree_display_preorder( adts_tree_node_t *p_adts_tree_node )
{
    assert(p_adts_tree_node);

    tree_node_t   *p_node   = (tree_node_t *) p_adts_tree_node;
    tree_t        *p_tree   = p_node->p_tree;
    adts_stack_t  *p_stack  = adts_stack_create();
    adts_sanity_t *p_sanity = &(p_tree->sanity);

    //adts_sanity_entry(p_sanity);

    while (p_node || adts_stack_is_not_empty(p_stack)) {
        if (p_node) {
            tree_node_display(p_node);
            (void) adts_stack_push(p_stack, p_node, sizeof(*p_node));
            p_node = p_node->p_left;
        }else {
            p_node = (tree_node_t *) adts_stack_pop(p_stack);
            p_node = p_node->p_right;
        }
    }

exception:
    if (p_stack) {
        adts_stack_destroy(p_stack);
    }

    //adts_sanity_exit(p_sanity);
    return;
} /* adts_tree_display_preorder() */


/**
 **************************************************************************
 *
 *************************************************************************
 */
void
adts_tree_display_inorder( adts_tree_node_t *p_adts_tree_node )
{
    assert(p_adts_tree_node);

    tree_node_t   *p_node   = (tree_node_t *) p_adts_tree_node;
    tree_t        *p_tree   = p_node->p_tree;
    adts_stack_t  *p_stack  = adts_stack_create();
    adts_sanity_t *p_sanity = &(p_tree->sanity);

    //adts_sanity_entry(p_sanity);

    while (p_node || adts_stack_is_not_empty(p_stack)) {
        if (p_node) {
            (void) adts_stack_push(p_stack, p_node, sizeof(*p_node));
            p_node = p_node->p_left;
        }else {
            p_node = (tree_node_t *) adts_stack_pop(p_stack);
            tree_node_display(p_node);
            p_node = p_node->p_right;
        }
    }

exception:
    if (p_stack) {
        adts_stack_destroy(p_stack);
    }

    //adts_sanity_exit(p_sanity);
    return;
} /* adts_tree_display_inorder() */


/**
 **************************************************************************
 *
 *************************************************************************
 */
void
adts_tree_display_postorder( adts_tree_node_t *p_adts_tree_node )
{
    assert(p_adts_tree_node);

    tree_node_t   *p_node   = (tree_node_t *) p_adts_tree_node;
    tree_node_t   *p_last   = NULL;
    tree_t        *p_tree   = p_node->p_tree;
    adts_stack_t  *p_stack  = adts_stack_create();
    adts_sanity_t *p_sanity = &(p_tree->sanity);

    //adts_sanity_entry(p_sanity);

    while (p_node || adts_stack_is_not_empty(p_stack)) {
        if (p_node) {
            (void) adts_stack_push(p_stack, p_node, sizeof(*p_node));
            p_node = p_node->p_left;
        }else {
            tree_node_t *p_peek = NULL;

            /* If last node processed was not p_right, then process p_right. */
            p_peek = (tree_node_t *) adts_stack_peek(p_stack);
            if (p_peek->p_right && (p_peek->p_right != p_last)) {
                p_node = p_peek->p_right;
            }else {
                /* Pop and discard since we already have the node pointer via
                 * p_peek.  We cannot use p_node since ir has a predefined
                 * meaning in this loop. */
                (void) adts_stack_pop(p_stack);
                tree_node_display(p_peek);
                p_last = p_peek;
            }
        }
    }

exception:
    if (p_stack) {
        adts_stack_destroy(p_stack);
    }

    //adts_sanity_exit(p_sanity);
    return;
} /* adts_tree_display_postorder() */


/**
 **************************************************************************
 *
 *************************************************************************
 */
void
adts_tree_display_level( adts_tree_node_t *p_adts_tree_node )
{
    assert(p_adts_tree_node);

    tree_node_t   *p_node   = (tree_node_t *) p_adts_tree_node;
    tree_t        *p_tree   = p_node->p_tree;
    adts_queue_t  *p_queue  = adts_queue_create();
    adts_sanity_t *p_sanity = &(p_tree->sanity);

    //adts_sanity_entry(p_sanity);

    while (p_node) {

        tree_node_display(p_node);

        if (p_node->p_left) {
            adts_queue_enqueue(p_queue,
                    p_node->p_left, sizeof(*(p_node->p_left)));
        }

        if (p_node->p_right) {
            adts_queue_enqueue(p_queue,
                    p_node->p_right, sizeof(*(p_node->p_right)));
        }

        p_node = adts_queue_dequeue(p_queue);
    }

exception:
    if (p_queue) {
        adts_queue_destroy(p_queue);
    }

    //adts_sanity_exit(p_sanity);
    return;
} /* adts_tree_display_level() */



/**
 **************************************************************************
 *
 *************************************************************************
 */
bool
adts_tree_bst_invalid( adts_tree_t *p_adts_tree )
{
    assert(p_adts_tree);

    bool           rc       = false;
    tree_t        *p_tree   = (tree_t *) p_adts_tree;
    int64_t        lastval  = -1;
    tree_node_t   *p_node   = p_tree->p_root;
    adts_stack_t  *p_stack  = adts_stack_create();
    adts_sanity_t *p_sanity = &(p_tree->sanity);

    adts_sanity_entry(p_sanity);

    while (p_node || adts_stack_is_not_empty(p_stack)) {
        if (p_node) {
            (void) adts_stack_push(p_stack, p_node, sizeof(*p_node));
            p_node = p_node->p_left;
        }else {
            p_node = (tree_node_t *) adts_stack_pop(p_stack);

            /* Iterative inorder LNR requires we ensure the values are in
             * ascending order only and adjust p_lastval on each pass */
            tree_node_display(p_node);

            /* Cast to avoid implicit compiler promotion on compare */
            if (lastval < (int64_t) p_node->p_data) {
                lastval = p_node->p_data;
            }else {
                rc = true;
                goto exception;
            }

            p_node = p_node->p_right;
        }
    }

exception:
    if (p_stack) {
        adts_stack_destroy(p_stack);
    }

    adts_sanity_exit(p_sanity);
    return rc;
} /* adts_tree_bst_invalid() */


/**
 **************************************************************************
 *
 *************************************************************************
 */
bool
adts_tree_bst_valid( adts_tree_t *p_adts_tree )
{
    return !(adts_tree_bst_invalid(p_adts_tree));
} /* adts_tree_bst_valid() */


/**
 **************************************************************************
 *
 *************************************************************************
 */
size_t
adts_tree_entries( adts_tree_t *p_adts_tree )
{
    tree_t *p_tree = (tree_t *) p_adts_tree;

    return p_tree->elems_curr;
} /* adts_tree_entries() */


// adts_tree_find()
// adts_tree_peek_min()
// adts_tree_peek_max()
// adts_tree_remove_min()
// adts_tree_remove_max()
// adts_tree_remove_all()
// adts_tree_remove()


/**
 **************************************************************************
 * \details
 *
 *************************************************************************
 */
void *
adts_tree_remove( adts_tree_t *p_adts_tree )
{
    void *p_data = NULL;

    CDISPLAY("Incomplete...");

    return p_data;
} /* adts_tree_remove() */


/**
 **************************************************************************
 * \details
 *
 *************************************************************************
 */
static void
tree_insert( tree_t      *p_tree,
             tree_node_t *p_node )
{
    int32_t      rc      = 0;
    tree_node_t  tnode   = {0};
    tree_node_t *p_tnode = &(tnode); /* tmp node */

    p_tnode->p_right = p_tree->p_root;

    /* traverse and save the rebalance point */
    CDISPLAY("Incomplete...");


exception:
    return;
} /* tree_insert() */


/**
 **************************************************************************
 * \details
 *
 *************************************************************************
 */
int32_t
adts_tree_insert( adts_tree_t      *p_adts_tree,
                  adts_tree_node_t *p_adts_tree_node,
                  void             *p_data,
                  size_t            bytes )
{
    assert(p_adts_tree);
    assert(p_adts_tree_node);

    int32_t        rc       = 0;
    tree_t        *p_tree   = (tree_t *) p_adts_tree;
    tree_node_t   *p_node   = (tree_node_t *) p_adts_tree_node;
    adts_sanity_t *p_sanity = &(p_tree->sanity);

    adts_sanity_entry(p_sanity);

    p_node->p_tree = p_tree;
    p_node->p_data = p_data;
    p_node->bytes  = bytes;

    if (NULL == p_tree->p_root) {
        /* First entry */
        p_tree->p_root = p_node;
    }else {
        /* Not first entry */
        tree_insert(p_tree, p_node);
    }

    p_tree->elems_curr++;
    p_tree->elems_max = MAX(p_tree->elems_max, p_tree->elems_curr);

    adts_sanity_exit(p_sanity);
    return rc;
} /* adts_tree_insert() */


/**
 **************************************************************************
 * \details
 *
 *************************************************************************
 */
void
adts_tree_destroy( adts_tree_t *p_adts_tree )
{
    tree_t        *p_tree   = (tree_t *) p_adts_tree;
    adts_sanity_t *p_sanity = &(p_tree->sanity);

    adts_sanity_entry(p_sanity);

    free(p_tree);

    /* No adts_sanity_exit() since we've freed the memory */

    return;
} /* adts_tree_destroy() */


/**
 **************************************************************************
 * \details
 *
 *************************************************************************
 */
adts_tree_t *
adts_tree_create( adts_tree_type_t  type )
{
    tree_t      *p_tree      = NULL;
    adts_tree_t *p_adts_tree = NULL;

    p_adts_tree = calloc(1, sizeof(*p_adts_tree));
    if (NULL == p_adts_tree) {
        goto exception;
    }

    p_tree       = (tree_t *) p_adts_tree;
    p_tree->type = type;

exception:
    return p_adts_tree;
} /* adts_tree_create() */







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
utest_tree_bytes( void )
{

    CDISPLAY("[%u]", sizeof(tree_t));
    CDISPLAY("[%u]", sizeof(adts_tree_t));

    _Static_assert(sizeof(tree_t) < sizeof(adts_tree_t),
        "Mismatch structs detected");

    CDISPLAY("[%u]", sizeof(tree_node_t));
    CDISPLAY("[%u]", sizeof(adts_tree_node_t));

    _Static_assert(sizeof(tree_node_t) < sizeof(adts_tree_node_t),
        "Mismatch structs detected");

    return;
} /* utest_tree_bytes() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
static tree_node_t *
utest_tree_generate_node( void *p_data )
{
    tree_node_t *p_node = NULL;

    p_node = calloc(1, sizeof(*p_node));
    if (NULL == p_node) {
        goto exception;
    }

    p_node->p_data = p_data;

exception:
    return p_node;
} /* utest_tree_generate_node() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
static adts_tree_node_t *
utest_tree_generate_ascii( void )
{
    tree_node_t *p_root = NULL;

    p_root                            = utest_tree_generate_node('F');
    p_root->p_left                    = utest_tree_generate_node('B');

    p_root->p_left->p_left            = utest_tree_generate_node('A');
    p_root->p_left->p_right           = utest_tree_generate_node('D');

    p_root->p_left->p_right->p_left   = utest_tree_generate_node('C');
    p_root->p_left->p_right->p_right  = utest_tree_generate_node('E');

    p_root->p_right                   = utest_tree_generate_node('G');
    p_root->p_right->p_right          = utest_tree_generate_node('I');

    p_root->p_right->p_right->p_left  = utest_tree_generate_node('H');

    return (adts_tree_node_t *) p_root;
} /* utest_tree_generate_ascii() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
static adts_tree_node_t *
utest_tree_generate_number( void )
{
    tree_node_t *p_root = NULL;

    p_root                            = utest_tree_generate_node(6);
    p_root->p_left                    = utest_tree_generate_node(2);

    p_root->p_left->p_left            = utest_tree_generate_node(1);
    p_root->p_left->p_right           = utest_tree_generate_node(4);

    p_root->p_left->p_right->p_left   = utest_tree_generate_node(3);
    p_root->p_left->p_right->p_right  = utest_tree_generate_node(5);

    p_root->p_right                   = utest_tree_generate_node(7);
    p_root->p_right->p_right          = utest_tree_generate_node(9);

    p_root->p_right->p_right->p_left  = utest_tree_generate_node(8);

    return (adts_tree_node_t *) p_root;
} /* utest_tree_generate_number() */


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
        CDISPLAY("Test 1: size verification ");
        utest_tree_bytes();
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test 2: create -> destroy ");

        adts_tree_t      *p_tree = NULL;
        adts_tree_type_t  type   = ADTS_TREE_AVL;

        p_tree = adts_tree_create(type);
        assert(p_tree);
        adts_tree_destroy(p_tree);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test 3: preorder traversal ");

        adts_tree_t      *p_tree = NULL;
        adts_tree_type_t  type   = ADTS_TREE_AVL;
        adts_tree_node_t *p_node = NULL;

        p_tree = adts_tree_create(type);
        assert(p_tree);

        p_node = utest_tree_generate_ascii();
        adts_tree_display_preorder(p_node);

        adts_tree_destroy(p_tree);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test 4: inorder traversal ");

        adts_tree_t      *p_tree = NULL;
        adts_tree_type_t  type   = ADTS_TREE_AVL;
        adts_tree_node_t *p_node = NULL;

        p_tree = adts_tree_create(type);
        assert(p_tree);

        p_node = utest_tree_generate_ascii();
        adts_tree_display_inorder(p_node);

        adts_tree_destroy(p_tree);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test 5: postorder traversal ");

        adts_tree_t      *p_tree = NULL;
        adts_tree_type_t  type   = ADTS_TREE_AVL;
        adts_tree_node_t *p_node = NULL;

        p_tree = adts_tree_create(type);
        assert(p_tree);

        p_node = utest_tree_generate_ascii();
        adts_tree_display_postorder(p_node);

        adts_tree_destroy(p_tree);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test 6: level traversal ");

        adts_tree_t      *p_tree = NULL;
        adts_tree_type_t  type   = ADTS_TREE_AVL;
        adts_tree_node_t *p_node = NULL;

        p_tree = adts_tree_create(type);
        assert(p_tree);

        p_node = utest_tree_generate_ascii();
        adts_tree_display_level(p_node);

        adts_tree_destroy(p_tree);
    }
return;
    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test 7: bst is valid ");

        int32_t           rc     = 0;
        adts_tree_t      *p_tree = NULL;
        adts_tree_type_t  type   = ADTS_TREE_AVL;
        adts_tree_node_t *p_node = NULL;

        p_tree = adts_tree_create(type);
        assert(p_tree);

        p_node = utest_tree_generate_ascii();

        tree_t *p_tmp = (tree_t *) p_tree;
        p_tmp->p_root = (tree_node_t *) p_node;

        rc = adts_tree_bst_valid(p_tree);
        assert(rc);

        adts_tree_destroy(p_tree);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test 8: bst is invalid ");

        int32_t           rc     = 0;
        adts_tree_t      *p_tree = NULL;
        adts_tree_type_t  type   = ADTS_TREE_AVL;
        adts_tree_node_t *p_node = NULL;

        p_tree = adts_tree_create(type);
        assert(p_tree);

        p_node = utest_tree_generate_ascii();

        tree_t *p_tmp = (tree_t *) p_tree;
        p_tmp->p_root = (tree_node_t *) p_node;
        p_tmp->p_root->p_data = 'X';

        rc = adts_tree_bst_invalid(p_tree);
        assert(rc);

        adts_tree_destroy(p_tree);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test 9: tree insert");

        int32_t           rc     = 0;
        adts_tree_t      *p_tree = NULL;
        adts_tree_type_t  type   = ADTS_TREE_AVL;

        #define UTEST_ELEMS (32)
        int32_t           key[] = {'A','B','C','D','E','F','G','H','I'};
        adts_tree_node_t  node[ UTEST_ELEMS ] = {0};
        int32_t           elems = 1;

        p_tree = adts_tree_create(type);
        assert(p_tree);

        for (int32_t idx = 0; idx < elems; idx++) {
            rc = adts_tree_insert(p_tree,
                    &(node[idx]), key[idx], sizeof(key[idx]));
            assert(0 == rc);
            adts_tree_display_inorder(&(node[idx]));
        }

        adts_tree_destroy(p_tree);
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
utest_adts_tree( void )
{
    utest_control();

    return;
} /* utest_adts_tree() */
