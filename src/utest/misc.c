
#include <adts.h>
#include <utest.h>



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
 *  Describe a function that takes an int value, and returns true if the
 *  bit pattern of that int value is the same if you reverse it (i.e. it's
 *  a palindrome); i.e. boolean isPalindrome(int x)
 *
 ****************************************************************************
 */
bool
is_binary_palindrome( int32_t val )
{
    bool    rc  = false;
    int32_t out = 0;

    if ((-1 == val) || (0 == val)) {
        return true;
    }

    /* reverse bits */
    for (int32_t i = 0; i < 32; i++) {
        out |= !!(val & (1 << i)) << (31 - i);
    }

    /* shift right */
    for (int32_t i = 0; i < 32; i++) {
        if (out & (1 << i)) {
            /* done */
            break;
        }

        out = out >> 1;
    }

    return (val == out);
} /* is_binary_palindrome() */





/*
 ****************************************************************************
 *  Format an RGB value (three 1-byte numbers) as a 6-digit hexadecimal string
 *
 ****************************************************************************
 */
char
i2c( char i )
{
    char arr[] = {'0','1','2','3','4','5','6','7','8','9',
                  'A','B','C','D','E','F'};

    return arr[i];
} /* i2c() */

int32_t
rgb_to_string( uint8_t r,
               uint8_t g,
               uint8_t b )
{
    size_t bytes = 9; // "0xRRGGBB\0"
    char  *p_out = calloc(1, bytes);

    assert(p_out);

    p_out[0] = '0';
    p_out[1] = 'x';
    p_out[2] = i2c((r & 0xF0) >> 4);
    p_out[3] = i2c(r & 0x0F);
    p_out[4] = i2c((g & 0xF0) >> 4);
    p_out[5] = i2c(g & 0x0F);
    p_out[6] = i2c((b & 0xF0) >> 4);
    p_out[7] = i2c(b & 0x0F);
    p_out[8] = '\0';

    return p_out;
} /* rgb_to_string() */


/*
 ****************************************************************************
 *  find largest value in an int array
 *
 ****************************************************************************
 */
int32_t
max_arr_value( const int32_t arr[],
               const size_t  elems )
{
    int32_t max = arr[0];

    for (int32_t i = 1; i < elems; i++) {
        max = (arr[i] > max) ? arr[i] : max;
    }

    return max;
} /* max_arr_value() */



/*
 ****************************************************************************
 *  Write function to print the odd numbers from 1 to 99
 *
 ****************************************************************************
 */
void
odd_num_display( size_t limit )
{
    for (int32_t i = 1; i <= limit; i += 2) {
        printf(" %3i ", i);

        /* eyecandy */
        if (9 == (i % 10)) {
            printf("\n");
        }
    }

    return;
} /* odd_num_display() */


/*
 ****************************************************************************
 *  Print out the grade-school multiplication table up to 12x12
 *
 ****************************************************************************
 */
void
multiplication_table( size_t limit )
{
    for (int32_t j = 1; j <= limit; j++) {
        for (int32_t k = 1; k <= limit; k++) {
            printf("%3i ", j * k);
        }
        printf("\n");
    }

    return;
} /* multiplication_table() */



/*
 ****************************************************************************
 * Iteratively generate the fibonacci numbers up to the specified term.  Where
 * term refers to the 0-based offset in the series.  Return the value
 * corresponding to that term.
 *
 ****************************************************************************
 */
size_t
fibonacci_i( size_t term )
{
    size_t curr = 0;
    size_t prev = 0;
    int32_t idx = 2;

    if (0 == term) {
        return 0;
    }

    if (1 == term) {
        return 1;
    }

    prev = 0;
    curr = 1;
    while (term >= idx) {
        int32_t x = prev + curr;

        prev = curr;
        curr = x;

        idx++;
    }

    return curr;
} /* fibonacci_i() */


/*
 ****************************************************************************
 * Validate if all opening and closing brackes are valid
 * - () () {()} {()[]}
 *
 *
 ****************************************************************************
 */
static bool
scope_compare( char curr,
               char prev )
{
    bool rc = false;

    switch (curr) {
        case ']':
            rc = !('[' == prev);
            break;
        case '}':
            rc = !('{' == prev);
            break;
        case ')':
            rc = !('(' == prev);
            break;
        default:
            /* compare expected but not matched */
            rc = true;
            break;
    }

    return rc;
} /* scope_compare() */



bool
scope_invalid( char *p_str )
{
    assert(p_str);

    bool          rc      = false;
    char         *p_run   = p_str;
    adts_stack_t *p_stack = adts_stack_create();

    while ('\0' != *p_run) {
        char tmp = 0;

        if (('[' == *p_run) || ('{' == *p_run) || ('(' == *p_run)) {
            adts_stack_push(p_stack, *p_run, sizeof(*p_run));
        }
        else if ((']' == *p_run) || ('}' == *p_run) || (')' == *p_run)) {
            /* Close scopes found, now pop from stack to compare */
            tmp = adts_stack_pop(p_stack);
            rc  = scope_compare(*p_run, tmp);
            if (rc) {
                goto exception;
            }
        }
        p_run++;
    }

    if (adts_stack_is_not_empty(p_stack)) {
        /* stack should always be empty on completion to detect unclosed
         * scope values. */
        rc = true;
    }

exception:
    adts_stack_destroy(p_stack);
    return rc;
} /* scope_invalid() */



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
        CDISPLAY("Test: fibonacci iterative");
        size_t val = 0;
        val = fibonacci_i(10);
        CDISPLAY("Output: %d", val);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: display 12x12 multiplcation table");
        size_t limit = 12;
        multiplication_table(limit);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: display odd numbers up to n");
        size_t limit = 99;
        odd_num_display(limit);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: max array value");
        int32_t arr[] = { 0, -5, 55, 34532, -555555, 1024 };
        size_t  elems = sizeof(arr) / sizeof(arr[0]);
        int32_t max   = 0;

        max = max_arr_value(arr, elems);
        CDISPLAY("Output: %i", max);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: RGB to string");
        int32_t  R[]   = {0, 1, 55, 0xFF};
        int32_t  G[]   = {0, 1, 55, 0xFF};
        int32_t  B[]   = {0, 1, 55, 0xFF};
        size_t   elems = sizeof(R) / sizeof(R[0]);
        char    *p_rgb = NULL;

        for (int32_t i = 0; i < elems; i++) {
            CDISPLAY("Input:  %2x %2x %2x", R[i], G[i], B[i]);
            p_rgb = rgb_to_string(R[i], G[i], B[i]);
            CDISPLAY("Output: %s", p_rgb);
        }
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: binary palindrome");
        int32_t arr[] = { 0, -1, -5, 55, 34532, -555555, 1024 };
        size_t  elems = sizeof(arr) / sizeof(arr[0]);

        for (int32_t i = 0; i < elems; i++) {
            int32_t val = arr[i];
            CDISPLAY("Input:  %i", val);
            bool rc = is_binary_palindrome(val);
            CDISPLAY("Output: %i", rc);
        }
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: scope validity");

        #define X  (256)
        #define Y  (8)

        char    arr[Y][X] = {
                             {""},
                             {"()"},
                             {"() {()} [({})]"},
                             {"[] ({} ()) [() {()}]"},
                             {"[1+2] (3 * {4 + 5} + (6 + 7)) - [() {()}]"},
                             {")"},
                             {"()("},
                             {"{[)"},
                            };
        size_t  elems     = Y;

        for (int32_t i = 0; i < elems; i++) {
            char *p_ws = &(arr[0, i]);

            printf("\n");
            CDISPLAY("[%i] Input:  %s", i, p_ws);
            bool rc = scope_invalid(p_ws);
            CDISPLAY("[%i] Output: %i", i, rc);
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
utest_misc( void )
{
    CDISPLAY("=========================================================");

    utest_control();

    return;
} /* utest_misc() */
