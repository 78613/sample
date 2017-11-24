
#include <time.h>  /* clock_gettime() */
#include <errno.h>
#include <string.h>
#include <inttypes.h>

/* Toolbox */
#include <adts_cycles.h>
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



/******************************************************************************
 * #     # #     #   ###   ####### ####### #######  #####  #######  #####
 * #     # ##    #    #       #       #    #       #     #    #    #     #
 * #     # # #   #    #       #       #    #       #          #    #
 * #     # #  #  #    #       #       #    #####    #####     #     #####
 * #     # #   # #    #       #       #    #             #    #          #
 * #     # #    ##    #       #       #    #       #     #    #    #     #
 *  #####  #     #   ###      #       #    #######  #####     #     #####
******************************************************************************/


/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
static void
utest_control( void )
{

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
utest_adts_cycles( void )
{

    utest_control();

    return;
} /* utest_adts_cycles() */
