

#include <errno.h>
#include <assert.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include <inttypes.h>

/* Toolbox */
#include <adts_rbt.h>
#include <adts_stack.h>
#include <adts_queue.h>
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
typedef enum {
    RED   = true,
    BLACK = false,
} rbt_color_t;

typedef struct {
    //size_t height;
    size_t children;
} rbt_stats_t;

typedef struct rbt_node_s {
    void               *key;
    void               *p_data;
    struct rbt_node_s  *p_left;
    struct rbt_node_s  *p_right;
    rbt_color_t         color;
    rbt_stats_t         stats;
} rbt_node_t;


/******************************************************************************
 * ####### #     # #     #  #####  #######   ###   ####### #     #  #####
 * #       #     # ##    # #     #    #       #    #     # ##    # #     #
 * #       #     # # #   # #          #       #    #     # # #   # #
 * #####   #     # #  #  # #          #       #    #     # #  #  #  #####
 * #       #     # #   # # #          #       #    #     # #   # #       #
 * #       #     # #    ## #     #    #       #    #     # #    ## #     #
 * #        #####  #     #  #####     #      ###   ####### #     #  #####
******************************************************************************/

// rbt_delete
// rbt_get_value
// rbt_key_present
// rbt_key_missing
// rbt_nodes
// rbt_peek_min
// rbt_peek_max
// rbt_delete_min
// rbt_delete_max
// rbt_floor
// rbt_cieling
// rbt_rank


/**
 **************************************************************************
 *
 *************************************************************************
 */
static inline void
rbt_node_display( rbt_node_t *p_node )
{
    printf("[%p]  data: %11p  key: %4c color: %8s  left: %11p  right: %11p\n",
            p_node,
            p_node->p_data,
            p_node->key,
            ((RED == p_node->color) ? "RED" : "BLACK"),
            p_node->p_left,
            p_node->p_right);

    return;
} /* rbt_node_display() */


/**
 **************************************************************************
 *
 *************************************************************************
 */
void
adts_rbt_display_preorder( adts_rbt_node_t *p_adts_rbt_node )
{
    assert(p_adts_rbt_node);

    rbt_node_t    *p_node   = (rbt_node_t *) p_adts_rbt_node;
    adts_stack_t  *p_stack  = adts_stack_create();

    while (p_node || adts_stack_is_not_empty(p_stack)) {
        if (p_node) {
            rbt_node_display(p_node);
            (void) adts_stack_push(p_stack, p_node, sizeof(*p_node));
            p_node = p_node->p_left;
        }else {
            p_node = (rbt_node_t *) adts_stack_pop(p_stack);
            p_node = p_node->p_right;
        }
    }

exception:
    if (p_stack) {
        adts_stack_destroy(p_stack);
    }

    return;
} /* adts_rbt_display_preorder() */


/**
 **************************************************************************
 *
 *************************************************************************
 */
void
adts_rbt_display_inorder( adts_rbt_node_t *p_adts_rbt_node )
{
    assert(p_adts_rbt_node);

    rbt_node_t    *p_node   = (rbt_node_t *) p_adts_rbt_node;
    adts_stack_t  *p_stack  = adts_stack_create();

    while (p_node || adts_stack_is_not_empty(p_stack)) {
        if (p_node) {
            (void) adts_stack_push(p_stack, p_node, sizeof(*p_node));
            p_node = p_node->p_left;
        }else {
            p_node = (rbt_node_t *) adts_stack_pop(p_stack);
            rbt_node_display(p_node);
            p_node = p_node->p_right;
        }
    }

exception:
    if (p_stack) {
        adts_stack_destroy(p_stack);
    }

    return;
} /* adts_rbt_display_inorder() */


/**
 **************************************************************************
 *
 *************************************************************************
 */
void
adts_rbt_display_postorder( adts_rbt_node_t *p_adts_rbt_node )
{
    assert(p_adts_rbt_node);

    rbt_node_t    *p_node   = (rbt_node_t *) p_adts_rbt_node;
    rbt_node_t    *p_last   = NULL;
    adts_stack_t  *p_stack  = adts_stack_create();

    while (p_node || adts_stack_is_not_empty(p_stack)) {
        if (p_node) {
            (void) adts_stack_push(p_stack, p_node, sizeof(*p_node));
            p_node = p_node->p_left;
        }else {
            rbt_node_t *p_peek = NULL;

            /* If last node processed was not p_right, then process p_right. */
            p_peek = (rbt_node_t *) adts_stack_peek(p_stack);
            if (p_peek->p_right && (p_peek->p_right != p_last)) {
                p_node = p_peek->p_right;
            }else {
                /* Pop and discard since we already have the node pointer via
                 * p_peek.  We cannot use p_node since ir has a predefined
                 * meaning in this loop. */
                (void) adts_stack_pop(p_stack);
                rbt_node_display(p_peek);
                p_last = p_peek;
            }
        }
    }

exception:
    if (p_stack) {
        adts_stack_destroy(p_stack);
    }

    return;
} /* adts_rbt_display_postorder() */


/**
 **************************************************************************
 *
 *************************************************************************
 */
void
adts_rbt_display_level( adts_rbt_node_t *p_adts_rbt_node )
{
    assert(p_adts_rbt_node);

    rbt_node_t    *p_node   = (rbt_node_t *) p_adts_rbt_node;
    adts_queue_t  *p_queue  = adts_queue_create();

    while (p_node) {

        rbt_node_display(p_node);

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

    return;
} /* adts_rbt_display_level() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
static bool
rbt_is_red( rbt_node_t *p_node )
{
    /* Ternary operand with dereferrence safety */
    return (p_node) ? (RED == p_node->color) : false;
} /* rbt_is_red() */

#if 0
/*
 ****************************************************************************
 *
 ****************************************************************************
 */
static bool
rbt_is_black( rbt_node_t *p_node )
{
    /* Ternary operand with dereferrence safety */
    return (p_node) ? (BLACK == p_node->color) : false;
} /* rbt_is_black() */
#endif

/*
 ****************************************************************************
 *
 ****************************************************************************
 */
static rbt_node_t *
rbt_rotate_left( rbt_node_t *p_root )
{
    /* Sanity */
    assert(rbt_is_red(p_root->p_right));

    rbt_node_t *p_tmp = NULL;

    /* Rotate */
    p_tmp           = p_root->p_right;
    p_root->p_right = p_tmp->p_left;
    p_tmp->p_left   = p_root;

    /* Adjust color */
    p_tmp->color  = p_root->color;
    p_root->color = RED;

    CDISPLAY("");
    return p_tmp;
} /* rbt_rotate_left() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
static rbt_node_t *
rbt_rotate_right( rbt_node_t *p_root )
{
    /* Sanity */
    assert(rbt_is_red(p_root->p_left));

    rbt_node_t *p_tmp = NULL;

    /* Rotate */
    p_tmp          = p_root->p_left;
    p_root->p_left = p_tmp->p_right;
    p_tmp->p_right = p_root;

    /* Adjust color */
    p_tmp->color  = p_root->color;
    p_root->color = RED;

    CDISPLAY("");
    return p_tmp;
} /* rbt_rotate_right() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
static void
rbt_color_change( rbt_node_t *p_root )
{
    assert(false == rbt_is_red(p_root));
    assert(rbt_is_red(p_root->p_left));
    assert(rbt_is_red(p_root->p_right));

    p_root->color          = RED;
    p_root->p_left->color  = BLACK;
    p_root->p_right->color = BLACK;

    return;
} /* rbt_color_change() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
static size_t
rbt_nodes( rbt_node_t *p_root )
{
    size_t nodes = 0;

    if (p_root) {
        nodes = p_root->stats.children;
    }

    return nodes;
} /* rbt_nodes() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
static void
rbt_delete( rbt_node_t *p_root,
            const void *key )
{
    assert(0);
    return;
} /* rbt_delete() */


/*
 ****************************************************************************
 *
 * \details
 *   This algorithm leverages recursion to obtain 0-cost staistics on
 *   child nodes.
 *
 ****************************************************************************
 */
static rbt_node_t *
rbt_insert( rbt_node_t  *p_root,
            const void  *key,
            const void  *p_data )
{
    rbt_stats_t *p_stats = NULL;

    if (NULL == p_root) {
        p_root = adts_mem_zalloc(sizeof(*p_root));
        if (NULL == p_root) {
            goto exception;
        }
        p_root->key    = key;
        p_root->p_data = p_data;
        CDISPLAY("%c", p_root->key);
	}else {
		CDISPLAY("%c", key);

		if (key < p_root->key) {
			CDISPLAY("");
			p_root->p_left = rbt_insert(p_root->p_left, key, p_data);
		}else if (key > p_root->key) {
			CDISPLAY("");
			p_root->p_right = rbt_insert(p_root->p_right, key, p_data);
		}else {
			CDISPLAY("");
			p_root->p_data = p_data;
		}

		if (rbt_is_red(p_root->p_right) && (false == rbt_is_red(p_root->p_left))) {
			CDISPLAY("");
			p_root = rbt_rotate_left(p_root);
		}

		if (rbt_is_red(p_root->p_left) && (false == rbt_is_red(p_root->p_left->p_left))) {
			CDISPLAY("");
			p_root = rbt_rotate_right(p_root);
		}

		if (rbt_is_red(p_root->p_left) && rbt_is_red(p_root->p_right)) {
			CDISPLAY("");
			rbt_color_change(p_root);
		}

		p_stats            = &(p_root->stats);
		p_stats->children  = 1; /* self */
		p_stats->children += rbt_nodes(p_root->p_left);
		p_stats->children += rbt_nodes(p_root->p_right);
	}

exception:
    return p_root;
} /* rbt_insert() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
static rbt_node_t *
rbt_find_node( rbt_node_t *p_root,
               const void *key )
{
    rbt_node_t *p_tmp   = p_root;
    rbt_node_t *p_match = NULL;

    while (p_tmp) {
        if (key < p_tmp->key) {
            p_tmp = p_tmp->p_left;
        }else if (key > p_tmp->key) {
            p_tmp = p_tmp->p_right;
        }else {
            p_match = p_tmp;
            break;
        }
    }

    return p_match;
} /* rbt_find_node() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
static rbt_node_t *
rbt_find_min( rbt_node_t *p_root )
{
    rbt_node_t *p_tmp = p_root;

    while (p_tmp) {
        if (NULL == p_tmp->p_left) {
            break;
        }
        p_tmp = p_tmp->p_left;
    }

    return p_tmp;
} /* rbt_find_min() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
static rbt_node_t *
rbt_find_max( rbt_node_t *p_root )
{
    rbt_node_t *p_tmp = p_root;

    while (p_tmp) {
        if (NULL == p_tmp->p_right) {
            break;
        }
        p_tmp = p_tmp->p_right;
    }

    return p_tmp;
} /* rbt_find_max() */


/*
 ****************************************************************************
 * \details
 *  Floor is defined as: "Largest Key <= Input Key"
 *
 ****************************************************************************
 */
static rbt_node_t *
rbt_find_floor( rbt_node_t *p_root,
                const void *key )
{
    rbt_node_t *p_tmp = NULL;

    if (NULL == p_root) {
        goto exception;
    }

    if (key == p_root->key) {
        p_tmp = p_root;
    }else if (key < p_root->key) {
        p_tmp = rbt_find_floor(p_root->p_left, key);
    }else {
        rbt_node_t *temp = NULL;

        temp  = rbt_find_floor(p_root->p_right, key);
        p_tmp = (temp) ? temp : p_root;
    }

exception:
    return p_tmp;
} /* rbt_find_floor() */


/*
 ****************************************************************************
 * \details
 *  Ceiling is defined as: "Smallest Key >= Input Key"
 *
 ****************************************************************************
 */
static rbt_node_t *
rbt_find_ceiling( rbt_node_t *p_root,
                  const void *key )
{
    rbt_node_t *p_tmp = NULL;

    if (NULL == p_root) {
        goto exception;
    }

    if (key == p_root->key) {
        p_tmp = p_root;
    }else if (key > p_root->key) {
        p_tmp = rbt_find_ceiling(p_root->p_right, key);
    }else {
        rbt_node_t *temp = NULL;

        temp  = rbt_find_ceiling(p_root->p_left, key);
        p_tmp = (temp) ? temp : p_root;
    }

exception:
    return p_tmp;
} /* rbt_find_ceiling() */





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
utest_rbt_bytes( void )
{

    CDISPLAY("[%u]", sizeof(rbt_node_t));
    CDISPLAY("[%u]", sizeof(adts_rbt_node_t));

    _Static_assert(sizeof(rbt_node_t) <= sizeof(adts_rbt_node_t),
        "Mismatch structs detected");

    return;
} /* utest_rbt_bytes() */



/*
 ****************************************************************************
 *
 ****************************************************************************
 */
static rbt_node_t *
utest_rbt_generate_node( void *key,
                         void *p_data )
{
    rbt_node_t *p_node = NULL;

    p_node = adts_mem_zalloc(sizeof(*p_node));
    if (NULL == p_node) {
        goto exception;
    }

    p_node->key    = key;
    p_node->p_data = p_data;

exception:
    return p_node;
} /* utest_rbt_generate_node() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
static adts_rbt_node_t *
utest_rbt_generate_ascii( void )
{
    rbt_node_t *p_root = NULL;

    p_root                            = utest_rbt_generate_node('F', -1);
    p_root->p_left                    = utest_rbt_generate_node('B', -1);

    p_root->p_left->p_left            = utest_rbt_generate_node('A', -1);
    p_root->p_left->p_right           = utest_rbt_generate_node('D', -1);

    p_root->p_left->p_right->p_left   = utest_rbt_generate_node('C', -1);
    p_root->p_left->p_right->p_right  = utest_rbt_generate_node('E', -1);

    p_root->p_right                   = utest_rbt_generate_node('G', -1);
    p_root->p_right->p_right          = utest_rbt_generate_node('I', -1);

    p_root->p_right->p_right->p_left  = utest_rbt_generate_node('H', -1);

    return (adts_rbt_node_t *) p_root;
} /* utest_rbt_generate_ascii() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
static adts_rbt_node_t *
utest_rbt_generate_number( void )
{
    rbt_node_t *p_root = NULL;

    p_root                            = utest_rbt_generate_node(6, -1);
    p_root->p_left                    = utest_rbt_generate_node(2, -1);

    p_root->p_left->p_left            = utest_rbt_generate_node(1, -1);
    p_root->p_left->p_right           = utest_rbt_generate_node(4, -1);

    p_root->p_left->p_right->p_left   = utest_rbt_generate_node(3, -1);
    p_root->p_left->p_right->p_right  = utest_rbt_generate_node(5, -1);

    p_root->p_right                   = utest_rbt_generate_node(7, -1);
    p_root->p_right->p_right          = utest_rbt_generate_node(9, -1);

    p_root->p_right->p_right->p_left  = utest_rbt_generate_node(8, -1);

    return (adts_rbt_node_t *) p_root;
} /* utest_rbt_generate_number() */


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
        CDISPLAY("Test 2: size verification ");
        utest_rbt_bytes();
    }
#if 0
    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test 3: preorder traversal ");

        adts_rbt_node_t *p_node = NULL;

        p_node = utest_rbt_generate_ascii();
        adts_rbt_display_preorder(p_node);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test 4: inorder traversal ");

        adts_rbt_node_t *p_node = NULL;

        p_node = utest_rbt_generate_ascii();
        adts_rbt_display_inorder(p_node);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test 5: postorder traversal ");

        adts_rbt_node_t *p_node = NULL;

        p_node = utest_rbt_generate_ascii();
        adts_rbt_display_postorder(p_node);
    }

#endif
    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test 6: level traversal ");

        adts_rbt_node_t *p_node = NULL;

        p_node = utest_rbt_generate_ascii();
        adts_rbt_display_level(p_node);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test 7: formal generation ");
        char        keys[]     = {'A','B','C','D','E','F','G','H','I'};
        size_t      elems      = sizeof(keys) / sizeof(keys[0]);
        rbt_node_t *p_tmp      = NULL;
        rbt_node_t *p_node[32] = {0};

        elems = 3;

        for (int32_t idx = 0; idx < elems; idx++) {
            p_node[idx] = rbt_insert(p_tmp, keys[idx], -1);
            p_tmp = p_node[idx];

            printf("\n");
            //adts_rbt_display_level(p_node[idx]);
        }
        //p_tmp = p_node[0];
        adts_rbt_display_level(p_tmp);
        //adts_rbt_display_preorder(p_tmp);
        //adts_rbt_display_inorder(p_tmp);
        //adts_rbt_display_postorder(p_tmp);
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
utest_adts_rbt( void )
{
    utest_control();

    return;
} /* utest_adts_rbt() */


