
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <unistd.h> /* sleep() */
#include <stdbool.h>
#include <inttypes.h>

/* Toolbox */
#include <services.h>



/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
bool
bit_is_set( int32_t value,
            int32_t bit )
{
    return !!(value & (1 << bit));
} /* bit_is_set() */


/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
bool
bit_is_not_set( int32_t value,
                int32_t bit )
{

    return !(bit_is_set(value, bit));
} /* bit_is_not_set() */


/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
int32_t
bit_clear( int32_t value,
           int32_t bit )
{
    return value & ~(1 << bit);
} /* bit_clear() */


/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
int32_t
bit_clear_msb_to_kth( int32_t value,
                      int32_t bit )
{
    int32_t mask = (1 << bit) - 1;

    return value & mask;
} /* bit_clear_msb_to_kth() */


/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
int32_t
bit_clear_lsb_to_kth( int32_t value,
                      int32_t bit )
{
    int32_t mask = -1 << bit;

    return value & mask;
} /* bit_clear_lsb_to_kth() */


/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
int32_t
bit_toggle( int32_t value,
            int32_t bit )
{
    int32_t mask = (1 << bit);

    return value ^ mask;
} /* bit_toggle() */


/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
int32_t
bit_clear_lsb( int32_t val )
{
    return (val & (val - 1));
} /* bit_clear_lsb() */


/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
int32_t
bit_get_lsb( int32_t val )
{
    return (val & (-val));
} /* bit_get_lsb() */


/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
int32_t
bit_count( int32_t val )
{
    int32_t bits = 0;

    while (val) {
        bits++;
        val = val & (val - 1);
    }

    return bits;
} /* bit_count() */


/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
int32_t
bit_next_largest( uint32_t val )
{
    uint32_t out      = 0;
    uint32_t smallest = 0;
    uint32_t ripple   = 0;
    uint32_t ones     = 0;

    smallest = val & (val * -1);
    ripple   = val + smallest;
    ones     = val ^ ripple;
    ones     = (ones >> 2) / smallest;
    out      = ripple | ones;

    return (int32_t) out;
} /* bit_next_largest() */



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
        int32_t rc     = 0;
        int32_t bit    = 0;
        int32_t arr[]  = {
                          0,
                          2,
                          8,
                          128,
                          1,
                          3,
                          9,
                          -1,
                         };
        size_t   elems = sizeof(arr) / sizeof(int32_t);

        for (size_t cnt = 0; cnt < elems; cnt++) {
            int32_t val = arr[cnt];
            rc = bit_is_set(val, bit);
            CDISPLAY("%3i bit %2i  %s", val, bit, (rc) ? "true" : "false");
        }
    }

    CDISPLAY("=========================================================");
    {
        int32_t rc     = 0;
        int32_t bit    = 0;
        int32_t arr[]  = {
                          0,
                          2,
                          8,
                          128,
                          1,
                          3,
                          9,
                          -1,
                         };
        size_t   elems = sizeof(arr) / sizeof(int32_t);

        for (size_t cnt = 0; cnt < elems; cnt++) {
            int32_t val = arr[cnt];
            rc = bit_is_not_set(val, bit);
            CDISPLAY("%3i bit %2i  %s", val, bit, (rc) ? "true" : "false");
        }
    }

    CDISPLAY("=========================================================");
    {
        int32_t bit    = 0;
        int32_t arr[]  = {
                          0,
                          2,
                          8,
                          128,
                          1,
                          3,
                          9,
                          -1,
                         };
        size_t   elems = sizeof(arr) / sizeof(int32_t);

        for (size_t cnt = 0; cnt < elems; cnt++) {
            int32_t val = arr[cnt];
            int32_t out = bit_clear(val, bit);
            CDISPLAY("%3i bit %2i  %3i", val, bit, out);
        }
    }

    CDISPLAY("=========================================================");
    {
        int32_t bit    = 4;
        int32_t arr[]  = {
                          0,
                          2,
                          8,
                          128,
                          1,
                          3,
                          9,
                          -1,
                         };
        size_t   elems = sizeof(arr) / sizeof(int32_t);

        for (size_t cnt = 0; cnt < elems; cnt++) {
            int32_t val = arr[cnt];
            int32_t out = bit_clear_lsb_to_kth(val, bit);
            CDISPLAY("%3i bit %2i  %3i", val, bit, out);
        }
    }

    CDISPLAY("=========================================================");
    {
        int32_t bit    = 2;
        int32_t arr[]  = {
                          0,
                          2,
                          8,
                          128,
                          1,
                          3,
                          9,
                          -1,
                         };
        size_t   elems = sizeof(arr) / sizeof(int32_t);

        for (size_t cnt = 0; cnt < elems; cnt++) {
            int32_t val = arr[cnt];
            int32_t out = bit_clear_msb_to_kth(val, bit);
            CDISPLAY("%3i bit %2i  %3i", val, bit, out);
        }
    }

    CDISPLAY("=========================================================");
    {
        int32_t bit    = 0;
        int32_t arr[]  = {
                          0,
                          2,
                          8,
                          128,
                          1,
                          3,
                          9,
                          -1,
                         };
        size_t   elems = sizeof(arr) / sizeof(int32_t);

        for (size_t cnt = 0; cnt < elems; cnt++) {
            int32_t val = arr[cnt];
            int32_t out = bit_toggle(val, bit);
            CDISPLAY("%3i bit %2i  %3i", val, bit, out);
        }
    }

    CDISPLAY("=========================================================");
    {
        int32_t arr[]  = {
                          0,
                          2,
                          8,
                          128,
                          1,
                          3,
                          9,
                          -1,
                         };
        size_t   elems = sizeof(arr) / sizeof(int32_t);

        for (size_t cnt = 0; cnt < elems; cnt++) {
            int32_t val = arr[cnt];
            int32_t out = bit_clear_lsb(val);
            CDISPLAY("%3i ->  %3i", val, out);
        }
    }


    CDISPLAY("=========================================================");
    {
        int32_t arr[]  = {
                          0,
                          2,
                          8,
                          128,
                          1,
                          3,
                          9,
                          -1,
                         };
        size_t   elems = sizeof(arr) / sizeof(int32_t);

        for (size_t cnt = 0; cnt < elems; cnt++) {
            int32_t val = arr[cnt];
            int32_t out = bit_get_lsb(val);
            CDISPLAY("%3i ->  %3i", val, out);
        }
    }

    CDISPLAY("=========================================================");
    {
        int32_t arr[]  = {
                          0,
                          2,
                          8,
                          128,
                          15,
                          3,
                          9,
                          -1,
                         };
        size_t   elems = sizeof(arr) / sizeof(int32_t);

        for (size_t cnt = 0; cnt < elems; cnt++) {
            int32_t val = arr[cnt];
            int32_t out = bit_count(val);
            CDISPLAY("%3i ->  %3i", val, out);
        }
    }

    CDISPLAY("=========================================================");
    {
        int32_t arr[]  = {
                          0,
                          2,
                          8,
                          128,
                          15,
                          3,
                          9,
                          -1,
                         };
        size_t   elems = sizeof(arr) / sizeof(int32_t);

        for (size_t cnt = 0; cnt < elems; cnt++) {
            int32_t  val = arr[cnt];
            uint32_t out = bit_next_largest(val);
            CDISPLAY("%3i ->  %3i", val, out);
        }
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
utest_services_bits( void )
{

    utest_control();

    return;
} /* utest_services_bits() */
