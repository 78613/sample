
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
#define SWAP( _pa, _pb, _type ) \
do {                            \
    _type _ptmp = *(_pa);       \
                                \
    *(_pa) = *(_pb);            \
    *(_pb) = _ptmp;             \
} while (0);


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
#define ADTS_SHUFFLE( _arr, _elems, _type )        \
do {                                               \
    for (int64_t _i = 0; _i < (_elems); _i++) {    \
        int32_t _r = rand() % (_i + 1);            \
        SWAP(&(_arr[_i]), &(_arr[_r]), _type);     \
    }                                              \
} while (0);


/*
 ****************************************************************************
 * Return TRUE if Array is _NOT_ sorted from min to max.
 ****************************************************************************
 */
#if 0
#define ADTS_ARR_UNSORTED( _arr, _elems, _rc )      \
do {                                                \
    _rc = false;                                    \
    if (unlikely(1 >= (_elems))) {                  \
        /* sorted by definition */                  \
        break;                                      \
    }                                               \
                                                    \
    for (int64_t _i = 1; _i < (_elems); _i++) {     \
        if ((_arr[_i - 1]) > (_arr[_i])) {          \
            (_rc) = true;                           \
            break;                                  \
        }                                           \
    }                                               \
} while (0);
#endif

#if 1
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
#endif

/*
 ****************************************************************************
 * Returns TRUE is Array IS sorted from min to max.
 ****************************************************************************
 */
#if 0
#define ADTS_ARR_SORTED( _arr, _elems, _ret )  \
do {                                           \
    bool _tmp = false;                         \
    ADTS_ARR_UNSORTED((_arr), (_elems), _tmp); \
    _ret = !(_tmp);                            \
} while (0);

#endif

#if 1
bool
adts_arr_is_sorted64( uint64_t arr[],
                      size_t   elems )
{
    return !(adts_arr_is_not_sorted64(arr, elems));
} /* adts_arr_is_sorted64() */
#endif

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
    ADTS_SHUFFLE(arr, elems, uint64_t);

    /* start the sort recursion */
    sort_quick64(arr, 0, elems - 1);

    return;
} /* adts_sort_quick64() */



typedef struct {
    bool          initialized;

    uint64_t     *p_arr; /* Data */
    uint64_t      elems; /* Measurements */
    uint64_t      min;
    uint64_t      max;

    double        mean;
    double        median;   /* median */
    uint64_t      medidx;   /* index of median */
    uint64_t      medleft;  /* even median ancestor */
    uint64_t      medright; /* even median ancestor */
    uint64_t      mode;
    uint64_t      moden;    /* instances of mode */

    uint64_t      range;
    double        variance;
    double        stdev;
    double        x1; /* 3sigma 68.27 */
    double        x2; /* 3sigma 95.45 */
    double        x3; /* 3sigma 99.73 */

    uint64_t      p25; /* quartiles */
    uint64_t      p50;
    uint64_t      p75;
    uint64_t      p99;

    uint64_t      p39s;
    uint64_t      p49s;
    uint64_t      p59s;
    uint64_t      p69s;
    uint64_t      p79s;
    uint64_t      p89s;
    uint64_t      p99s;
} adts_measures_t;

static void
adts_measures_init( adts_measures_t *p_meas )
{
    memset(p_meas, 0, sizeof(*p_meas));
    p_meas->min = ~(0);
    p_meas->initialized = true;
    return;
} /* adts_measures_init() */


static void
adts_measures_display( adts_measures_t *p_m )
{
    uint64_t min = 0;
    uint64_t max = 0;

    assert(p_m->initialized);

    CDISPLAY("measurements:%16llu",  p_m->elems);

    CDISPLAY("min:         %16llu",  p_m->min);
    CDISPLAY("max:         %16llu",  p_m->max);
    CDISPLAY("range:       %16llu (%llu-%llu)",  p_m->range, p_m->min, p_m->max);

    CDISPLAY("mean:        %16f",    p_m->mean);
    if (p_m->elems & 0x1) {
        CDISPLAY("median:      %16f [%llu]: %llu",    p_m->median,
            p_m->medidx, (uint64_t) p_m->median);
    }else {
        CDISPLAY("median:      %16f [%llu]:%llu [%llu]:%llu", p_m->median,
            (p_m->medidx - 1), p_m->medleft, p_m->medidx, p_m->medright);
    }
    CDISPLAY("mode:        %16llu",  p_m->mode);
    CDISPLAY("variance:    %16f",    p_m->variance);
    CDISPLAY("std.dev:     %16f",    p_m->stdev);

    /* 3-Sigma || Z-Score */
    min = ((p_m->mean - p_m->x1) < p_m->min) ? p_m->min : (p_m->mean - p_m->x1);
    max = ((p_m->mean + p_m->x1) > p_m->max) ? p_m->max : (p_m->mean + p_m->x1);
    CDISPLAY(" x1 68%%      %16f (%llu-%llu)", p_m->x1, min, max);

    min = ((p_m->mean - p_m->x2) < p_m->min) ? p_m->min : (p_m->mean - p_m->x2);
    max = ((p_m->mean + p_m->x2) > p_m->max) ? p_m->max : (p_m->mean + p_m->x2);
    CDISPLAY(" x2 95%%      %16f (%llu-%llu)", p_m->x2, min, max);

    min = ((p_m->mean - p_m->x3) < p_m->min) ? p_m->min : (p_m->mean - p_m->x3);
    max = ((p_m->mean + p_m->x3) > p_m->max) ? p_m->max : (p_m->mean + p_m->x3);
    CDISPLAY(" x3 99.7%%    %16f (%llu-%llu)", p_m->x3, min, max);

    /* Quartiles */
    CDISPLAY("p25:         %16llu",  p_m->p25);
    CDISPLAY("p50:         %16llu",  p_m->p50);
    CDISPLAY("p75:         %16llu",  p_m->p75);
    CDISPLAY("p99:         %16llu",  p_m->p99);

    /* Percentiles */
    CDISPLAY("p99.9:       %16llu",  p_m->p39s);
    CDISPLAY("p99.99:      %16llu",  p_m->p49s);
    CDISPLAY("p99.999:     %16llu",  p_m->p59s);
    CDISPLAY("p99.9999:    %16llu",  p_m->p69s);
    CDISPLAY("p99.99999:   %16llu",  p_m->p79s);
    CDISPLAY("p99.999999:  %16llu",  p_m->p89s);
    CDISPLAY("p99.9999999: %16llu",  p_m->p99s);

    return;
} /* adts_measures_display() */


static void
adts_measures( uint64_t        *p_arr,
               size_t           elems,
               adts_measures_t *p_m )
{
    uint64_t sum = 0;

    adts_measures_init(p_m);

    p_m->p_arr = p_arr;
    p_m->elems = elems;

    /* Basic measures */
    for (int32_t cnt = 0; cnt < p_m->elems; cnt++) {
        p_m->max   = MAX(p_m->max, p_arr[cnt]);
        p_m->min   = MIN(p_m->min, p_arr[cnt]);
        sum       += p_arr[cnt];
        //CDISPLAY("[%2llu]: %5llu", cnt, p_arr[cnt]);
    }

    /* Advanced measures */
    p_m->range  = p_m->max - p_m->min;
    p_m->mean   = sum / p_m->elems;

    for (int32_t cnt = 0; cnt < p_m->elems; cnt++) {
        p_m->stdev += pow((p_arr[cnt] - p_m->mean), 2);
    }
    p_m->variance = p_m->stdev / p_m->elems;
    p_m->stdev    = sqrt(p_m->variance);

    p_m->x1 = p_m->stdev * 1;
    p_m->x2 = p_m->stdev * 2;
    p_m->x3 = p_m->stdev * 3;

    /* Percentiles */
    //FIXME: There's a bug in this sort function.....
    CDISPLAY("..........");
    adts_sort_quick64(p_arr, p_m->elems);
    CDISPLAY("..........");
    //for (int32_t cnt = 0; cnt < p_m->elems; cnt++) {
    //    CDISPLAY("[%2u]: %5llu", cnt, p_arr[cnt]);
    //}

    if (p_m->elems & 0x1) {
        p_m->medidx = p_m->elems / 2;
        p_m->median = p_arr[p_m->medidx];
    }else {
        p_m->medidx   = p_m->elems / 2;
        p_m->medleft  = p_arr[p_m->medidx - 1];
        p_m->medright = p_arr[p_m->medidx];
        p_m->median   = (p_m->medleft + p_m->medright) / 2;
    }

    /* Calculated indexes */
    p_m->p25  = ((25 * (p_m->elems + 1)) / 100);
    p_m->p50  = ((55 * (p_m->elems + 1)) / 100);
    p_m->p75  = ((75 * (p_m->elems + 1)) / 100);
    p_m->p99  = ((99 * (p_m->elems + 1)) / 100);
    p_m->p39s = ((99.9 * (p_m->elems + 1)) / 100);
    p_m->p49s = ((99.99 * (p_m->elems + 1)) / 100);
    p_m->p59s = ((99.999 * (p_m->elems + 1)) / 100);
    p_m->p69s = ((99.9999 * (p_m->elems + 1)) / 100);
    p_m->p79s = ((99.99999 * (p_m->elems + 1)) / 100);
    p_m->p89s = ((99.999999 * (p_m->elems + 1)) / 100);
    p_m->p99s = ((99.9999999 * (p_m->elems + 1)) / 100);

    p_m->p25  = p_arr[p_m->p25 - 1];
    p_m->p50  = p_arr[p_m->p50 - 1];
    p_m->p75  = p_arr[p_m->p75 - 1];
    p_m->p99  = p_arr[p_m->p99 - 1];
    p_m->p39s = p_arr[p_m->p39s - 1];
    p_m->p49s = p_arr[p_m->p49s - 1];
    p_m->p59s = p_arr[p_m->p59s - 1];
    p_m->p69s = p_arr[p_m->p69s - 1];
    p_m->p79s = p_arr[p_m->p79s - 1];
    p_m->p89s = p_arr[p_m->p89s - 1];
    p_m->p99s = p_arr[p_m->p99s - 1];

    return;
} /* adts_measures() */



static void
utest_control( void )
{

    CDISPLAY("====================================================");
    {
        size_t           iter   = 1000 * 1000;
        size_t           bytes  = sizeof(uint64_t) * iter;
        uint64_t        *p_arr  = NULL;
        uint64_t         stop   = 0;
        uint64_t         start  = 0;
        uint64_t         cycles = 0;
        adts_measures_t  m;
        adts_measures_t *p_m = &(m);

        p_arr = malloc(bytes);
        assert(p_arr);
        memset(p_arr, 0, bytes);

        for (int32_t cnt = 0; cnt < iter; cnt++) {
            start      = adts_cycles_start();
            stop       = adts_cycles_stop();
            cycles     = stop - start;
            p_arr[cnt] = cycles;
            //CDISPLAY("[%2llu]: %5llu", cnt, p_arr[cnt]);
        }

        adts_measures(p_arr, iter, p_m);
        adts_measures_display(p_m);

        free(p_arr);
    }

    #if 0
    CDISPLAY("====================================================");
    {
        size_t           iter   = 1000 * 1000;
        size_t           bytes  = sizeof(uint64_t) * iter;
        uint64_t        *p_arr  = NULL;
        uint64_t         sum    = 0;
        uint64_t         stop   = 0;
        uint64_t         start  = 0;
        uint64_t         cycles = 0;
        adts_measures_t  m;
        adts_measures_t *p_m = &(m);

        adts_measures_init(p_m);

        p_arr = malloc(bytes);
        assert(p_arr);
        memset(p_arr, 0, bytes);

        p_m->elems = iter;

        /* Basic measures */
        for (int32_t cnt = 0; cnt < p_m->elems; cnt++) {
            start      = adts_cycles_start();
            stop       = adts_cycles_stop();
            cycles     = stop - start;

            p_m->max   = MAX(p_m->max, cycles);
            p_m->min   = MIN(p_m->min, cycles);

            sum       += cycles;
            p_arr[cnt] = cycles;
            //CDISPLAY("[%2llu]: %5llu", cnt, p_arr[cnt]);
        }

        /* Advanced measures */
        p_m->range  = p_m->max - p_m->min;
        p_m->mean   = sum / p_m->elems;

        for (int32_t cnt = 0; cnt < p_m->elems; cnt++) {
            p_m->stdev += pow((p_arr[cnt] - p_m->mean), 2);
        }
        p_m->variance = p_m->stdev / p_m->elems;
        p_m->stdev    = sqrt(p_m->variance);

        p_m->x1 = p_m->stdev * 1;
        p_m->x2 = p_m->stdev * 2;
        p_m->x3 = p_m->stdev * 3;

        /* Percentiles */
        //FIXME: There's a bug in this sort function.....
        CDISPLAY("..........");
        adts_sort_quick64(p_arr, p_m->elems);
        CDISPLAY("..........");
        //for (int32_t cnt = 0; cnt < p_m->elems; cnt++) {
        //    CDISPLAY("[%2u]: %5llu", cnt, p_arr[cnt]);
        //}

        if (p_m->elems & 0x1) {
            p_m->medidx = p_m->elems / 2;
            p_m->median = p_arr[p_m->medidx];
        }else {
            p_m->medidx   = p_m->elems / 2;
            p_m->medleft  = p_arr[p_m->medidx - 1];
            p_m->medright = p_arr[p_m->medidx];
            p_m->median   = (p_m->medleft + p_m->medright) / 2;
        }

        /* Calculated indexes */
        p_m->p25  = ((25 * (p_m->elems + 1)) / 100);
        p_m->p50  = ((55 * (p_m->elems + 1)) / 100);
        p_m->p75  = ((75 * (p_m->elems + 1)) / 100);
        p_m->p99  = ((99 * (p_m->elems + 1)) / 100);
        p_m->p39s = ((99.9 * (p_m->elems + 1)) / 100);
        p_m->p49s = ((99.99 * (p_m->elems + 1)) / 100);
        p_m->p59s = ((99.999 * (p_m->elems + 1)) / 100);
        p_m->p69s = ((99.9999 * (p_m->elems + 1)) / 100);
        p_m->p79s = ((99.99999 * (p_m->elems + 1)) / 100);
        p_m->p89s = ((99.999999 * (p_m->elems + 1)) / 100);
        p_m->p99s = ((99.9999999 * (p_m->elems + 1)) / 100);

        p_m->p25  = p_arr[p_m->p25 - 1];
        p_m->p50  = p_arr[p_m->p50 - 1];
        p_m->p75  = p_arr[p_m->p75 - 1];
        p_m->p99  = p_arr[p_m->p99 - 1];
        p_m->p39s = p_arr[p_m->p39s - 1];
        p_m->p49s = p_arr[p_m->p49s - 1];
        p_m->p59s = p_arr[p_m->p59s - 1];
        p_m->p69s = p_arr[p_m->p69s - 1];
        p_m->p79s = p_arr[p_m->p79s - 1];
        p_m->p89s = p_arr[p_m->p89s - 1];
        p_m->p99s = p_arr[p_m->p99s - 1];

        #if 0
        /* mode */
        {
            uint64_t  buckets = p_m->median - p_m->min;
            size_t    lbytes  = buckets * sizeof(uint64_t);
            uint64_t *p_tmp   = NULL;

            CDISPLAY("buckets: %llu", buckets);

            switch (buckets) {
                case 0:
                    p_m->mode = p_m->min;
                    //FIXME: get hit count.
                    break;
                case 1:
                    assert(true);
                    break;
                default:
            }

            p_tmp = malloc(lbytes);
            assert(p_tmp);
            memset(p_arr, 0, lbytes);

            for (int32_t cnt = 0; cnt < buckets; cnt++) {
                CDISPLAY("[%2u]: %5llu", cnt, p_tmp[cnt]);
            }

            free(p_tmp);
        }
        #endif

        adts_measures_display(p_m);
        //adts_hexdump(p_m, sizeof(*p_m), "?");
        free(p_arr);
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
