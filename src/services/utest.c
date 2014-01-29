

#include <stdint.h>
#include <services.h>
#include <services_bits.h>


static void
utest_control( void )
{
    utest_services_bits();

    return;
} /* utest_control() */


int32_t
main( void )
{
    int32_t rc = 0;

    utest_control();

    return rc;
} /* main() */

