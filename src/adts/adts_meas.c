

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
    //FIXME: consider adding a valid bit
    uint64_t value; // Measured value
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

#if 0
typedef struct {
    uint64_t min;
    uint64_t max;
    uint64_t total;
    uint32_t entries;
    uint64_t entry[];
} meas_public_t;
#endif

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
    p_meas->min = MIN(p_meas->min, value);
    p_meas->max = MAX(p_meas->max, value);

    p_meas->entry[p_meas->curr].value = value;
    p_meas->curr++;
    if (p_meas->curr >= p_meas->entries) {
        p_meas->curr = 0;
    }

    p_meas->total++;

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
    meas_t      *p_meas      = NULL;
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
    p_meas->min      = -1;
    p_meas->max      = 0;
    p_meas->entry    = (char *) p_meas + sizeof(*p_meas);
    p_adts_meas      = (adts_meas_t *) p_meas;

exception:
    return p_adts_meas;
} /* adts_meas_create() */




void
adts_meas_destroy_embedded( void    *p_handle )
{
    int32_t        rc       = 0;
    meas_t        *p_meas   = (meas_t *) p_handle;
    adts_sanity_t *p_sanity = NULL;

    rc = meas_input_sanity(p_meas);
    if (rc) {
        assert(0);
        goto exception;
    }

    p_sanity = &(p_meas->sanity);
    adts_sanity_entry(p_sanity);

    p_meas->state = MEAS_FREE;

exception:
    return;
} /* adts_meas_destroy_embedded() */



//
// This function uses the memory space passed in by the caller.  It can only
// fail via invalid caller input.
//   - The starting address must be 8B aligned.
//   - The size must be sufficient to allow for minimal history of eight (8)
//     entries.
//   - Explicitly avoid memory alloc/free operations for runtime determinism
//
// Inputs:
//    p_mem    - Starting memory address - Must be eight (8) byte aligned.
//    ibytes   - bytes from starting memory address
//    p_errout - Output error code
//
void *
adts_meas_create_embedded( int8_t   *p_mem,
                           uint32_t  ibytes,
                           int32_t  *p_errout )
{
    void     *p_handle  = NULL;
    meas_t   *p_meas    = NULL;
    uint32_t  elems     = 0;
    uint32_t  offset    = 0;
    uint32_t  required  = 0;
    uint32_t  remaining = ibytes;

    *p_errout = 0;

    /* 64bit alignment required. */
    offset = (uint64_t) p_mem & 0x7;
    if (offset) {
        assert(0);
        *p_errout = 5;
        goto exception;
    }

    /* Ensure we have sufficient workspace */
    required  = sizeof(*p_meas);
    required += sizeof(meas_entry_t) * ADTS_MEAS_MIN_ENTRIES;
    if (required > remaining) {
        *p_errout = 10;
        goto exception;
    }

    /* Sanity */
    memset(p_mem, 0, ibytes);

    /* Derive elements from remaining space */
    elems = (remaining - sizeof(*p_meas)) / sizeof(meas_entry_t);

    p_meas           = (meas_t *) p_mem;
    p_meas->state    = MEAS_ALLOC;
    p_meas->entries  = elems;
    p_meas->min      = -1;
    p_meas->max      = 0;
    p_meas->entry    = (char *) p_meas + sizeof(*p_meas);

    p_handle         = (adts_meas_t *) p_meas;

exception:
    return p_handle;
} /* adts_meas_create_embedded() */




void
meas_display( void *p_mem )
{
    meas_t *p_meas = (meas_t *) p_mem;

    CDISPLAY("p_meas             = %p", p_meas);
    CDISPLAY("p_meas->state      = %p", p_meas->state);
    CDISPLAY("p_meas->sanity     = %p", p_meas->sanity);
    CDISPLAY("p_meas->curr       = %u", p_meas->curr);
    CDISPLAY("p_meas->entries    = %u", p_meas->entries);
    CDISPLAY("p_meas->total      = %llu", p_meas->total);
    CDISPLAY("p_meas->min        = %llu", p_meas->min);
    CDISPLAY("p_meas->max        = %llu", p_meas->max);
    CDISPLAY("p_meas->entry      = %p", p_meas->entry);

    for (int32_t cnt = 0; cnt < p_meas->entries; cnt++) {
        CDISPLAY("p_meas->entry[%03llu].value = %llu",
                cnt, p_meas->entry[cnt].value);
    }

    return;
} /* meas_display() */


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
    char data[] = {1,4,5,7,3,4,8,5,3,5,9,7,5,2,6,5,9,8,6,6,22};

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
        uint32_t     elems       = 16;
        uint32_t     bytes       = 0;
        adts_meas_t *p_adts_meas = NULL;

        p_adts_meas = adts_meas_create(elems);
        for (uint64_t cnt = 0; cnt < sizeof(data); cnt++) {
            (void) adts_meas_add(p_adts_meas, data[cnt]);
        }

        bytes  =  sizeof(*p_adts_meas);
        bytes += (sizeof(meas_entry_t) * elems);
        adts_hexdump(p_adts_meas, bytes, "MEAS");
        meas_display(p_adts_meas);
    }

    CDISPLAY("=========================================================");
    {
        //TEST: good visual dump amd iterate - compare to above
        char         arr[176]    = {0};
        int32_t      err         = 0;
        uint32_t     bytes       = sizeof(arr);
        void        *p_adts_meas = NULL;

        p_adts_meas = adts_meas_create_embedded(arr, bytes, &err);
        for (uint64_t cnt = 0; cnt < sizeof(data); cnt++) {
            (void) adts_meas_add(p_adts_meas, data[cnt]);
        }

        adts_hexdump(p_adts_meas, bytes, "MEAS Embedded");
        meas_display(p_adts_meas);
    }

    CDISPLAY("=========================================================");
    {
        //TEST: good visual dump amd iterate - compare to above
        char         arr[256]    = {0};
        int32_t      err         = 0;
        uint32_t     bytes       = sizeof(arr);
        void        *p_adts_meas = NULL;

        p_adts_meas = adts_meas_create_embedded(arr, bytes, &err);

        for (uint64_t cnt = 0; cnt < sizeof(data); cnt++) {
            (void) adts_meas_add(p_adts_meas, data[cnt]);
        }

        adts_hexdump(p_adts_meas, bytes, "MEAS Embedded");
        meas_display(p_adts_meas);
    }

    CDISPLAY("=========================================================");
    {
        //TEST: good visual dump amd iterate - compare to above after destroy
        char         arr[256]    = {0};
        int32_t      err         = 0;
        uint32_t     bytes       = sizeof(arr);
        void        *p_adts_meas = NULL;

        p_adts_meas = adts_meas_create_embedded(arr, bytes, &err);
        for (uint64_t cnt = 0; cnt < sizeof(data); cnt++) {
            (void) adts_meas_add(p_adts_meas, data[cnt]);
        }
        adts_meas_destroy_embedded(p_adts_meas);
        adts_hexdump(p_adts_meas, bytes, "MEAS Embedded");
        meas_display(p_adts_meas);
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
utest_adts_meas( void )
{
    utest_control();

    return;
} /* utest_adts_meas() */
