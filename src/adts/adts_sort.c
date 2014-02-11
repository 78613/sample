

#include <errno.h>
#include <assert.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include <inttypes.h>

/* Toolbox */
#include <adts_sort.h>
#include <adts_private.h>
#include <adts_services.h>



/******************************************************************************
 #####  ####### ######  #     #  #####  ####### #     # ######  #######  #####
#     #    #    #     # #     # #     #    #    #     # #     # #       #     #
#          #    #     # #     # #          #    #     # #     # #       #
 #####     #    ######  #     # #          #    #     # ######  #####    #####
      #    #    #   #   #     # #          #    #     # #   #   #             #
#     #    #    #    #  #     # #     #    #    #     # #    #  #       #     #
 #####     #    #     #  #####   #####     #     #####  #     # #######  #####
******************************************************************************/


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
 ****************************************************************************
 */
static inline void
swap32( int32_t *p_a,
        int32_t *p_b )
{
    int32_t tmp = *p_a;

    *p_a = *p_b;
    *p_b = tmp;

    return;
} /* swap32() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
static inline int32_t
sort_pivot_select( int32_t a,
                   int32_t b )
{
    return (a + b) / 2;
} /* sort_pivot_select() */


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
 *  Time:  O(n*log(n)
 *  Space: O(n)
 ****************************************************************************
 */
static inline void
sort_quick_recursive( int32_t arr[],
                      size_t  head,
                      size_t  tail )
{
    if (head < tail) {
        int32_t hidx  = 0; /* temp head */
        int32_t tidx  = 0; /* temp tail */
        int32_t key   = 0;
        int32_t pivot = sort_pivot_select(head, tail);

        /* swap the head with the pivot point */
        swap32(&(arr[head]), &(arr[pivot]));

        key = arr[head];
        hidx  = head + 1;
        tidx  = tail;
        while (hidx <= tidx) {
            while ((hidx <= tail) && (arr[hidx] <= key)) {
                hidx++;
            }

            while ((tidx >= head) && (arr[tidx] > key)) {
                tidx--;
            }

            if (hidx < tidx) {
                swap32(&(arr[hidx]), &(arr[tidx]));
            }
        }
        /* swap two elements */
        swap32(&(arr[head]), &(arr[tidx]));

        /* recursively sort the lesser arr */
        sort_quick_recursive(arr, head, tidx - 1);
        sort_quick_recursive(arr, tidx + 1, tail);
    }

    return;
} /* sort_quick_recursive() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
static inline void
sort_quick_iterative( int32_t arr[],
                      size_t  elems )
{
    CDISPLAY("Incomplete...");

    return;
} /* sort_quick_iterative() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
void
adts_sort_quick_r( int32_t arr[],
                   size_t  elems )
{
    sort_quick_recursive(arr, 0, (elems - 1));
    return;
} /* adts_sort_quick_r() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
void
adts_sort_quick_i( int32_t arr[],
                   size_t  elems )
{
    sort_quick_iterative(arr, elems);
    return;
} /* adts_sort_quick_r() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
void
sort_merge( int32_t arr[],
            int32_t tmp[],
            int32_t left,
            int32_t mid,
            int32_t right )
{
    int32_t tmp_pos  = (right - left) + 1;
    int32_t elems    = left;
    int32_t left_end = mid - 1;

    while ((left <= left_end) && (mid <= right)) {
        if (arr[left] <= arr[mid]) {
            tmp[tmp_pos] = arr[left];
            tmp_pos     += 1;
            left        += 1;
        }else {
            tmp[tmp_pos] = arr[mid];
            tmp_pos     += 1;
            mid         += 1;
        }
    }

    while (left <= left_end) {
        tmp[tmp_pos] = arr[left];
        left        += 1;
        mid         += 1;
    }

    while (mid <= right) {
        tmp[tmp_pos] = arr[mid];
        mid         += 1;
        tmp_pos     += 1;
    }

    for (int32_t i = 0; i< elems; i++) {
        arr[right] = tmp[right];
        right     -= 1;
    }

    return;
} /* sort_merge() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
void
sort_merge_r( int32_t arr[],
              int32_t tmp[],
              int32_t left,
              int32_t right )
{
    /* recursively sort and merge the array until left and right indexes meet */
    if (left < right) {
        int32_t mid = (left + right) / 2;

        /* recursively sort the left side up to middle point */
        sort_merge_r(arr, tmp, left, mid);

        /* recursively sort from middle to right side */
        sort_merge_r(arr, tmp, (mid + 1), right);

        sort_merge(arr, tmp, left, (mid + 1), right);
    }

    return;
} /* sort_merge_r() */


/*
 ****************************************************************************
 *
 *  Time:  O(n*log(n)
 *  Space: O(n)
 ****************************************************************************
 */
int32_t
adts_sort_merge_r( int32_t arr[],
                   size_t  elems )
{
    int32_t  rc    = 0;
    int32_t *p_tmp = NULL;

    /* Merge sort requires O(n) space, thus allocate here */
    p_tmp = calloc(1, sizeof(arr));
    if (NULL == p_tmp) {
        rc = EINVAL;
        goto exception;
    }

    /* Pass the temp array along with start end indexes */
    sort_merge_r(arr, p_tmp, 0, (elems - 1));

    CDISPLAY("FIXME!!!!!");

exception:
    if (p_tmp) {
        free(p_tmp);
    }

    return rc;
} /* adts_sort_merge_r() */



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
        CDISPLAY("Test: quicksort ");
        const int32_t arr[] = {1,9,2,8,3,7,4,6,5,0,99};
        const size_t  elems = sizeof(arr) / sizeof(arr[0]);

        adts_array_display(arr, elems);
        adts_sort_quick_r(arr, elems);
        printf("\n");
        adts_array_display(arr, elems);

    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: merge ");
        const int32_t arr[] = {1,9,2,8,3,7,4,6,5,0,99};
        const size_t  elems = sizeof(arr) / sizeof(arr[0]);

        adts_array_display(arr, elems);
        adts_sort_merge_r(arr, elems);
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
