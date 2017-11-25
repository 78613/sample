
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
#include <adts_sort.h>
#include <adts_hexdump.h>


/*
 ****************************************************************************
 * \details
 *   macro based swap logic.
 *    - leverages input type for tmp and max reuse
 *    - macro based for pseudo inline capability
 ****************************************************************************
 */
#define SWAP( _pa, _pb, _type ) { \
    _type _tmp = *_pa;            \
                                  \
    *_pa = *_pb;                  \
    *_pb = _tmp;                  \
}


/*
 ****************************************************************************
 * \details
 *   uniformly random permutation of the input array, provided no
 *   duplicate values
 *
 *   FIXME:
 *     Using % i results in pseudo - random.  There is no inherit capability
 *     in C standard lib to get a random number from 0 -> M.  Thus more
 *     research is neede here
 *
 * Time:  O(n) // linear time shuffle
 * Space: O(1) // no space scaling
 ****************************************************************************
 */
void
adts_shuffle64( uint64_t arr[],
                size_t   elems )
{
    for (int32_t i = 0; i < elems; i++) {
        //FIXME: Using % i results in pseudo - random.  Research this further
        int32_t r = rand() % (i + 1);
        SWAP(&arr[i], &arr[r], uint64_t);
    }

    return;
} /* adts_shuffle64() */



/*
 ****************************************************************************
 *
 ****************************************************************************
 */
bool
adts_arr_is_not_sorted64( int32_t arr[],
                          size_t  elems )
{
    bool rc = 0;

    if (unlikely(1 >= elems)) {
        /* sorted by definition */
        goto exception;
    }

    for (int32_t idx = 1; idx < elems; idx++) {
        if (arr[idx - 1] > arr[idx]) {
            rc = true;
            break;
        }
    }

exception:
    return rc;
} /* adts_arr_is_not_sorted64() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
bool
adts_arr_is_sorted64( uint64_t arr[],
                      size_t   elems )
{
    return !(adts_arr_is_not_sorted64(arr, elems));
} /* adts_arr_is_sorted64() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
static int32_t
sort_quick_partition64( uint64_t arr[],
                        size_t  lo,
                        size_t  hi )
{
    int32_t i = lo;
    int32_t j = hi + 1;

    for (;;) {
        /* find item on left to swap */
        while (arr[++i] < arr[lo]) {
            if (i == hi) {
                break;
            }
        }

        /* find item on right to swap */
        while (arr[lo] < arr[--j]) {
            if (j == lo) {
                break;
            }
        }

        /* check if pointes crossed */
        if (i >= j) {
            break;
        }

        SWAP(&arr[i], &arr[j], uint64_t);
    }

    /* swap with partition item */
    SWAP(&arr[lo], &arr[j], uint64_t);

    /* Index of item now in place */
    return j;
} /* sort_quick_partition64() */



/*
 ****************************************************************************
 *
 ****************************************************************************
 */
static void
sort_quick64( uint64_t arr[],
              size_t   lo,
              size_t   hi )
{
    if (hi <= lo) {
        /* done */
        goto exception;
    }

    #if 0
    /* small arrays are more efficiently processed with elementary sorts */
    if (hi <= (lo + SORT_THREASHOLD - 1)) {
        adts_sort_insertion_ext(arr, lo, hi);
        goto exception;
    }
    #endif

    int32_t j = sort_quick_partition64(arr, lo, hi);

    sort_quick64(arr, lo, (j - 1));
    sort_quick64(arr, (j + 1), hi);

exception:
    return;
} /* sort_quick64() */


/*
 ****************************************************************************
 *
 *  Time:  O(?)
 *  Space: O(1)
 ****************************************************************************
 */
void
adts_sort_quick64( uint64_t arr[],
                   size_t   elems )
{
    /* shuffle to probabilistically eliminate the N^2 worst case scenario */
    adts_shuffle64(arr, elems);

    /* start the sort recursion */
    sort_quick64(arr, 0, elems - 1);

    return;
} /* adts_sort_quick64() */



static void
utest_control( void )
{
    CDISPLAY("====================================================");
    {
        CDISPLAY("Baseline:");

        size_t        elems  = 1024 * 1024;
        size_t        bytes  = sizeof(uint64_t) * elems;
        float         mean   = 0.0;
        float         sum    = 0.0;
        float         var    = 0.0;
        float         stdev  = 0.0;
        int32_t       rc     = 0;
        uint64_t      min    = ~(0);
        uint64_t      max    = 0;
        uint64_t      stop   = 0;
        uint64_t      start  = 0;
        uint64_t      range  = 0;
        uint64_t     *p_arr  = NULL;
        uint64_t      cycles = 0;
        uint64_t      p25    = 0;
        uint64_t      p50    = 0;
        uint64_t      p75    = 0;
        uint64_t      p99    = 0;
        uint64_t      p39s   = 0;
        uint64_t      p49s   = 0;
        uint64_t      p59s   = 0;
        uint64_t      p69s   = 0;
        uint64_t      p79s   = 0;
        uint64_t      p89s   = 0;
        uint64_t      p99s   = 0;

        p_arr = malloc(bytes);
        assert(p_arr);
        memset(p_arr, 0, bytes);

        /* Basic measures */
        for (int32_t cnt = 0; cnt < elems; cnt++) {
            start      = adts_cycles_start();
            stop       = adts_cycles_stop();
            cycles     = stop - start;
            max        = MAX(max, cycles);
            min        = MIN(min, cycles);
            sum       += cycles;
            p_arr[cnt] = cycles;
        }

        /* Advanced measures */
        range = max - min;
        mean  = sum / elems;
        for (int32_t cnt = 0; cnt < elems; cnt++) {
            stdev += pow((p_arr[cnt] - mean), 2);
        }
        var   = stdev / elems;
        stdev = sqrt(var);

        /* Percentiles */
        adts_sort_quick64(p_arr, elems);
        rc = adts_arr_is_sorted64(p_arr, elems);
        assert(false == rc);

        p25  = (25 * (elems + 1)) / 100;
        p50  = (50 * (elems + 1)) / 100;
        p75  = (75 * (elems + 1)) / 100;
        p99  = (99 * (elems + 1)) / 100;
        p39s = (99.9 * (elems + 1)) / 100;
        p49s = (99.99 * (elems + 1)) / 100;
        p59s = (99.999 * (elems + 1)) / 100;
        p69s = (99.9999 * (elems + 1)) / 100;
        p79s = (99.99999 * (elems + 1)) / 100;
        p89s = (99.999999 * (elems + 1)) / 100;
        p99s = (99.9999999 * (elems + 1)) / 100;

        CDISPLAY("measures:    %16llu",  elems);
        CDISPLAY("min:         %16llu",  min);
        CDISPLAY("max:         %16llu",  max);
        CDISPLAY("range:       %16llu",  range);
        CDISPLAY("mean:        %16f",    mean);
        CDISPLAY("variance:    %16f",    var);
        CDISPLAY("std.dev:     %16f",    stdev);
        CDISPLAY("p25:         %16llu",  p_arr[p25]);
        CDISPLAY("p50:         %16llu",  p_arr[p50]);
        CDISPLAY("p75:         %16llu",  p_arr[p75]);
        CDISPLAY("p99:         %16llu",  p_arr[p99]);
        CDISPLAY("p99.9:       %16llu",  p_arr[p39s]);
        CDISPLAY("p99.99:      %16llu",  p_arr[p49s]);
        CDISPLAY("p99.999:     %16llu",  p_arr[p59s]);
        CDISPLAY("p99.9999:    %16llu",  p_arr[p69s]);
        CDISPLAY("p99.99999:   %16llu",  p_arr[p79s]);
        CDISPLAY("p99.999999:  %16llu",  p_arr[p89s]);
        CDISPLAY("p99.9999999: %16llu",  p_arr[p99s]);

        free(p_arr);
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
