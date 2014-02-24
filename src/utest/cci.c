
#include <adts.h>
#include <utest.h>


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
   ####### #     # #     #  #####  #######   ###   ####### #     #  #####
   #       #     # ##    # #     #    #       #    #     # ##    # #     #
   #       #     # # #   # #          #       #    #     # # #   # #
   #####   #     # #  #  # #          #       #    #     # #  #  #  #####
   #       #     # #   # # #          #       #    #     # #   # #       #
   #       #     # #    ## #     #    #       #    #     # #    ## #     #
   #        #####  #     #  #####     #      ###   ####### #     #  #####
******************************************************************************/


/*
 * You are given two 32-bit numbers, N and M, and two bit positions,
 * i and j. Write a method to set all bits between i and j in N equal
 * to M (e.g., M becomes a substring of N located at i and starting at j).
 *
 * EXAMPLE:
 *
 * Input:  N = 10000000000,
 *         M = 10101,
 *         i = 2,
 *         j = 6
 *
 * Output: N = 10001010100
 *
 */
// imask
// 0x000001000000
// 0x000000111111
// 0x111111000000
//
// jmask
// 0x000000001000
// 0x000000000111
//
// mask
// 0x111111000000
// 0x000000000111
// --------------
// 0x111111000111

static int32_t
c5e1( uint32_t n,
      uint32_t m,
      int32_t  i,
      int32_t  j )
{
    CDISPLAY("Input: ");
    CDISPLAY("i: %d  j: %d", i, j);
    adts_bit_display_32(n);
    adts_bit_display_32(m);

    uint32_t out   = 0;
    uint32_t imask = ~((1 << i) - 1);
    uint32_t jmask = (1 << j) - 1;
    uint32_t mask  = imask | jmask;

    out  = n & mask;  /* cleared space */
    out |= m << j;    /* insert m */


    CDISPLAY("Output: ");
    adts_bit_display_32(out);
    return out;
} /* c5e1() */



/*
 * Given a (decimal - e.g. 3.72) number that is passed in as a string, print
 * the binary representation. If the number can not be represented accurately
 * in binary, print “ERROR”
 *
 * Method:
 *   - validate each char for valid digit or decimal
 *     - if not 0-9 || . || - then display error
 *   - convert string to decimal
 *   - convert decimal to binary
 */
static bool
is_digit( char c )
{
    return (('0' <= c) && ('9' >= c));
} /* is_digit() */

static bool
is_digit_aux( char c )
{
    bool rc = false;

    switch (c) {
        case '-':
        case '.':
            rc = true;
            break;
        default:
            break;
    }

    return rc;
} /* is_digit_aux() */

static void
c5e2( char *p_str )
{
    CDISPLAY("Input:  %s", p_str);

    size_t  bytes = strlen(p_str);
    int32_t val   = 0;
    int32_t tmp   = 0;
    int32_t pow   = 1;

    /* convert to base10 */
    for (int32_t idx = bytes - 1; idx >= 0; idx--) {
        char c = p_str[idx];
        if ((false == is_digit(c)) &&
            (false == is_digit_aux(c))) {
            CDISPLAY("ERROR");
            goto exception;
        }

        tmp  = c - '0';
        tmp *= pow;
        val += tmp;
        pow *= 10;
    }

    CDISPLAY("Output: %d", val);

    char bits[33] = {0};
    for (int32_t idx = 0; idx < 32; idx++) {
        bool b = !!(val & (1 << idx));
        bits[(31 - idx)] = (b) ? '1' : '0';
    }
    bits[32] = '\n';

    CDISPLAY("Output: 0b%s", bits);


exception:
    return;
} /* c5e2() */



/******************************************************************************
   #     # #     #   ###   ####### ####### #######  #####  #######  #####
   #     # ##    #    #       #       #    #       #     #    #    #     #
   #     # # #   #    #       #       #    #       #          #    #
   #     # #  #  #    #       #       #    #####    #####     #     #####
   #     # #   # #    #       #       #    #             #    #          #
   #     # #    ##    #       #       #    #       #     #    #    #     #
    #####  #     #   ###      #       #    #######  #####     #     #####
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
        c5e1(-1, 255, 16, 0);
    }

    CDISPLAY("=========================================================");
    {
        c5e2("255");
        c5e2("2X5");
        c5e2("256");

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
utest_cci( void )
{
    CDISPLAY("=========================================================");

    utest_control();

    return;
} /* utest_cci() */
