

#include <stdint.h>

#include <adts_bits.h>
#include <adts_hexdump.h>
#include <adts_snapshot.h>
#include <adts_services.h>


/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
static void
utest_control( void )
{
    utest_services_bits();
    utest_adts_hexdump();
    utest_adts_snapshot();

    return;
} /* utest_control() */



/*
 ****************************************************************************
 *
 *
 ****************************************************************************
 */
int32_t
main( void )
{
    int32_t rc = 0;

    utest_control();

    return rc;
} /* main() */

