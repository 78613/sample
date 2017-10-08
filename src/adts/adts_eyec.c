


#include <ctype.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include <inttypes.h>

/* Toolbox */
#include <adts_eyec.h>
#include <adts_private.h>
#include <adts_hexdump.h>
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

#define EYEC_BMARK ("ADTS")
#define EYEC_EMARK ("adts")


typedef struct {
    uint32_t safety;
    union {
        uint8_t  desc[sizeof(uint32_t)];
        uint32_t description;
    };
} eyec_t;


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
adts_eyec_invalid( adts_eyec_t *p_adts_bmark,
                   adts_eyec_t *p_adts_emark )
{
    bool    rc      = false;
    eyec_t *p_bmark = (eyec_t *) p_adts_bmark;
    eyec_t *p_emark = (eyec_t *) p_adts_emark;

    rc |= strncmp(EYEC_BMARK, p_bmark->safety, sizeof(p_bmark->safety));
    rc |= strncmp(EYEC_EMARK, p_emark->safety, sizeof(p_emark->safety));
    return rc;
} /* adts_eyec_invalid() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
bool
adts_eyec_valid( adts_eyec_t *p_adts_bmark,
                 adts_eyec_t *p_adts_emark )
{
    return !(adts_eyec_valid(p_adts_bmark, p_adts_emark));
} /* adts_eyec_valid() */


/*
 ****************************************************************************
 * Initialize the beggining and end markers in a structure.
 ****************************************************************************
 */
int32_t
adts_eyec_create( adts_eyec_t *p_adts_bmark,
                  adts_eyec_t *p_adts_emark,
                  char        *p_str )
{
    /* Sanity */
    assert(p_str);

    eyec_t   *p_bmark = (eyec_t *) p_adts_bmark;
    eyec_t   *p_emark = (eyec_t *) p_adts_emark;
    int32_t   rc      = 0;
    size_t    dbytes  = sizeof(p_bmark->description);
    size_t    sbytes  = strnlen(p_str, dbytes);
    size_t    bytes   = MIN(dbytes, sbytes);

    memset(p_adts_bmark, 0, sizeof(*p_adts_bmark));
    memset(p_adts_emark, 0, sizeof(*p_adts_emark));

    /* Though O(2n) cost, it's statistically insignificant when we consider the
     * input size vs. code complexity.  If this were a string larger than a
     * cacheline then the optimization for O(n) would be highly desired. */
    if (dbytes < sbytes) {
        rc = EINVAL;
        goto exception;
    }

    strncpy(&(p_bmark->safety), EYEC_BMARK, sizeof(p_bmark->safety));
    strncpy(&(p_emark->safety), EYEC_EMARK, sizeof(p_emark->safety));

    for (int32_t cnt = 0; cnt < bytes; cnt++) {
        p_bmark->desc[cnt] = toupper(p_str[cnt]);
        p_emark->desc[cnt] = tolower(p_str[cnt]);
    }

exception:
    return rc;
} /* adts_eyec_create() */



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
utest_eyec_bytes( void )
{

    CDISPLAY("[%u]", sizeof(eyec_t));
    CDISPLAY("[%u]", sizeof(adts_eyec_t));

    _Static_assert(sizeof(eyec_t) <= sizeof(adts_eyec_t),
        "Mismatch structs detected");

    return;
} /* utest_eyec_bytes() */




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
        utest_eyec_bytes();
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test 2: ");
        CDISPLAY("val = %u", (127-32));
        CDISPLAY("val = %u", (' '));
        CDISPLAY("val = %u", ('~'));
        CDISPLAY("val = %u", ('\0'));
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test 3: Display");

        typedef struct {
            adts_eyec_t bmark;
            uint64_t    buffer[16];
            adts_eyec_t emark;
            uint64_t    tail[1];
        } test3_t;

        test3_t  tmp   = {0};
        test3_t *p_tmp = &(tmp);

        (void) adts_eyec_create(&(p_tmp->bmark), &(p_tmp->emark), "AbCd");
        adts_hexdump(p_tmp, sizeof(*p_tmp), "tmp");
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test 4: small input sanity");

        typedef struct {
            adts_eyec_t bmark;
            uint64_t    buffer[16];
            adts_eyec_t emark;
            uint64_t    tail[1];
        } test3_t;

        test3_t  tmp   = {0};
        test3_t *p_tmp = &(tmp);

        (void) adts_eyec_create(&(p_tmp->bmark), &(p_tmp->emark), "Ab");
        adts_hexdump(p_tmp, sizeof(*p_tmp), "tmp");
    }


    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test 5: large input sanity");

        typedef struct {
            adts_eyec_t bmark;
            uint64_t    buffer[16];
            adts_eyec_t emark;
            uint64_t    tail[1];
        } test3_t;

        test3_t  tmp   = {0};
        test3_t *p_tmp = &(tmp);

        (void) adts_eyec_create(&(p_tmp->bmark), &(p_tmp->emark), "AbCdEfG");
        adts_hexdump(p_tmp, sizeof(*p_tmp), "tmp");
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
utest_adts_eyec( void )
{
    utest_control();

    return;
} /* utest_adts_eyec() */

