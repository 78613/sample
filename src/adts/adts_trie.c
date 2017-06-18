

#include <errno.h>
#include <assert.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include <inttypes.h>

/* Toolbox */
#include <adts_trie.h>
#include <adts_private.h>
#include <adts_hexdump.h>
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


/* ASCII based implementation, thus allow only for the standard codes, 32-126
 * 32-126.  Refer to ASCII tables for details.
 *
 * Optionally support extended ASCII codes or full unicode.  Note that
 * said support will result in significant wasted space if unused. */
//#define TRIE_FANOUT ((uint8_t) ~0)
#define TRIE_FANOUT (127-32)


/**
 **************************************************************************
 *
 *************************************************************************
 */
typedef struct trie_node_s {
    bool                term;   /* Word terminator */
    int32_t             refcnt;
    struct trie_node_s *p_next[ TRIE_FANOUT ];
} trie_node_t;



/**
 **************************************************************************
 *
 *************************************************************************
 */
typedef struct trie_s {
    size_t            elems_curr;
    size_t            elems_max;
    adts_sanity_t     sanity;
    trie_node_t      *p_root;
} trie_t;



/******************************************************************************
 * ####### #     # #     #  #####  #######   ###   ####### #     #  #####
 * #       #     # ##    # #     #    #       #    #     # ##    # #     #
 * #       #     # # #   # #          #       #    #     # # #   # #
 * #####   #     # #  #  # #          #       #    #     # #  #  #  #####
 * #       #     # #   # # #          #       #    #     # #   # #       #
 * #       #     # #    ## #     #    #       #    #     # #    ## #     #
 * #        #####  #     #  #####     #      ###   ####### #     #  #####
******************************************************************************/


bool
trie_string_sanity( char *p_str )
{
    return ((' ' <= *p_str) && ('~' >= *p_str));
} /* trie_string_sanity() */



int32_t
trie_insert( trie_t  *p_trie,
             char    *p_str )
{
    char          *p_tmp    = p_str;
    int32_t        cnt      = 0;
    int32_t        rc       = 0;
    adts_sanity_t *p_sanity = &(p_trie->sanity);

    adts_sanity_entry(p_sanity);

#if 0
    while ('\0' != *p_tmp) {
        if (trie_string_sanity(p_tmp)) {
            rc = EINVAL;
            break;
        }

        if (NULL == (p_trie->p_root[p_str[*p_tmp - ' ']])) {

        }

        cnt++;
        p_tmp++;
    }
#endif


exception:
    if (rc) {
        while (cnt) {
            cnt--;
        }
    }

    adts_sanity_exit(p_sanity);
    return rc;
} /* trie_insert() */


/**
 **************************************************************************
 *
 *************************************************************************
 */
int32_t
adts_trie_insert( adts_trie_t  *p_adts_trie,
                  char         *p_str )
{
    return trie_insert((trie_t *) p_adts_trie, (trie_node_t *) p_str);
} /* adts_trie_insert() */


/**
 **************************************************************************
 *
 *************************************************************************
 */
void
adts_trie_initialize( adts_trie_t *p_adts_trie )
{
    trie_t  *p_trie = (trie_t *) p_adts_trie;

    memset(p_adts_trie, 0, sizeof(*p_adts_trie));


    return;
} /* adts_trie_initialize() */



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
utest_trie_bytes( void )
{

    CDISPLAY("[%u]", sizeof(trie_t));
    CDISPLAY("[%u]", sizeof(adts_trie_t));

    _Static_assert(sizeof(trie_t) <= sizeof(adts_trie_t),
        "Mismatch structs detected");

    CDISPLAY("[%u]", sizeof(trie_node_t));
    CDISPLAY("[%u]", sizeof(adts_trie_node_t));

    _Static_assert(sizeof(trie_node_t) <= sizeof(adts_trie_node_t),
        "Mismatch structs detected");

    return;
} /* utest_trie_bytes() */




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
        utest_trie_bytes();
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test 2: ");
        uint64_t val = ((uint8_t) -1);
        uint64_t obj = sizeof(struct trie_node_s);

        CDISPLAY("val = %u", val);
        CDISPLAY("val = %u", obj);
        CDISPLAY("val = %u", (127-32));
        CDISPLAY("val = %u", (' '));
        CDISPLAY("val = %u", ('~'));
        CDISPLAY("val = %u", ('\0'));
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test 3: ");
        adts_trie_t  adts_trie   = {0};
        adts_trie_t *p_adts_trie = &(adts_trie);

        adts_trie_initialize(p_adts_trie);
        adts_hexdump(p_adts_trie, sizeof(*p_adts_trie), "foobar");


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
utest_adts_trie( void )
{
    utest_control();

    return;
} /* utest_adts_trie() */
