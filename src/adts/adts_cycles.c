
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
adts_cycles_baseline( void )
{
    uint64_t start = 0;
    uint64_t stop  = 0;

    start = adts_cycles_start();
    stop  = adts_cycles_stop();

    return (stop - start);
} /* adts_cycles_baseline() */


#if 0
/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
static inline uint64_t
adts_cycles_baseline( void )
{
    uint64_t begin;
    uint64_t end;
    unsigned cycles_low,
             cycles_high,
             cycles_low1,
             cycles_high1;

    asm volatile ("CPUID\n\t"
            "RDTSC\n\t"
            "mov %%edx, %0\n\t"
            "mov %%eax, %1\n\t": "=r" (cycles_high), "=r" (cycles_low)
                :: "%rax", "%rbx", "%rcx", "%rdx");

    /***********************************/
    /* Test begin                      */
    /***********************************/


    /***********************************/
    /* Test End                        */
    /***********************************/

    asm   volatile("RDTSCP\n\t"
            "mov %%edx, %0\n\t"
            "mov %%eax, %1\n\t"
            "CPUID\n\t": "=r" (cycles_high1), "=r" (cycles_low1)
                :: "%rax", "%rbx", "%rcx", "%rdx");

    begin = (((uint64_t) cycles_high  << 32) | cycles_low);
    end   = (((uint64_t) cycles_high1 << 32) | cycles_low1);

    //CDISPLAY("%llu", end - begin);

    #if 0
    if ( (end - begin) < 0) {
        /* OVERVFLOW!!!*/
        times[j][i] = 0;
    }
    else
    {
        times[j][i] = end - begin;
    }
    #endif

    return (end - begin);
} /* adts_cycles_baseline() */
#endif


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
#include <math.h>
#include <assert.h>

static void
utest_control( void )
{
    CDISPLAY("====================================================");
    {
        CDISPLAY("Baseline:");

        const size_t  elems  = 1024 * 1024;
        uint64_t      cycles = 0;
        float         mean   = 0;
        uint64_t      min    = ~(0);
        uint64_t      max    = 0;
        uint64_t      sum    = 0;
        uint64_t      start  = 0;
        uint64_t      stop   = 0;

        for (int32_t cnt = 0; cnt < elems; cnt++) {
            start      = adts_cycles_start();
            stop       = adts_cycles_stop();
            cycles     = stop - start;
            max        = MAX(max, cycles);
            min        = MIN(min, cycles);
            sum       += cycles;
        }
        mean = (float) sum / elems;

        CDISPLAY("iter: %16llu",  elems);
        CDISPLAY("min:  %16llu",  min);
        CDISPLAY("max:  %16llu",  max);
        CDISPLAY("mean: %16f",   mean);
    }

    CDISPLAY("====================================================");
    {
        CDISPLAY("Baseline:");

        const size_t  elems  = 1024 * 1024;
        const size_t  bytes  = sizeof(uint64_t) * elems;
        uint64_t      cycles = 0;
        float         mean   = 0.0;
        float         sum    = 0.0;
        float         stdev  = 0.0;
        float         stvar  = 0.0;
        uint64_t      min    = ~(0);
        uint64_t      max    = 0;
        uint64_t      start  = 0;
        uint64_t      stop   = 0;
        uint64_t     *p_arr  = NULL;

        p_arr = malloc(bytes);
        assert(p_arr);
        memset(p_arr, 0, bytes);

        for (int32_t cnt = 0; cnt < elems; cnt++) {
            start      = adts_cycles_start();
            stop       = adts_cycles_stop();
            cycles     = stop - start;
            max        = MAX(max, cycles);
            min        = MIN(min, cycles);
            sum       += cycles;
            p_arr[cnt] = cycles;
        }

        mean = sum / elems;
        for (int32_t cnt = 0; cnt < elems; cnt++) {
            stdev += pow((p_arr[cnt] - mean), 2);
        }
        stvar = stdev / elems;
        stdev = sqrt(stvar);

        CDISPLAY("iter:  %16llu",  elems);
        CDISPLAY("min:   %16llu",  min);
        CDISPLAY("max:   %16llu",  max);
        CDISPLAY("mean:  %16f",    mean);
        CDISPLAY("stvar: %16f",    stvar);
        CDISPLAY("stdev: %16f",    stdev);

        free(p_arr);
    }

#if 0
    CDISPLAY("====================================================");
    {
        CDISPLAY("Test1:");
        #define  UT_ITER   (1000)

        uint64_t min    = ~(0);
        uint64_t max    = 0;
        uint64_t cycles = 0;

        for (int32_t cnt = 0; cnt < UT_ITER; cnt++) {
            cycles = adts_cycles_baseline();
            max = MAX(max, cycles);
            min = MIN(min, cycles);
        }

        CDISPLAY("iterations: %llu", UT_ITER);
        CDISPLAY("min: %10llu", min);
        CDISPLAY("max: %10llu", max);
    }

    CDISPLAY("Test2: ===================================================");
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

    CDISPLAY("Test 3: ====================================================");
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

    CDISPLAY("Test 4: ====================================================");
    {
        uint64_t cyc = 0;

        cyc = adts_cycles_baseline();
        CDISPLAY("%llu", cyc);
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
utest_adts_cycles( void )
{

    utest_control();

    return;
} /* utest_adts_cycles() */
