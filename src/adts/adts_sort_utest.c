

#include <errno.h>
#include <assert.h>

#include <adts_sort.h>
#include <adts_hexdump.h>
#include <adts_services.h>



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
        CDISPLAY("Test: shuffle with duplicates");
        int32_t       rc    = 0;
        int32_t       arr[] = {3,4,5,5,5,8,9,10,99};
        const size_t  elems = sizeof(arr) / sizeof(arr[0]);

        adts_array_display(arr, elems);
        adts_shuffle(arr, elems);
        printf("\n");
        adts_array_display(arr, elems);
        rc = adts_arr_is_not_sorted(arr, elems);
        assert(rc);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: shuffle no duplicates");
        int32_t       rc    = 0;
        int32_t       arr[] = {0,1,2,3,4,5,6,7,8,9};
        const size_t  elems = sizeof(arr) / sizeof(arr[0]);

        adts_array_display(arr, elems);
        adts_shuffle(arr, elems);
        printf("\n");
        adts_array_display(arr, elems);
        rc = adts_arr_is_not_sorted(arr, elems);
        assert(rc);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: sort insertion");
        int32_t       rc    = 0;
        int32_t       arr[] = {1,9,2,8,3,7,4,6,5,0,99};
        const size_t  elems = sizeof(arr) / sizeof(arr[0]);

        adts_array_display(arr, elems);
        adts_sort_insertion(arr, elems);
        printf("\n");
        adts_array_display(arr, elems);
        rc = adts_arr_is_sorted(arr, elems);
        assert(rc);
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
        rc = adts_arr_is_sorted(arr, elems);
        assert(rc);
    }

#if 0
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
        rc = adts_arr_is_sorted(arr, elems);
        assert(rc);
    }
#endif

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
        rc = adts_arr_is_sorted(arr, elems);
        assert(rc);
    }

#if 0
    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: quick sort 3way");
        int32_t       rc    = 0;
        int32_t       arr[] = {1,9,2,8,3,7,4,6,5,0,99};
        const size_t  elems = sizeof(arr) / sizeof(arr[0]);

        adts_array_display(arr, elems);
        adts_sort_quick_3way(arr, elems);
        printf("\n");
        adts_array_display(arr, elems);
        rc = adts_arr_is_sorted(arr, elems);
        //assert(rc);
    }
#endif

    return;
} /* utest_control() */



/*
 ****************************************************************************
 * test public entrypoint
 *
 ****************************************************************************
 */
void
utest_adts_sort_public( void )
{
    utest_control();

    return;
} /* utest_adts_sort_public() */
