
#include <time.h>  /* clock_gettime() */
#include <errno.h>
#include <string.h>
#include <inttypes.h>

/* Toolbox */
#include <adts_time.h>
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
 * \brief
 *
 * \detail
 *   Attempt to align the entries to cache affinity interval
 *
 ****************************************************************************
 */
#define TSTAMP_MESSAGE_BYTES (64)
#define TSTAMP_WORKSPACE_BYTES (128)
#define TSTAMP_ENTRIES (TSTAMP_WORKSPACE_BYTES / sizeof(uint64_t))

/*
 ****************************************************************************
 * \brief
 *
 * \detail
 *
 *
 ****************************************************************************
 */
typedef struct {
    uint64_t       mim;         /* Min lifetime exec time */
    uint64_t       max;         /* Max lifetime exec time */
    uint64_t       last;        /* Most recent exec time */
    uint32_t       elems_curr;
    uint32_t       elems_limit;
    uint64_t       workspace[ TSTAMP_ENTRIES ];  /* Self descriptive... */
    adts_sanity_t  sanity;
} tstamp_mgr_t;



/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
typedef union {
    struct {
        uint32_t low;
        uint32_t high;
    };
    uint64_t cycles;
} cycles_t;




/******************************************************************************
 * ####### #     # #     #  #####  #######   ###   ####### #     #  #####
 * #       #     # ##    # #     #    #       #    #     # ##    # #     #
 * #       #     # # #   # #          #       #    #     # # #   # #
 * #####   #     # #  #  # #          #       #    #     # #  #  #  #####
 * #       #     # #   # # #          #       #    #     # #   # #       #
 * #       #     # #    ## #     #    #       #    #     # #    ## #     #
 * #        #####  #     #  #####     #      ###   ####### #     #  #####
******************************************************************************/
//ts_create
//ts_destroy
//ts_init()
//ts_start()
//ts_stop()
//ts_add()
//ts_calc()
//ts_get_last()
//ts_get_min()
//ts_get_max()
//ts_display()




/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
static inline uint64_t
adts_cycles_stop( void )
{
    cycles_t c;

    asm volatile ("RDTSCP\n\t"
                  "mov %%edx, %0\n\t"
                  "mov %%eax, %1\n\t"
                  "CPUID\n\t": "=r" (c.high), "=r" (c.low)
                      :: "%rax", "%rbx", "%rcx", "%rdx");

    return c.cycles;
} /* adts_cycles_stop() */


/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
static inline uint64_t
adts_cycles_start( void )
{
    cycles_t c;

    asm volatile ("CPUID\n\t"
                  "RDTSC\n\t"
                  "mov %%edx, %0\n\t"
                  "mov %%eax, %1\n\t": "=r" (c.high), "=r" (c.low)
                      :: "%rax", "%rbx", "%rcx", "%rdx");

    return c.cycles;
} /* adts_cycles_start() */



/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
static inline uint64_t
adts_cycles_base( void )
{
    uint64_t start, end;
    unsigned cycles_low, cycles_high, cycles_low1, cycles_high1;

    asm volatile ("CPUID\n\t"
            "RDTSC\n\t"
            "mov %%edx, %0\n\t"
            "mov %%eax, %1\n\t": "=r" (cycles_high), "=r" (cycles_low)
                :: "%rax", "%rbx", "%rcx", "%rdx");

    /***********************************/
    /* Test Start                      */
    /***********************************/


    /***********************************/
    /* Test End                        */
    /***********************************/

    asm   volatile("RDTSCP\n\t"
            "mov %%edx, %0\n\t"
            "mov %%eax, %1\n\t"
            "CPUID\n\t": "=r" (cycles_high1), "=r" (cycles_low1)
                :: "%rax", "%rbx", "%rcx", "%rdx");

    start = (((uint64_t) cycles_high  << 32) | cycles_low);
    end   = (((uint64_t) cycles_high1 << 32) | cycles_low1);

    //CDISPLAY("%llu", end - start);

    #if 0
    if ( (end - start) < 0) {
        /* OVERVFLOW!!!*/
        times[j][i] = 0;
    }
    else
    {
        times[j][i] = end - start;
    }
    #endif

    return (end - start);
} /* adts_cycles_base() */





/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
uint64_t
adts_tstamp( void )
{
    uint64_t         tsval  = 0;
    struct timespec  ts     = {0};
    struct timespec *p_ts   = &(ts);

    clock_gettime(CLOCK_MONOTONIC_RAW, p_ts);
    tsval = p_ts->tv_sec + p_ts->tv_nsec;

    return tsval;
} /* adts_tstamp() */



/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
void
adts_tstamp_destroy( adts_time_t *p_adts_time )
{
    size_t        bytes        = 0;
    tstamp_mgr_t *p_tstamp_mgr = (tstamp_mgr_t *) p_adts_time;

    adts_sanity_t *p_sanity = &(p_tstamp_mgr->sanity);

/*
    adts_sanity_entry(p_sanity);

    bytes = sizeof(*p_tstamp_mgr);
    memset(p_tstamp_mgr, 0, bytes);
    free(p_tstamp_mgr);
*/
    /* No adts_sanity_exit() since we've freed the memory */

    return;
} /* adts_tstamp_destroy() */



/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
adts_time_t *
adts_tstamp_create( void )
{
    int32_t        rc           = 0;
    adts_time_t   *p_adts_time  = NULL;
    tstamp_mgr_t  *p_tstamp_mgr = NULL;

    p_adts_time = adts_mem_zalloc(sizeof(*p_adts_time));
    if (NULL == p_adts_time) {
        rc = ENOMEM;
        goto exception;
    }

exception:
    return p_adts_time;
} /* adts_tstamp_crate() */



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
utest_time_bytes( void )
{
    CDISPLAY("[%u]", sizeof(tstamp_mgr_t));
    CDISPLAY("[%u]", sizeof(adts_time_t));

    _Static_assert(sizeof(tstamp_mgr_t) <= sizeof(adts_time_t),
        "Mismatch structs detected");

    return;
} /* utest_time_bytes() */



/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
static void
utest_time_constants( void )
{
    CDISPLAY("TSTAMP_ENTRIES: %u", TSTAMP_ENTRIES);

    return;
} /* utest_time_constants() */



/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
static void
utest_control( void )
{
    utest_time_bytes();
    utest_time_constants();

    CDISPLAY("=========================================================");
    {
        int64_t ts = 0;

        ts = adts_tstamp();
        CDISPLAY("%16llu", ts);
    }

    CDISPLAY("=========================================================");
    {
        int64_t ts1 = 0;
        int64_t ts2 = 0;

        ts1 = adts_tstamp();
        sleep(1);
        //nanosleep(???);
        ts2 = adts_tstamp();
        CDISPLAY("%16llu", ts1);
        CDISPLAY("%16llu", ts2);

        /* Time difference */
        CDISPLAY("%16llu", (ts2 - ts1));
    }

    CDISPLAY("=========================================================");
    {
        #define  UT_ITER   (1000)

        uint64_t min    = ~(0);
        uint64_t max    = 0;
        uint64_t cycles = 0;

        for (int32_t cnt = 0; cnt < UT_ITER; cnt++) {
            cycles = adts_cycles_base();
            max = MAX(max, cycles);
            min = MIN(min, cycles);
        }

        CDISPLAY("iterations: %llu", UT_ITER);
        CDISPLAY("min: %10llu", min);
        CDISPLAY("max: %10llu", max);
    }

    CDISPLAY("=========================================================");
    {
        // out of time, iterate to normalize this for more acurate sampling
        uint64_t cs = 0;
        uint64_t ce = 0;

        cs = adts_cycles_start();
        ce = adts_cycles_stop();

        CDISPLAY("%llu", cs);
        CDISPLAY("%llu", ce);
        CDISPLAY("%llu", (ce - cs));
    }

    CDISPLAY("=========================================================");
    {
        #define  UT_ITER   (1000)

        uint64_t cs     = 0;
        uint64_t ce     = 0;
        uint64_t min    = ~(0);
        uint64_t max    = 0;
        uint64_t cycles = 0;

        for (int32_t cnt = 0; cnt < UT_ITER; cnt++) {
            cs     = adts_cycles_start();
            {
                /* Measure code here */
                //char *x = adts_mem_zalloc(1024);
                char *x = malloc(1024);
                free(x);
            }
            ce     = adts_cycles_stop();
            cycles = (ce - cs);

            max = MAX(max, cycles);
            min = MIN(min, cycles);
        }

        CDISPLAY("iterations: %llu", UT_ITER);
        CDISPLAY("min: %10llu", min);
        CDISPLAY("max: %10llu", max);
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
utest_adts_time( void )
{

    utest_control();

    return;
} /* utest_adts_time() */
