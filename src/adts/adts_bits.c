
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include <inttypes.h>

/* Toolbox */
#include <adts_display.h>

/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
#define BIT_WIDTH_32 (32)
#define BIT_WIDTH_64 (64)


/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
void
adts_bit_display_32( int32_t val )
{
    char v                       = 0;
    char arr[ BIT_WIDTH_32 + 1 ] = {0}; /* Account for nul termination char */

    /* enforce nul termination */
    arr[BIT_WIDTH_32] = '\0';

    /* walk from msb to lsb */
    for (int32_t i = 0; i < BIT_WIDTH_32; i++) {
        /* Get the ascii value to set */
        v = (val & (1 << i)) ? '1' : '0';

        /* Insert in array in reverse order to bit walking */
        arr[BIT_WIDTH_32 - (1 + i)] = v;
    }

    CDISPLAY("%16i 0b%s", val, arr);

    return;
} /* adts_bit_display_32() */


/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
void
adts_bit_display_64( int64_t val )
{
    char v                       = 0;
    char arr[ BIT_WIDTH_64 + 1 ] = {0}; /* Account for nul termination char */

    /* enforce nul termination */
    arr[BIT_WIDTH_64] = '\0';

    /* walk from msb to lsb */
    for (int32_t i = 0; i < BIT_WIDTH_64; i++) {
        /* Get the ascii value to set */
        /* Note: use unsigned long long definition for 64bit correctness */
        v = (val & (1ull << i)) ? '1' : '0';

        /* Insert in array in reverse order to bit walking */
        arr[BIT_WIDTH_64 - (1 + i)] = v;
    }

    CDISPLAY("%16lli 0b%s", val, arr);

    return;
} /* adts_bit_display_64() */



/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
bool
adts_bit_is_set( int32_t value,
                 int32_t bit )
{
    return !!(value & (1 << bit));
} /* adts_bit_is_set() */


/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
bool
adts_bit_is_not_set( int32_t value,
                     int32_t bit )
{

    return !(adts_bit_is_set(value, bit));
} /* adts_bit_is_not_set() */


/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
int32_t
adts_bit_clear( int32_t value,
                int32_t bit )
{
    return value & ~(1 << bit);
} /* adts_bit_clear() */


/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
int32_t
adts_bit_clear_msb_to_kth( int32_t value,
                           int32_t bit )
{
    int32_t mask = (1 << bit) - 1;

    return value & mask;
} /* adts_bit_clear_msb_to_kth() */


/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
int32_t
adts_bit_clear_lsb_to_kth( int32_t value,
                           int32_t bit )
{
    int32_t mask = -1 << bit;

    return value & mask;
} /* adts_bit_clear_lsb_to_kth() */


/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
int32_t
adts_bit_toggle( int32_t value,
                 int32_t bit )
{
    int32_t mask = (1 << bit);

    return value ^ mask;
} /* adts_bit_toggle() */


/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
int32_t
adts_bit_clear_lsb( int32_t val )
{
    return (val & (val - 1));
} /* adts_bit_clear_lsb() */


/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
int32_t
adts_bit_get_lsb( int32_t val )
{
    return (val & (-val));
} /* adts_bit_get_lsb() */


/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
int32_t
adts_bit_count( int32_t val )
{
    int32_t bits = 0;

    while (val) {
        bits++;
        val = val & (val - 1);
    }

    return bits;
} /* adts_bit_count() */


/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
//FIXME!!!
int32_t
adts_bit_next_largest( uint32_t val )
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
} /* adts_bit_next_largest() */


/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
int32_t
adts_bit_reverse( int32_t val )
{
    int32_t out = 0;

    if (0 == val || (-1 == val)) {
        /* Well known symmetric */
        out = val;
        goto exception;
    }

    for (int32_t i = 0; i < BIT_WIDTH_32; i++) {
        /* verbose calculation for readability / maintainability */
        int32_t bitpos = (BIT_WIDTH_32 - 1) - i;
        int32_t bitval = !!(val & (1 << i));

        out |= bitval << bitpos;
    }

exception:
    /* optional output validation */
    adts_bit_display_32(val);
    adts_bit_display_32(out);

    return out;
} /* adts_bit_reverse() */



/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
static void
utest_control( void )
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
    const size_t   elems = sizeof(arr) / sizeof(int32_t);



    CDISPLAY("=========================================================");
    {
        int32_t rc     = 0;
        int32_t bit    = 0;

        for (size_t cnt = 0; cnt < elems; cnt++) {
            int32_t val = arr[cnt];
            rc = adts_bit_is_set(val, bit);
            CDISPLAY("%3i bit %2i  %s", val, bit, (rc) ? "true" : "false");
        }
    }

    CDISPLAY("=========================================================");
    {
        int32_t rc     = 0;
        int32_t bit    = 0;

        for (size_t cnt = 0; cnt < elems; cnt++) {
            int32_t val = arr[cnt];
            rc = adts_bit_is_not_set(val, bit);
            CDISPLAY("%3i bit %2i  %s", val, bit, (rc) ? "true" : "false");
        }
    }

    CDISPLAY("=========================================================");
    {
        int32_t bit    = 0;

        for (size_t cnt = 0; cnt < elems; cnt++) {
            int32_t val = arr[cnt];
            int32_t out = adts_bit_clear(val, bit);
            CDISPLAY("%3i bit %2i  %3i", val, bit, out);
        }
    }

    CDISPLAY("=========================================================");
    {
        int32_t bit    = 4;

        for (size_t cnt = 0; cnt < elems; cnt++) {
            int32_t val = arr[cnt];
            int32_t out = adts_bit_clear_lsb_to_kth(val, bit);
            CDISPLAY("%3i bit %2i  %3i", val, bit, out);
            adts_bit_display_32(val);
            adts_bit_display_32(out);
        }
    }

    CDISPLAY("=========================================================");
    {
        int32_t bit    = 2;

        for (size_t cnt = 0; cnt < elems; cnt++) {
            int32_t val = arr[cnt];
            int32_t out = adts_bit_clear_msb_to_kth(val, bit);
            CDISPLAY("%3i bit %2i  %3i", val, bit, out);
            adts_bit_display_32(val);
            adts_bit_display_32(out);
        }
    }

    CDISPLAY("=========================================================");
    {
        int32_t bit    = 0;

        for (size_t cnt = 0; cnt < elems; cnt++) {
            int32_t val = arr[cnt];
            int32_t out = adts_bit_toggle(val, bit);
            CDISPLAY("%3i bit %2i  %3i", val, bit, out);
        }
    }

    CDISPLAY("=========================================================");
    {
        for (size_t cnt = 0; cnt < elems; cnt++) {
            int32_t val = arr[cnt];
            int32_t out = adts_bit_clear_lsb(val);
            CDISPLAY("%3i ->  %3i", val, out);
        }
    }


    CDISPLAY("=========================================================");
    {
        for (size_t cnt = 0; cnt < elems; cnt++) {
            int32_t val = arr[cnt];
            int32_t out = adts_bit_get_lsb(val);
            CDISPLAY("%3i ->  %3i", val, out);
        }
    }

    CDISPLAY("=========================================================");
    {
        for (size_t cnt = 0; cnt < elems; cnt++) {
            int32_t val = arr[cnt];
            int32_t out = adts_bit_count(val);
            CDISPLAY("%3i ->  %3i", val, out);
        }
    }

    CDISPLAY("=========================================================");
    {
        for (size_t cnt = 0; cnt < elems; cnt++) {
            int32_t  val = arr[cnt];
            CDISPLAY("FIXME!!!");
            //uint32_t out = adts_bit_next_largest(val);
            //CDISPLAY("%3i ->  %3i", val, out);
        }
    }

    CDISPLAY("=========================================================");
    {
        for (size_t cnt = 0; cnt < elems; cnt++) {
            int32_t  val = arr[cnt];
            //CDISPLAY("%3i ->  %3i", val, 0);
            adts_bit_display_32(val);
        }
    }

    CDISPLAY("=========================================================");
    {
        for (size_t cnt = 0; cnt < elems; cnt++) {
            int32_t  val = arr[cnt];
            //CDISPLAY("%3i ->  %3i", val, 0);
            adts_bit_display_64(val);
        }
    }

    CDISPLAY("=========================================================");
    {
        for (size_t cnt = 0; cnt < elems; cnt++) {
            int32_t val = arr[cnt];
            int32_t out = adts_bit_reverse(val);
            //CDISPLAY("%3i ->  %3i", val, out);
        }
    }


    CDISPLAY("=========================================================");
    {
	/* Clear bits test */

	#define IP    (1 << 5)
	#define UDPV4 (1 << 6)
	#define UDPV6 (1 << 7)
	uint32_t val = 0;

	val = (IP|UDPV4|UDPV6);
	CDISPLAY("0x%8i", val);
	adts_bit_display_32(val);

	val &= ~(UDPV4|UDPV6);
	CDISPLAY("0x%8i", val);
	adts_bit_display_32(val);
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
utest_adts_bits( void )
{

    utest_control();

    return;
} /* utest_adts_bits() */
