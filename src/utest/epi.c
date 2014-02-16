


#include <errno.h>
#include <assert.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include <inttypes.h>

/* Toolbox */
#include <adts_private.h>
#include <adts_services.h>


static size_t
get_bytes( char *p_str )
{
    size_t bytes = 0;

    while ('\0' != *p_str) {
        bytes++;
        p_str++;
    }

    return bytes;
} /* get_bytes() */


void
abs_test( int32_t a,
          int32_t b )
{
    a = -15;
    b = -5;
    CDISPLAY("Output1: %i", abs(a - b));
    CDISPLAY("Output1: %i", abs(b - a));

    return;
} /* abs_test() */



/*
 ****************************************************************************
 *
 *
 * Time:  O(n2)  //count and convert each char
 * Space: O(1)
 ****************************************************************************
 */
int32_t
epi_c5_ex58_excel_col_to_int( char *p_str )
{
    CDISPLAY("Input:  %s", p_str);

    int32_t val   = 0;
    int32_t pow   = 1;
    size_t  bytes = get_bytes(p_str);

    for (int32_t idx = (bytes - 1); idx >= 0; idx--) {

        int32_t tmp = p_str[idx] - 'A';

        tmp += 1;   /* ensure start at 1 */
        tmp *= pow;
        val += tmp;
        pow *= 26;
    }

    CDISPLAY("Output: %d", val);
    return val;
} /* epi_c5_ex58_excel_col_to_int() */



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
        CDISPLAY("Test: convert excel columns to integer");
        #define X (8)
        #define Y (32)
        char arr[Y][X] = {
                          {"A"},
                          {"AA"},
                          {"ZZ"},
                          {"Z"}
                          };
        for (int32_t i = 0; i < X; i++) {
            char *p_str = &(arr[0,i]);
            epi_c5_ex58_excel_col_to_int(p_str);
        }
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: absolute value test");
        #define X (8)
        #define Y (32)
        char arr[Y][X] = {
                          {  0,  1},
                          { 15, -5},
                          {-32,  8},
                          { 20,  0}
                          };
        for (int32_t i = 0; i < X; i++) {
            int32_t a = *(char *) &(arr[0,i]);
            int32_t b = *(char *) &(arr[1,i]);
            //CDISPLAY("view: %i %i", a, b);
            //CDISPLAY("view: %i", *(char *) &(arr[0,i]));
            abs_test(a, b);
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
utest_epi( void )
{
    CDISPLAY("=========================================================");

    utest_control();

    return;
} /* utest_epi() */
