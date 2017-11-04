
#include <adts.h>
#include <utest.h>

#define TRUE      true
#define FALSE     false
#define VOID      void
#define ULONG     uint64_t
#define BOOLEAN   bool
#define NT_ASSERT assert






static void
test_array_size( void )
{
    #define ARR_ENTRIES (128)
    uint32_t arr[ ARR_ENTRIES ] = {0};

    CDISPLAY("%d", ARR_ENTRIES);
    CDISPLAY("%d", sizeof(uint32_t));
    CDISPLAY("%d", sizeof(uint32_t) * ARR_ENTRIES);
    CDISPLAY("%d", sizeof(*arr));
    CDISPLAY("%d", sizeof(arr));
    CDISPLAY("%d", sizeof(void *));

    return;
} /* test_array_size() */


static bool
test_bounds_valid( char *SrcStart,
                   ULONG SrcLen,
                   char *DestStart,
                   ULONG DestLen )
{

    ULONG   srcEnd  = SrcStart + SrcLen;
    ULONG   destEnd = DestStart + DestLen;
    BOOLEAN rc      = false;

    //
    // src   |------------------|
    // dest  |-----------| xxxxxx
    //
    if (SrcLen > DestLen) {
        goto Cleanup;
    }

    //
    // src   |-----------|
    // dest  x  |-----------|
    //
    if (SrcStart < DestStart) {
        goto Cleanup;
    }

    //
    // src         |---------|
    // dest  |-----------|   x
    //
    if (srcEnd > destEnd) {
        goto Cleanup;
    }

    rc = true;

Cleanup:
    return rc;
} /* test_bounds_valid() */


// returns true of the source falls outside of the destination
static BOOLEAN
test_bounds_invalid( char *SrcStart,
                     ULONG SrcLen,
                     char *DestStart,
                     ULONG DestLen )
{
    ULONG   srcEnd  = SrcStart + SrcLen;
    ULONG   destEnd = DestStart + DestLen;

    return ((SrcLen   > DestLen) ||   /* too long - wrap protection */
            (SrcStart < DestStart) || /* buffer underrun */
            (srcEnd   > destEnd));    /* buffer overrun */
} /* test_bounds_invalid() */


// works for static and dynamically allocated arrays
static void
test_print_array( uint64_t       *arr,
                  const int32_t   rows,
                  const int32_t   cols )
{
    for (int32_t x = 0; x < rows; x++) {
        for (int32_t y = 0; y < cols; y++) {
            printf("%6i ", arr[x * rows + y]);
        }
        printf("\n");
    }

    return;
} /* test_print_array() */


// Simple incrementer tests
static void
test_incrementer( void )
{
    CDISPLAY("i++");
    for (int32_t i = 0; i < 10; i++) {
        CDISPLAY("i = %u", i);
    }

    CDISPLAY("++i");
    for (int32_t i = 0; i < 10; ++i) {
        CDISPLAY("i = %u", i);
    }

    CDISPLAY("i+=1");
    for (int32_t i = 0; i < 10; i+=1) {
        CDISPLAY("i = %u", i);
    }

    {
        int32_t i = 0;
        int32_t x = 0;
        x = i++;
        CDISPLAY("%u = i++;", x);
    }

    {
        int32_t i = 0;
        int32_t x = 0;
        x = ++i;
        CDISPLAY("%u = ++i;", x);
    }

    {
        int32_t i = 0;
        int32_t x = 0;
        i += 1;
        x = i;
        CDISPLAY("%u = +=i;", x);
    }


    return;
} /* test_incrementer() */


// Only works for static arrays

static void
utest_control( void )
{
    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: 1");
    }

    CDISPLAY("=========================================================");
    {
        test_array_size();
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: Valid Bounds");
        #define X (8)
        #define Y (8)
        uint64_t arr[Y][X] = {
                          {10, 20},
                          {10, 100},
                          {110, 50},
                          {190, 100},
                          {210, 100},
                          {110, 200}
                          };

        /* print the array */
        test_print_array(&(arr), X, Y);


        #if 0
        // Hand print individual entries for translation as inputs
        for (int32_t x = 0; x < X; x++) {
            for (int32_t y = 0; y < Y; y++) {
                printf("%6i ", arr[x][y]);
            }
            printf("\n");
        }
        #endif

        for (int32_t i = 0; i < X; i++) {
            void     *srcStart  = arr[i][0];
            uint64_t  srcLen    = arr[i][1];
            void     *destStart = 100;
            uint64_t  destLen   = 100;
            bool      rc        = 0;

            CDISPLAY("%5d %5d %5d %5d", srcStart, srcLen, destStart, destLen);
            rc = test_bounds_invalid(srcStart, srcLen, destStart, destLen);
            CDISPLAY("%d", rc);
        }
    }

    CDISPLAY("=========================================================");
    {
        test_incrementer();
    }

    return;
} /* utest_control() */



void
utest_wtest( void )
{
    utest_control();

    return;
} /* utest_wtest() */

