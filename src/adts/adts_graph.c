


#include <errno.h>
#include <assert.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include <inttypes.h>

/* Toolbox */
#include <adts_graph.h>
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


typedef struct {
    size_t  nsucc;
    size_t  list_elems;
    bool    list_sorted;
    size_t *p_list;
} graph_node_t;


typedef struct {
    size_t         vertices;
    size_t         edges;    /* also known as arcs */
    graph_node_t **adjlist;
    adts_sanity_t  sanity;
} graph_t;





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
int32_t
adts_graph_add_edge( adts_graph_t      *p_adts_graph,
                     adts_graph_node_t *p_adts_src,
                     adts_graph_node_t *p_adts_dest )
{
    int32_t        rc       = 0;
    graph_t       *p_graph  = (graph_t *) p_adts_graph;
    adts_sanity_t *p_sanity = &(p_graph->sanity);

    adts_sanity_entry(p_sanity);

    /* validate inputs */




exception:
    adts_sanity_exit(p_sanity);
    return rc;
} /* adts_graph_add_edge() */



/*
 ****************************************************************************
 *
 ****************************************************************************
 */
void
adts_graph_destroy( adts_graph_t *p_adts_graph )
{
    graph_t       *p_graph  = (graph_t *) p_adts_graph;
    adts_sanity_t *p_sanity = &(p_graph->sanity);

    adts_sanity_entry(p_sanity);

    for (int32_t i = 0; i < p_graph->vertices; i++) {
        free(p_graph->adjlist[i]);
        p_graph->adjlist[i] = NULL;
    }

    free(p_graph->adjlist);
    free(p_graph);

    /* No adts_sanity_exit() since we've freed the memory */

    return;
} /* adts_graph_destroy() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
adts_graph_t *
adts_graph_create( size_t vertices )
{
    bool               valid_nodes       = false;
    size_t             bytes             = 0;
    int32_t            rc                = 0;
    int32_t            idx               = 0;
    graph_t           *p_graph           = NULL;
    adts_graph_t      *p_adts_graph      = NULL;
    adts_graph_node_t *p_adts_graph_node = NULL;

    p_adts_graph  = adts_mem_zalloc(sizeof(*p_adts_graph));
    if (NULL == p_adts_graph) {
        rc = ENOMEM;
        goto exception;
    }

    p_graph           = (graph_t *) p_adts_graph;
    p_graph->vertices = vertices;

    bytes = sizeof(p_adts_graph_node) * vertices;
    p_graph->adjlist = calloc(1, bytes);
    if (NULL == p_graph->adjlist) {
        rc = ENOMEM;
        goto exception;
    }

    for (idx = 0; idx < vertices; idx++) {
        graph_node_t *p_node = NULL;

        p_node = calloc(1, sizeof(*p_adts_graph_node));
        if (NULL == p_node) {
            rc = ENOMEM;
            goto exception;
        }

        p_node->nsucc       = 0;
        p_node->list_elems  = 1;
        p_node->list_sorted = 1;

        valid_nodes           = true;
        p_graph->adjlist[idx] = p_node;
    }

exception:
    if (rc) {
        if (valid_nodes) {
            for (int32_t i = idx - 1; i >= 0; i--) {
                free(p_graph->adjlist[i]);
            }
        }

        if (p_graph) {
        	if (p_graph->adjlist) {
            	free(p_graph->adjlist);
				p_graph->adjlist = NULL;
        	}
            free(p_graph);
            p_graph = NULL;
        }
    }

    return p_graph;
} /* adts_graph_create() */




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
utest_graph_bytes( void )
{

    CDISPLAY("[%u]", sizeof(graph_t));
    CDISPLAY("[%u]", sizeof(adts_graph_t));

    _Static_assert(sizeof(graph_t) <= sizeof(adts_graph_t),
        "Mismatch structs detected");

    CDISPLAY("[%u]", sizeof(graph_node_t));
    CDISPLAY("[%u]", sizeof(adts_graph_node_t));

    _Static_assert(sizeof(graph_node_t) <= sizeof(adts_graph_node_t),
        "Mismatch structs detected");

    return;
} /* utest_graph_bytes() */


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
        CDISPLAY("Test: bytes ");
        utest_graph_bytes();
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: create -> destroy ");
        size_t        vertices = 16;
        adts_graph_t *p_graph  = NULL;

        p_graph = adts_graph_create(vertices);
        assert(p_graph);
        adts_graph_destroy(p_graph);
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
utest_adts_graph( void )
{
    utest_control();

    return;
} /* utest_adts_graph() */
