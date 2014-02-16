


#include <errno.h>
#include <assert.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include <inttypes.h>

/* Toolbox */
#include <adts_private.h>
#include <adts_services.h>



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
        CDISPLAY("Test:");
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
