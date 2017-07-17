
#include <time.h>  /* clock_gettime() */
#include <sched.h> /* sched_getcpu() */
#include <string.h>
#include <inttypes.h>

#include <adts_snapshot.h>
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


/**
 ****************************************************************************
 * \details
 *
 ****************************************************************************
 */
#define SNAPSHOT_NAME_BYTES (32)
#define SNAPSHOT_DATE_BYTES (16)
#define SNAPSHOT_TIME_BYTES (16)


/**
 ****************************************************************************
 * \details
 *
 ****************************************************************************
 */
typedef struct {
    char       file[ SNAPSHOT_NAME_BYTES ];
    char       func[ SNAPSHOT_NAME_BYTES ];
    char       date[ SNAPSHOT_DATE_BYTES ];
    char       time[ SNAPSHOT_TIME_BYTES ];
    int64_t    timestamp;
    int32_t    cpu;
    int32_t    line;
} snapshot_t;


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
 ****************************************************************************
 * \details
 *
 ****************************************************************************
 */
void
adts_snapshot_display( adts_snapshot_t *p_adt_snap )
{
    snapshot_t *p_snap = (snapshot_t *) p_adt_snap;

    printf("%3d %4d %-25.25s %-30.30s %s %s 0x%016llX \n",
            p_snap->cpu,
            p_snap->line,
            p_snap->file,
            p_snap->func,
            p_snap->date,
            p_snap->time,
            p_snap->timestamp);

    return;
} /* adts_snapshot_display() */

/**
 ****************************************************************************
 * \details
 *
 ****************************************************************************
 */
void
adts_snapshot_private( adts_snapshot_t *p_adt_snap,
                       int32_t          line,
                       char            *p_file,
                       char            *p_func,
                       char            *p_date,
                       char            *p_time )
{
    uint64_t         tsval  = 0;
    snapshot_t      *p_snap = (snapshot_t *) p_adt_snap;
    struct timespec  ts     = {0};
    struct timespec *p_ts   = &(ts);

    clock_gettime(CLOCK_REALTIME, p_ts);
    tsval = p_ts->tv_sec + p_ts->tv_nsec;

    memset(p_adt_snap, 0, sizeof(*p_adt_snap));

    p_snap->cpu       = sched_getcpu();
    p_snap->line      = line;
    p_snap->timestamp = tsval;
    strncpy(p_snap->file, p_file, sizeof(p_snap->file));
    strncpy(p_snap->func, p_func, sizeof(p_snap->func));
    strncpy(p_snap->date, p_date, sizeof(p_snap->date));
    strncpy(p_snap->time, p_time, sizeof(p_snap->time));

    return;
} /* adts_snapshot_private() */




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
utest_snapshot_bytes( void )
{
    _Static_assert(sizeof(snapshot_t) <= sizeof(adts_snapshot_t),
        "Mismatch structs detected");

    CDISPLAY("[%u]", sizeof(snapshot_t));
    CDISPLAY("[%u]", sizeof(adts_snapshot_t));

    return;
} /* utest_snapshot_bytes() */


/*
 ****************************************************************************
 * \details
 *
 ****************************************************************************
 */
static void
utest_control( void )
{
    CDISPLAY("=========================================================");
    {
        utest_snapshot_bytes();
    }

    CDISPLAY("=========================================================");
    {
        adts_snapshot_t adt_snap    = {0};
        adts_snapshot_t *p_adt_snap = &(adt_snap);

        adts_snapshot(p_adt_snap);
        adts_snapshot_display(p_adt_snap);
    }

    return;
} /* utest_control() */


/**
 **************************************************************************
 * \details
 *   Public test entrypoint
 *
 **************************************************************************
 */
void
utest_adts_snapshot( void )
{
    utest_control();

    return;
} /* utest_adts_snapshot() */
