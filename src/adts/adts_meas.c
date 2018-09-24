

#include <errno.h>
#include <assert.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include <inttypes.h>

/* Toolbox */
#include <adts_meas.h>
#include <adts_sanity.h>
#include <adts_private.h>
#include <adts_display.h>
#include <adts_hexdump.h>


/******************************************************************************
 #####  ####### ######  #     #  #####  ####### #     # ######  #######  #####
#     #    #    #     # #     # #     #    #    #     # #     # #       #     #
#          #    #     # #     # #          #    #     # #     # #       #
 #####     #    ######  #     # #          #    #     # ######  #####    #####
      #    #    #   #   #     # #          #    #     # #   #   #             #
#     #    #    #    #  #     # #     #    #    #     # #    #  #       #     #
 #####     #    #     #  #####   #####     #     #####  #     # #######  #####
******************************************************************************/
#define EYEC4(_a, _b, _c, _d) \
   (((_a) <<  0) | \
    ((_b) <<  8) | \
    ((_c) << 16) | \
    ((_d) << 24))

typedef enum {
    MEAS_ALLOC = EYEC4('U','S','E','!'),
    MEAS_FREE  = EYEC4('f','r','e','e'),
} meas_state_t;

typedef struct {
    uint64_t val; // Measured value
} meas_entry_t;

typedef struct {
    meas_state_t  state;     // access safety
    adts_sanity_t sanity;    // concurrency detection
    uint32_t      curr;      // current index ready for use
    uint32_t      entries;   // data elements
    uint64_t      total;     // lifetime number of measurements
    uint64_t      min;       // lifetime min value
    uint64_t      max;       // lifetime max value
    meas_entry_t *entry;
} meas_t;


typedef struct {
    uint64_t min;
    uint64_t max;
    uint64_t total;
    uint32_t entries;
    uint64_t entry[];
} meas_public_t;

/******************************************************************************
 * ####### #     # #     #  #####  #######   ###   ####### #     #  #####
 * #       #     # ##    # #     #    #       #    #     # ##    # #     #
 * #       #     # # #   # #          #       #    #     # # #   # #
 * #####   #     # #  #  # #          #       #    #     # #  #  #  #####
 * #       #     # #   # # #          #       #    #     # #   # #       #
 * #       #     # #    ## #     #    #       #    #     # #    ## #     #
 * #        #####  #     #  #####     #      ###   ####### #     #  #####
******************************************************************************/
static inline int32_t
meas_input_sanity( meas_t *p_meas )
{
    uint32_t err = false;

    if (NULL == p_meas) {
        err = ENOMEM;
        goto exception;
    }

    if (MEAS_ALLOC != p_meas->state) {
        err = EINVAL;
        goto exception;
    }

exception:
    return err;
} /* meas_input_sanity() */




static inline void
meas_add( meas_t  *p_meas,
          uint64_t value )
{
    p_meas->entry[p_meas->curr].val = value;
    p_meas->curr++;
    if (p_meas->entries <= p_meas->curr) {
        p_meas->curr = 0;
    }

    p_meas->total++;
    p_meas->min = MIN(p_meas->min, p_meas->entry[p_meas->curr].val);
    p_meas->min = MAX(p_meas->max, p_meas->entry[p_meas->curr].val);

    return;
} /* meas_add() */




int32_t
adts_meas_add( adts_meas_t *p_adts_meas,
               uint64_t     value )
{
    int32_t        rc       = 0;
    meas_t        *p_meas   = (meas_t *) p_adts_meas;
    adts_sanity_t *p_sanity = NULL;

    rc = meas_input_sanity(p_meas);
    if (rc) {
        goto exception;
    }

    p_sanity = &(p_meas->sanity);
    adts_sanity_entry(p_sanity);

    meas_add(p_meas, value);

    adts_sanity_exit(p_sanity);

exception:
    return rc;
} /* adts_meas_add() */




int32_t
adts_meas_destroy( adts_meas_t *p_adts_meas )
{
    int32_t        rc       = 0;
    meas_t        *p_meas   = (meas_t *) p_adts_meas;
    adts_sanity_t *p_sanity = NULL;

    rc = meas_input_sanity(p_meas);
    if (rc) {
        goto exception;
    }

    p_sanity = &(p_meas->sanity);
    adts_sanity_entry(p_sanity);

    p_meas->state = MEAS_FREE;

    free(p_meas);

exception:
    /* No adts_sanity_exit() since we've freed the memory */
    return rc;
} /* adts_meas_destroy() */




adts_meas_t *
adts_meas_create( uint32_t elems )
{
    uint32_t     bytes       = 0;
    meas_t      *p_meas      = 0;
    adts_meas_t *p_adts_meas = NULL;

    if (elems < ADTS_MEAS_MIN_ENTRIES) {
        goto exception;
    }

    bytes  = sizeof(meas_t);
    bytes += (elems * sizeof(meas_entry_t));

    p_meas = adts_mem_zalloc(bytes);
    if (NULL == p_meas) {
        goto exception;
    }

    p_meas->state    = MEAS_ALLOC;
    p_meas->entries  = elems;
    p_meas->entry    = (char *) p_meas + sizeof(*p_meas);
    p_adts_meas      = (adts_meas_t *) p_meas;

exception:
    return p_adts_meas;
} /* adts_meas_create() */



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
utest_meas_bytes( void )
{
    CDISPLAY("[%u]", sizeof(meas_t));
    CDISPLAY("[%u]", sizeof(adts_meas_t));

    _Static_assert(sizeof(meas_t) <= sizeof(adts_meas_t),
        "Mismatch structs detected");

    return;
} /* utest_meas_bytes() */


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
        utest_meas_bytes();
    }

    CDISPLAY("=========================================================");
    {
        //TEST: insufficient elements.
        uint32_t     elems       = 0;
        adts_meas_t *p_adts_meas = NULL;

        p_adts_meas = adts_meas_create(elems);
        assert(NULL == p_adts_meas);
    }

    CDISPLAY("=========================================================");
    {
        //TEST: Good Create
        adts_meas_t *p_adts_meas = NULL;

        p_adts_meas = adts_meas_create(1024);
        adts_meas_destroy(p_adts_meas);
    }

    CDISPLAY("=========================================================");
    {
        //TEST: visual dump
        uint32_t     elems       = 8;
        uint32_t     bytes       = 0;
        adts_meas_t *p_adts_meas = NULL;

        p_adts_meas = adts_meas_create(elems);


        bytes  =  sizeof(*p_adts_meas);
        bytes += (sizeof(meas_entry_t) * elems);
        adts_hexdump(p_adts_meas, bytes, "MEAS");
    }


    CDISPLAY("=========================================================");
    {
        //TEST: visual dump amd iterate
        uint32_t     elems       = 32;
        uint32_t     bytes       = 0;
        adts_meas_t *p_adts_meas = NULL;

        p_adts_meas = adts_meas_create(elems);

        for (uint64_t cnt = 0; cnt < elems + 2; cnt++) {
            (void) adts_meas_add(p_adts_meas, cnt);
        }

        bytes  =  sizeof(*p_adts_meas);
        bytes += (sizeof(meas_entry_t) * elems);
        adts_hexdump(p_adts_meas, bytes, "MEAS");
    }

#if 0

    CDISPLAY("=========================================================");
    {
        uint32_t     elems       = 1024;
        adts_meas_t *p_adts_meas = NULL;

        p_adts_meas = adts_meas_create(elems);
        //adts_meas_enmeas(p_meas, -1, 0x11223344);
        //adts_meas_display(p_meas);
        //(void) adts_meas_demeas(p_meas);
        adts_meas_destroy(p_adts_meas);
    }
#endif
    return;
} /* utest_control() */



/*
 ****************************************************************************
 * test entrypoint
 *
 ****************************************************************************
 */
void
utest_adts_meas( void )
{
    utest_control();

    return;
} /* utest_adts_meas() */
