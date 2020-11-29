
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
 ****************************************************************************
 * a. Implement an algorithm do determine if a string has all unique characters.
 * b. what if you cannot use additional data structures?
 *
 ****************************************************************************
 */
// O(n) time complexity  -> n scaling with input length
// O(1) space complexity -> temp space required, constant at char/8b range
static bool
cci_c1_1a( char     *p_str,
           uint32_t  bytes )
{
    char  arr[256] = {0};
    char *p_c      = p_str;
    bool  rc       = false;

    for (int32_t i = 0; i < bytes; i++) {
        if (*p_c == '\0') {
            rc = true;
            goto exception;
        }

        if (arr[*p_c]) {
            /* collision, string is not unique. */
            goto exception;
        }
        arr[*p_c]++;   /* Accounting */
        p_c++;         /* Next character */
    }
    rc = true;

exception:
    return rc;
} /* cci_c1_1a() */


// O(n^2) time complexity  -> quadratic classic nested for loops. Approximating n^2
// O(1)   space complexity -> No temp space required.
static bool
cci_c1_1b( char     *p_str,
           uint32_t  bytes )
{
    for (int32_t i = 0; i < bytes; i++) {
        for (int32_t j = (i + 1); j < bytes; j++) {
            if (p_str[i] == p_str[j]) {
                return false;
            }
        }
    }

    return true;
} /* cci_c1_1b() */


/*
 ****************************************************************************
 * a. Write a function reserve(char *str) which reverses a null-terminated string
 * Notes:
 *   Get bytes without the \n terminator since we need it for a valid reversed string
 *   Set ps as leftmost ptr moving to right.
 *   Set pe as rightmost ptr moving to left.
 *   if ps == pe, then we've encountered an odd string and we're in the middle, thus done
 *   if ps > pe, then we've encoutnered an even string and we're in the middle, thus done
 *   optimized as ps >= pe...
 *
 * Testcase "" breaks must be accounted for.
 *
 ****************************************************************************
 */
// O(n/2) time complexity  -> start and end processed simultaneously
// O(1)   space complexity -> one temp variable required, in place swap
static char *
cci_c1_2( char *p_str )
{
    int32_t   bytes = strlen(p_str) - 1;
    char     *ps    = &p_str[0];
    char     *pe    = &p_str[bytes];
    char      tmp   = 0;

    for (;;) {
        if (ps >= pe) {
            goto exception;
        }

        if (pe < ps) {
            goto exception;
        }

        tmp = *ps;
        *ps = *pe;
        *pe = tmp;

        ps++;
        pe--;
    }

exception:
    return p_str;
} /* cci_c1_2() */


/*
 ****************************************************************************
 * Given two strings, write a method to determine if one is a permutaiton of the other
 * proposals:
 * 1) Math permutation shortcut it to assign a value to each letter and multiply
 *    -> has risk of overflowing long long with too long of a string
 * 2) Create lookup table and compare values
 *    -> overflow on each entry can be mitigated to reasonable abount by entry size (uint64_t)
 *
 * First determine the string length.  If A and B don't match, then permutation = false, because
 * permutation != substring.
 *  - strlen on A and B to get size is inneficient for large strings.
 *  -> Better to walk both strings and detect assymetry and exit.
 *
 * Tests...
 *  - abcd   cbad
 *  - aaabb  aabbb
 *  - abc    abcd
 *  - ""     abc
 *  - ""     ""
 *  - \n     \n
 *  - abc    \n
 *
 ****************************************************************************
 */
// O(n) time complexity  -> strings processed simultaneously + check of 256 entry array.
// O(1) space complexity -> constant, only need two 256 temp arrays.
static bool
cci_c1_3a( char *p_stra,
           char *p_strb )
{
    bool     rc        = false;
    char    *p_sa      = p_stra;
    char    *p_sb      = p_strb;
    char     arra[256] = {0};
    char     arrb[256] = {0};
    int32_t  cnt       = 0;

    // sanity
    // ->check for empty string

    for (;;) {
        //CDISPLAY("%3u %c", cnt, *p_sa);
        //CDISPLAY("%3u %c", cnt, *p_sb);
        //getchar();

        // can optimze further
        if ((*p_sa == '\0') || (*p_sb == '\0')) {
            if ((*p_sa == '\0') && (*p_sb == '\0')) {
                /* same length, reached end, move to comparison */
                break;
            }
            /* different length, thus not permutation */
            goto exception;
        }

        arra[*p_sa]++;
        arrb[*p_sb]++;
        p_sa++;
        p_sb++;
        cnt++;
    }

    //compare
    for (int32_t i = 0; i < 256; i++) {
        if (arra[i] != arrb[i]) {
            /* Not a permutation, the letter instances are not equivalent */
            goto exception;
        }
    }

    /* permutation! */
    rc = true;
exception:
    //adts_hexdump(arra, sizeof(arra), "arra[]");
    //adts_hexdump(arrb, sizeof(arrb), "arrb[]");
    return rc;
} /* cci_c1_3a */


/*
 ****************************************************************************
 * Write a method that replaces all spaces in a string with %20.  Assume the
 * string has sufficient space a tthe end of the string to hold the additional
 * characters, and that you are given the "true" length of the string.
 *
 * Tests...
 *  - abcd
 *  - a b c d
 *  - "\0"
 *  - ""
 *  - " "
 *
 ****************************************************************************
 */
// O(n^2) time complexity -> nested for loop.  Approximates 0(N^2)
// O(1) space complexity  -> No additional space used.
static char *
cci_c1_4( char   *p_str,
          size_t  mbytes )
{
    //adts_hexdump(p_str, mbytes, "input");

    char     *pss       = p_str;
    char     *pse       = NULL;
    char     *pme       = &pss[mbytes - 1];
    uint32_t  sbytes    = 0;
    uint32_t  dbytes    = 0;

    //find
    sbytes = strlen(p_str);
    pse    = &p_str[sbytes - 1];
    dbytes = mbytes - sbytes;

    //CDISPLAY("%u - %u = %u", mbytes, sbytes, dbytes);
    //CDISPLAY("%llx - %llx = %u", pme, pse, (pme - pse));
    //CDISPLAY("%llx", &p_str[mbytes - 1]);

    for (int32_t i = sbytes; i >= 0; i--) {
        if (*pse == ' ') {
            //insert
            *pme = '0';
            pme--;
            *pme = '2';
            pme--;
            *pme = '%';
            pme--;
            dbytes -= 3;
            pse--;
        }

        *pme = *pse;
        *pse = '-';

        pme--;
        pse--;
        dbytes--;
    }

    //adts_hexdump(p_str, mbytes, "output");
    //adts_hexdump(pme, 64, "output");
    //CDISPLAY("%s %u", pss + dbytes, dbytes);
    //CDISPLAY("%c %llx", *pse, pse);
    //CDISPLAY("%c %llx", *pss, pss);

    return pss;
} /* cci_c1_4() */


/*
 ****************************************************************************
 * Implement a method to perform basic string compression using hte counts of
 * repeated characters. For example aabcccccaaa would become a2b1c5a3. If the
 * compressed string dould not become smaller than the original string, your
 * medhod should return the orignal string.
 *
 * Tests...
 * - aaaaaa -> all same
 * - abcd -> returns orig string
 * - aabbccdd -> mixed
 * - aaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbb -> >9 o each letter using >= 2 slots
 * - \0
 * - 0
 *
 ****************************************************************************
 */
// O(n) time complexity     -> single pass calc
// O(n-1) space complexity  -> approximates n since 1 char diff suffices to alloc
static size_t
cci_c1_5_get_compressed_bytes( char         *p_str,
                               const size_t  limit )
{
    char   tmp    = p_str[0];
    size_t bytes  = 2;
    size_t count  = 1;

    CDISPLAY("Limit = %u", limit);
    for (int32_t i = 1; i < limit; i++) {
        CDISPLAY("[%u] %c", i, p_str[i]);
        if (p_str[i] == p_str[i-1]) {
            count++;
            continue;
        }

        bytes += 2;
        while (count /= 10) {
            bytes++;
        }
        count  = 0;
    }

    CDISPLAY("Bytes = %u", bytes);
    return bytes;
} /* cci_c1_5_get_compressed_bytes() */

#if 0
static char *
cci_c1_5( char *p_str )
{
    bool    compressed = false;
    size_t  bytes      = strlen(p_str);
    char   *p_out      = NULL;
    char   *p_tmpstr   = p_str;
    char   *p_tmpout   = NULL;

    if (bytes <= 1) {
        // handle empty and single char strings
        goto exception;
    }

    // account for '\0' termination space
    bytes += 1;

    //int tmp = cci_c1_5_get_compressed_bytes(p_str, bytes);

#if 0
    p_out = calloc(bytes, sizeof(char));
    if (NULL == p_out) {
        // system failure
        goto exception;
    }
    p_tmpout = p_out;

    //CDISPLAY("%u", bytes);
    //CDISPLAY("%llx", p_tmpout);
    //CDISPLAY("%llx", &(p_tmpout[bytes - 1]));
    //CDISPLAY("%u", &(p_tmpout[bytes - 1]) - p_tmpout);

    *p_tmpout = *p_tmpstr;
    p_tmpout++;
    *p_tmpout = '1';
#if 0
    for (int32_t i = 1; i < (bytes - 2); i++) {
        CDISPLAY("[%u] [%llx = %c] [%llx = %c]",
            i, p_tmpout, *p_tmpout, &(p_tmpstr[i]), p_tmpstr[i])

        if (p_tmpstr[i] == p_tmpstr[i - 1]) {
            compressed = true;
            *p_tmpout += 1;
            //CDISPLAY("%c %c", p_tmpstr[i], p_tmpstr[i - 1]);
        }else {
            p_tmpout++;
            *p_tmpout = p_tmpstr[i];
            p_tmpout++;
        }
    }
    // dont forget to null terminate o_out!!!
#endif
#endif
exception:
    // common cleanup code
    if (compressed == false) {
        if (p_out) {
            free(p_out);
        }
        p_out = p_str;
    }

    return p_out;
} /* cci_c1_5() */
#endif


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
utest_control_1( void )
{
    CDISPLAY("cci 1.1a % 1.1.b______________");

    /* 2d array [ROW][COL] */
    #define COL (32)
    #define ROW (8)

    bool rc                = false;
    char string[ROW][COL]  = { "camilla_cardona",
                               "omar cardona",
                               "",
                               "abc123",
                               "0",
                               "aaa",
                               "!@#$",
                               "Finish"
                             };

    for (int32_t i = 0; i < ROW; i++) {
        char     *p_str = &string[0,i];
        uint32_t  bytes = strlen(p_str);

        CDISPLAY("%s", p_str);

        rc = cci_c1_1a(p_str, bytes);
        CDISPLAY("%s [%u]: %2u -> %s", (rc ? "TRUE " : "FALSE"), i, bytes, p_str);

        rc = cci_c1_1b(p_str, bytes);
        CDISPLAY("%s [%u]: %2u -> %s", (rc ? "TRUE " : "FALSE"), i, bytes, p_str);
    }

    return;
} /* utest_control_1() */


static void
utest_control_2( void )
{
    CDISPLAY("cci 1.2______________");

    /* 2d array [ROW][COL] */
    #define COL (32)
    #define ROW (8)

    bool rc                = false;
    char string[ROW][COL]  = { "camilla_cardona",
                               "omar cardona",
                               "",
                               "abc123",
                               "0",
                               "aaa",
                               "!@#$",
                               "Finish"
                             };

    for (int32_t i = 0; i < ROW; i++) {
        char *p_str = &string[0,i];
        CDISPLAY("%s", p_str);

        p_str = cci_c1_2(p_str);
        CDISPLAY("%s", p_str);
    }

    return;
} /* utest_control_2() */


static void
utest_control_3( void )
{
    CDISPLAY("cci 1.3______________");

    #define N (8)

    bool rc = false;
    char sta[N][64] = { "\0",
                        "abcd",
                        "aaabb",
                        "abc",
                        "",
                        "",
                        "abc",
                        "last"
                      };
    char stb[N][64] = { "\0",
                        "cbad",
                        "aabbb",
                        "abcd",
                        "abc",
                        "",
                        "\0",
                        "last"
                       };

    for (int32_t i = 0; i < N; i++) {
        char *p_sa = &(sta[i][0]);
        char *p_sb = &(stb[i][0]);

        CDISPLAY("%s", p_sa);
        CDISPLAY("%s", p_sb);

        rc = cci_c1_3a(p_sa, p_sb);
        CDISPLAY("%s", ((rc) ? "TRUE" : "FALSE"));
        //printf("Press any key to continue:");
        //getchar();
    }

    return;
} /* utest_control_3() */


static void
utest_control_5( void )
{
    CDISPLAY("cci 1.5______________");

    char    str[]   = { "aaaabbbccdAAAABBBCCD" };
    char   *p_out   = NULL;
    size_t  bytes   = strlen(str);
    size_t  cbytes  = 0;

    CDISPLAY("%s", str);
    cbytes = cci_c1_5_get_compressed_bytes(str, bytes);
    CDISPLAY("%u", cbytes);

    return;
} /* utest_control_5() */


static void
utest_control_6( void )
{
    CDISPLAY("cci 1.6______________");

    #define _COL (5)
    #define _ROW (5)

    char stx[_COL][_ROW] = { "AAAAA",
        "BBBBB",
        "CCCCC",
        "DDDDD",
        "EEEEE"
    };
#if 0
    #define ROW (3)
    #define COL (32)

    char sta[][COL] = { "Omar Cardona",
                           "Lizandra Chaparro",
                           "Camilla Cardona"
                         };

    adts_matrix(_COL, _ROW, sta);

    for (int32_t i = 0; i < ROW; i++) {
        char *p_str = &(sta[0][i]);
        CDISPLAY("[0][%u]: %s", i, p_str);
    }

    for (int32_t i = 0; i < ROW; i++) {
        char *p_str = &(sta[i][0]);
        CDISPLAY("[i][0]: %s", i, p_str);
    }
#endif
    return;
} /* utest_control_6() */


static void
utest_control( void )
{


    CDISPLAY("=========================================================");
    utest_control_1();
    CDISPLAY("=========================================================");
    utest_control_2();
    CDISPLAY("=========================================================");
    utest_control_3();
    // where is 4?
    CDISPLAY("=========================================================");
    utest_control_5();
    CDISPLAY("=========================================================");
    utest_control_6();





    return;
} /* utest_control() */

/*
 ****************************************************************************
 * test entrypoint
 *
 ****************************************************************************
 */
void
utest_cci_c1( void )
{
    CDISPLAY("=========================================================");

    utest_control();

    return;
} /* utest_cci_c1() */
