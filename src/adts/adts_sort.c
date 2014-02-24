

#include <errno.h>
#include <assert.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h> /* rand */
#include <stdbool.h>
#include <inttypes.h>

/* Toolbox */
#include <adts_sort.h>
#include <adts_private.h>
#include <adts_services.h>


/*
 ****************************************************************************
 *  Future work items:
 *    - The sorting function need to be designed as follows:
 *      - accept consumer callback function for comparison
 *        - for example , consumer func needed if sorting dates or times
 *      - provide a list of available callback function for user selection
 *        for the simple well known datatypes
 *      - sort ascending and decending based on input flags
 *      - manage linked list and array type sorting
 *      - see Java comparable service for examples (-1, 0, 1) inteface
 ****************************************************************************
 */


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
 * \detail
 *    For sorting algorithms where small arrays have too much processing
 *    overhead, we create a threahold such as to perform a simpler sort.
 ****************************************************************************
 */
#define SORT_THREASHOLD (8)


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
adts_shuffle( int32_t arr[],
              size_t  elems )
{
    for (int32_t i = 0; i < elems; i++) {
        //FIXME: Using % i results in pseudo - random.  Research this further
        int32_t r = rand() % (i + 1);
        SWAP(&arr[i], &arr[r], int32_t);
    }

    return;
} /* adts_shuffle() */



/*
 ****************************************************************************
 *
 ****************************************************************************
 */
bool
adts_arr_is_not_sorted( int32_t arr[],
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
} /* adts_arr_is_not_sorted() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
bool
adts_arr_is_sorted( int32_t arr[],
                    size_t  elems )
{
    return !(adts_arr_is_not_sorted(arr, elems));
} /* adts_arr_is_sorted() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
void
adts_array_display( int32_t       arr[],
                    const size_t  elems )
{
    size_t digits = adts_digits_decimal(elems);

    for (size_t idx = 0; idx < elems; idx++) {
        printf("[%*d]  data: %016p  %d \n",
                digits,
                idx,
                arr[idx],
                arr[idx]);
    }

    return;
} /* adts_array_display() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
void
adts_sort_shell( int32_t       arr[],
                 const size_t  elems )
{
    int32_t h = 1;

    while (h < (elems / 3)) {
        /* Initial increment sequence */
        h = (3 * h) + 1;
    }

    /* sort */
    while (h >= 1) {
        for (int32_t i = h; i < elems; i++) {
            for (int32_t j = i; (j >= h) && (arr[j] < arr[j - h]); j -= h) {
                SWAP(&arr[j], &arr[j - h], int32_t);
            }
        }

        /* mode to next increment */
        h = h / 3;
    }

    return;
} /* adts_sort_shell() */



/*
 ****************************************************************************
 * \details
 *   given an array with pre-determined bounds, perform a sort of the members
 *   within the pre-defined bounds.
 *
 ****************************************************************************
 */
void
adts_sort_shell_ext( int32_t arr[],
                     size_t  lo,
                     size_t  hi )
{
    int32_t elems = hi - lo;

    adts_sort_shell(&arr[lo], elems);

    return;
} /* adts_sort_shell_ext() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
void
sort_merge_merge( int32_t arr[],
                  int32_t aux[],
                  int32_t lo,
                  int32_t mid,
                  int32_t hi )
{
    /* copy */
    for (int32_t k = lo; k <= hi; k++) {
        aux[k] = arr[k];
    }

    int32_t i = lo;
    int32_t j = mid + 1;

    /* merge */
    for (int32_t k = lo; k <= hi; k++) {
        if (i > mid) {
            arr[k] = aux[j];
            j++;
        }else if (j > hi) {
            arr[k] = aux[i];
            i++;
        }else if (aux[j] < arr[i]) {
            arr[k] = aux[j];
            j++;
        }else {
            arr[k] = aux[i];
            i++;
        }
    }

    return;
} /* sort_merge_merge() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
static void
sort_merge_sort( int32_t arr[],
                 int32_t aux[],
                 int32_t lo,
                 int32_t hi )
{
    if (hi <= lo) {
        /* done */
        goto exception;
    }

    /* small arrays are more efficiently processed with elementary sorts */
    if (hi <= (lo + SORT_THREASHOLD - 1)) {
        adts_sort_shell_ext(arr, lo, hi);
        goto exception;
    }

    int32_t mid = lo + (hi - lo) / 2;

    sort_merge_sort(arr, aux, lo, mid);      /* soft left */
    sort_merge_sort(arr, aux, mid + 1, hi);  /* sort right */
    sort_merge_merge(arr, aux, lo, mid, hi); /* merge results */

exception:
    return;
} /* sort_merge_sort() */


/*
 ****************************************************************************
 *
 *  Time:  O(n*log(n)
 *  Space: O(n)
 ****************************************************************************
 */
int32_t
adts_sort_merge( int32_t arr[],
                 size_t  elems )
{
    int32_t  rc    = 0;
    int32_t *p_tmp = NULL;

    /* Merge sort requires O(n) space, thus allocate here */
    p_tmp = calloc(elems, sizeof(arr[0]));
    if (NULL == p_tmp) {
        rc = EINVAL;
        goto exception;
    }

    /* Pass the temp array along with start end indexes */
    sort_merge_sort(arr, p_tmp, 0, (elems - 1));

exception:
    if (p_tmp) {
        free(p_tmp);
    }

    return rc;
} /* adts_sort_merge() */



/*
 ****************************************************************************
 *
 ****************************************************************************
 */
static int32_t
sort_quick_partition( int32_t arr[],
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

        SWAP(&arr[i], &arr[j], int32_t);
    }

    /* swap with partition item */
    SWAP(&arr[lo], &arr[j], int32_t);

    /* Index of item now in place */
    return j;
} /* sort_quick_partition() */



/*
 ****************************************************************************
 *
 ****************************************************************************
 */
static void
sort_quick( int32_t arr[],
            size_t  lo,
            size_t  hi )
{
    if (hi <= lo) {
        /* done */
        goto exception;
    }

    /* small arrays are more efficiently processed with elementary sorts */
    if (hi <= (lo + SORT_THREASHOLD - 1)) {
        adts_sort_shell_ext(arr, lo, hi);
        goto exception;
    }

    int32_t j = sort_quick_partition(arr, lo, hi);

    sort_quick(arr, lo, j - 1);
    sort_quick(arr, j + 1, hi);

exception:
    return;
} /* sort_quick() */


/*
 ****************************************************************************
 *
 *  Time:  O(?)
 *  Space: O(1)
 ****************************************************************************
 */
void
adts_sort_quick( int32_t arr[],
                 size_t  elems )
{
    /* shuffle to probabilistically eliminate the N^2 worst case scenario */
    adts_shuffle(arr, elems);

    /* start the sort recursion */
    sort_quick(arr, 0, elems - 1);

    return;
} /* adts_sort_quick() */



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
 * test control
 *
 ****************************************************************************
 */
static void
utest_control( void )
{

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: display ");
        const int32_t arr[] = {1,9,2,8,3,7,4,6,5,0,99};
        const size_t  elems = sizeof(arr) / sizeof(arr[0]);

        adts_array_display(arr, elems);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: unsorted array ");
        const int32_t arr[] = {1,9,2,8,3,7,4,6,5,0,99};
        const size_t  elems = sizeof(arr) / sizeof(arr[0]);
        bool          rc    = false;

        adts_array_display(arr, elems);
        rc = adts_arr_is_sorted(arr, elems);
        assert(false == rc);

        rc = adts_arr_is_not_sorted(arr, elems);
        assert(rc);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: sorted array ");
        const int32_t arr[] = {3,4,5,5,5,8,9,10,99};
        const size_t  elems = sizeof(arr) / sizeof(arr[0]);
        bool          rc    = false;

        adts_array_display(arr, elems);
        rc = adts_arr_is_sorted(arr, elems);
        assert(rc);

        rc = adts_arr_is_not_sorted(arr, elems);
        assert(false == rc);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: shuffle");
        int32_t       rc    = 0;
        int32_t       arr[] = {3,4,5,5,5,8,9,10,99};
        const size_t  elems = sizeof(arr) / sizeof(arr[0]);

        adts_array_display(arr, elems);
        adts_shuffle(arr, elems);
        printf("\n");
        adts_array_display(arr, elems);

    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: shell sort");
        int32_t       rc    = 0;
        int32_t       arr[] = {1,9,2,8,3,7,4,6,5,0,99};
        const size_t  elems = sizeof(arr) / sizeof(arr[0]);

        adts_array_display(arr, elems);
        adts_sort_shell(arr, elems);
        printf("\n");
        adts_array_display(arr, elems);

    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: merge sort");
        int32_t       rc    = 0;
        int32_t       arr[] = {1,9,2,8,3,7,4,6,5,0,99};
        const size_t  elems = sizeof(arr) / sizeof(arr[0]);

        adts_array_display(arr, elems);
        rc = adts_sort_merge(arr, elems);
        assert(0 == rc);
        printf("\n");
        adts_array_display(arr, elems);
    }
return;
    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: quick sort");
        int32_t       rc    = 0;
        int32_t       arr[] = {1,9,2,8,3,7,4,6,5,0,99};
        const size_t  elems = sizeof(arr) / sizeof(arr[0]);

        adts_array_display(arr, elems);
        adts_sort_quick(arr, elems);
        printf("\n");
        adts_array_display(arr, elems);
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
utest_adts_sort( void )
{
    utest_control();

    return;
} /* utest_adts_sort() */
