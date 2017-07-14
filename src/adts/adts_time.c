
#include <time.h>  /* clock_gettime() */
#include <sched.h> /* sched_getcpu() */
#include <string.h>
#include <inttypes.h>

#include <adts_services.h>
#include <adts_time.h>


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
#define TSTAMP_ENTRIES (TSTAMP_WORKSPACE_BYTES / sizeof(struct timespec))

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
    int64_t         mim;
    int64_t         max;
    int64_t         last;
    int32_t         elems_curr;
    int32_t         elems_limit;
    struct timespec history[ TSTAMP_ENTRIES ];
    char            message[ TSTAMP_MESSAGE_BYTES ];
} tstamp_mgr_t;



/******************************************************************************
 * ####### #     # #     #  #####  #######   ###   ####### #     #  #####
 * #       #     # ##    # #     #    #       #    #     # ##    # #     #
 * #       #     # # #   # #          #       #    #     # # #   # #
 * #####   #     # #  #  # #          #       #    #     # #  #  #  #####
 * #       #     # #   # # #          #       #    #     # #   # #       #
 * #       #     # #    ## #     #    #       #    #     # #    ## #     #
 * #        #####  #     #  #####     #      ###   ####### #     #  #####
******************************************************************************/

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
int64_t
adts_timestamp_approximate( void )
{
    uint64_t         tsval  = 0;
    struct timespec  ts     = {0};
    struct timespec *p_ts   = &(ts);

    clock_gettime(CLOCK_REALTIME, p_ts);
    //tsval = p_ts->tv_sec + p_ts->tv_nsec;
    //tsval = p_ts->tv_sec;
    tsval = p_ts->tv_nsec;

    return tsval;
} /* adts_timestamp_approximate() */


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
utest_control( void )
{
    utest_time_bytes();

    CDISPLAY("=========================================================");
    {
        int64_t ts = 0;

        ts = adts_timestamp_approximate();
        CDISPLAY("%16llu", ts);
    }

    CDISPLAY("=========================================================");
    {
        int64_t ts1 = 0;
        int64_t ts2 = 0;

        ts1 = adts_timestamp_approximate();
        //sleep(1);
        //nanosleep(???);
        ts2 = adts_timestamp_approximate();
        CDISPLAY("%16llu", ts1);
        CDISPLAY("%16llu", ts2);

        /* Time difference */
        CDISPLAY("%16llu", (ts2 - ts1));
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
