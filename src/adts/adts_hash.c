

#include <errno.h>
#include <assert.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include <inttypes.h>

/* Toolbox */
#include <adts_hash.h>
#include <adts_math.h>
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
//#define HASH_NODE_VALID (0x1122334455667788llu)

/*
 ****************************************************************************
 * \FIXME:
 *   On resize the collisions must be rehashed.  Investigate how to implement
 ****************************************************************************
 */
typedef struct hash_node_s {
    bool                valid;    /**< Entry validity */
    void               *p_data;   /**< consumer datapointer */
    size_t              bytes;    /**< data bytes for  p_data */
    int64_t             key;      /**< Key used for hash */
    struct hash_node_s *p_prev;   /**< collision management */
    struct hash_node_s *p_next;   /**< collision management */
} hash_node_t;


/*
 ****************************************************************************
 * \details
 *   The hash workspace is an array of pointers, thus reserve as:
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
//FIXME: The number of entries in a hash table should be a prime number
//       investigate the following:
//         1) how to get the largest prime number from the selected pagesize
//         2) How to grow / shrink by approximate pow2 but rounded to prev
//            highest prime fitting in the pow2 space
#define HASH_DEFAULT_ELEMS (4096 / sizeof(void *))


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
typedef struct {
    size_t            elems_curr;
    size_t            elems_limit;
    hash_node_t     **workspace;
    adts_sanity_t     sanity;
} hash_t;





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
adts_hash_is_empty( adts_hash_t *p_adts_hash )
{
    hash_t *p_hash = (hash_t *) p_adts_hash;

    return (0 >= p_hash->elems_curr);
} /* adts_hash_is_empty() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
bool
adts_hash_is_not_empty( adts_hash_t *p_adts_hash )
{
    return !(adts_hash_is_empty(p_adts_hash));
} /* adts_hash_is_not_empty() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
size_t
adts_hash_entries( adts_hash_t *p_adts_hash )
{
    hash_t *p_hash = (hash_t *) p_adts_hash;

    return p_hash->elems_curr;
} /* adts_hash_entries() */



// hash_resize                   // use prime number elems
// hash_resize_shrink_candidate  // use prime number elems
//
// adts_hash_display
// adts_hash_remove
// adts_hash_insert
//
// adts_hash_destroy
// adts_hash_create  //prime number of elems




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
utest_hash_bytes( void )
{

    CDISPLAY("[%u]", sizeof(hash_t));
    CDISPLAY("[%u]", sizeof(adts_hash_t));

    _Static_assert(sizeof(hash_t) < sizeof(adts_hash_t),
        "Mismatch structs detected");

    return;
} /* utest_hash_bytes() */


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
utest_adts_hash( void )
{
    utest_control();

    return;
} /* utest_adts_hash() */
