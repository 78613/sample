
#include <stdint.h>

#include <adts_bits.h>
#include <adts_time.h>
#include <adts_stack.h>
#include <adts_queue.h>
#include <adts_hexdump.h>
#include <adts_snapshot.h>
#include <adts_services.h>


/*
 ****************************************************************************
 * \brief
 *   Internal control
 *
 ****************************************************************************
 */
static void
utest_control( void )
{
//    utest_adts_bits();
//    utest_adts_time();
    utest_adts_stack();
    utest_adts_queue();
//    utest_adts_hexdump();
//    utest_adts_snapshot();

    return;
} /* utest_control() */



/*
 ****************************************************************************
 * \brief
 *   Public testsuite entrypoint
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

