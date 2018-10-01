
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
utest_cycles_bytes( void )
{
    CDISPLAY("[%u]", sizeof(cycles_t));
    CDISPLAY("[%u]", sizeof(adts_cycles_t));

    _Static_assert(sizeof(cycles_t) <= sizeof(adts_cycles_t),
        "Mismatch structs detected");

    return;
} /* utest_cycles_bytes() */


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

    uint64_t      p25; /* quartiles */
    uint64_t      p50;
    uint64_t      p75;
    uint64_t      p99;

    uint64_t      p25i; /* indexes */
    uint64_t      p50i;
    uint64_t      p75i;
    uint64_t      p99i;

    uint64_t      p39s; /* percentiles */
    uint64_t      p49s;
    uint64_t      p59s;
    uint64_t      p69s;
    uint64_t      p79s;
    uint64_t      p89s;
    uint64_t      p99s;

    uint64_t      p39si; /* Indexes */
    uint64_t      p49si;
    uint64_t      p59si;
    uint64_t      p69si;
    uint64_t      p79si;
    uint64_t      p89si;
    uint64_t      p99si;

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
    CDISPLAY("mode:        %16llu (%llu) instances",  p_m->mode, p_m->moden);
    CDISPLAY("variance:    %16f",    p_m->variance);
    CDISPLAY("std.dev:     %16f",    p_m->stdev);

    /* Quartiles */
    CDISPLAY("p25:         %16llu [%llu]",  p_m->p25, p_m->p25i);
    CDISPLAY("p50:         %16llu [%llu]",  p_m->p50, p_m->p50i);
    CDISPLAY("p75:         %16llu [%llu]",  p_m->p75, p_m->p75i);
    CDISPLAY("p99:         %16llu [%llu]",  p_m->p99, p_m->p99i);

    /* Percentiles */
    CDISPLAY("p99.9:       %16llu [%llu]",  p_m->p39s, p_m->p39si);
    CDISPLAY("p99.99:      %16llu [%llu]",  p_m->p49s, p_m->p49si);
    CDISPLAY("p99.999:     %16llu [%llu]",  p_m->p59s, p_m->p59si);
    CDISPLAY("p99.9999:    %16llu [%llu]",  p_m->p69s, p_m->p69si);
    CDISPLAY("p99.99999:   %16llu [%llu]",  p_m->p79s, p_m->p79si);
    CDISPLAY("p99.999999:  %16llu [%llu]",  p_m->p89s, p_m->p89si);
    CDISPLAY("p99.9999999: %16llu [%llu]",  p_m->p99s, p_m->p99si);

    return;
} /* adts_measures_display() */


static int32_t
adts_measures( uint64_t        *p_arr,
               size_t           elements,
               adts_measures_t *p_m )
{
    int32_t  rc  = 0;
    uint64_t sum = 0;

    adts_measures_init(p_m);

    p_m->p_arr = p_arr;
    p_m->elems = elements;

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

    /* Percentiles */
    //FIXME: There's a bug in this sort function.....
    CDISPLAY("sort-pre");
    adts_sort_quick64(p_arr, p_m->elems);
    CDISPLAY("sort-post");
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

    #if 1
    /* mode */
    {
        uint64_t  limit   = (uint64_t) (p_m->median + 1);
        uint64_t  buckets = limit - p_m->min;
        size_t    bytes   = buckets * sizeof(buckets);
        uint64_t *p_mode  = NULL;
        uint64_t *p_vals  = NULL;

        p_mode = malloc(bytes);
        assert(p_mode);
        if (NULL == p_mode) {
            rc = ENOMEM;
            goto exception;
        }
        memset(p_mode, 0, bytes);

        p_vals = malloc(bytes);
        assert(p_vals);
        if (NULL == p_vals) {
            rc = ENOMEM;
            goto exception;
        }
        memset(p_vals, 0, bytes);

        uint64_t prev = p_arr[0];
        uint64_t curr = 0;
        int32_t  mcnt = 0;
        for (int32_t cnt = 0; cnt <= p_m->elems; cnt++) {
            if (prev != p_arr[cnt]) {
                mcnt++;
            }
            p_vals[mcnt]  = p_arr[cnt];
            p_mode[mcnt] += 1;
            prev          = p_arr[cnt];
        }

        CDISPLAY("Buckets: %llu", buckets);
        for (int32_t cnt = 0; cnt < buckets; cnt++) {
            CDISPLAY("[%2u]: %5llu hits: %llu", cnt, p_vals[cnt], p_mode[cnt]);
            if (p_m->moden < p_mode[cnt]) {
                p_m->mode  = p_vals[cnt];
                p_m->moden = p_mode[cnt];
            }
        }

        CDISPLAY("..............");
        /*
        if (p_mode) {
            free(p_mode);
            p_mode = NULL;
        }
		*/

		/*
        if (p_vals) {
            free(p_vals);
            p_vals = NULL;
        }
		*/
    }
    #endif

    /* Calculated indexes */
    p_m->p25i  = ((25 * (p_m->elems + 1)) / 100) - 1;
    p_m->p50i  = ((55 * (p_m->elems + 1)) / 100) - 1;
    p_m->p75i  = ((75 * (p_m->elems + 1)) / 100) - 1;
    p_m->p99i  = ((99 * (p_m->elems + 1)) / 100) - 1;
    p_m->p39si = ((99.9 * (p_m->elems + 1)) / 100) - 1;
    p_m->p49si = ((99.99 * (p_m->elems + 1)) / 100) - 1;
    p_m->p59si = ((99.999 * (p_m->elems + 1)) / 100) - 1;
    p_m->p69si = ((99.9999 * (p_m->elems + 1)) / 100) - 1;
    p_m->p79si = ((99.99999 * (p_m->elems + 1)) / 100) - 1;
    p_m->p89si = ((99.999999 * (p_m->elems + 1)) / 100) - 1;
    p_m->p99si = ((99.9999999 * (p_m->elems + 1)) / 100) - 1;

    /* Acquire values */
    p_m->p25  = p_arr[p_m->p25i];
    p_m->p50  = p_arr[p_m->p50i];
    p_m->p75  = p_arr[p_m->p75i];
    p_m->p99  = p_arr[p_m->p99i];
    p_m->p39s = p_arr[p_m->p39si];
    p_m->p49s = p_arr[p_m->p49si];
    p_m->p59s = p_arr[p_m->p59si];
    p_m->p69s = p_arr[p_m->p69si];
    p_m->p79s = p_arr[p_m->p79si];
    p_m->p89s = p_arr[p_m->p89si];
    p_m->p99s = p_arr[p_m->p99si];

exception:
    return rc;
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
