


#include <errno.h>
#include <assert.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include <inttypes.h>

/* Toolbox */
#include <adts_math.h>
#include <adts_private.h>
#include <adts_services.h>


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
 *
 ****************************************************************************
 */
bool
adts_is_prime( const size_t prime )
{
    bool   rc  = false;
    size_t num = 0;

    for (num = 2; num <= prime; num++) {
        if (0 == (prime % num)) {
            /* divisible */
            break;
        }
    }

    if (num == prime) {
        rc = true;
    }

    return rc;
} /* adts_is_prime() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
bool
adts_is_not_prime( const size_t prime )
{
    return !(adts_is_prime(prime));
} /* adts_is_prime() */


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
size_t
adts_prime_ceiling( const size_t limit )
{
    size_t num   = 0;
    size_t val   = 0;
    size_t prime = 3;

    if (unlikely(1 >= limit)) {
        /* sanity */
        goto exception;
    }

    if (unlikely(2 >= limit)) {
        val = 2;
        goto exception;
    }

    /* Prime # is divisible only by 1 and self */
    while (prime <= limit) {
        if (adts_is_prime(prime)) {
            val = prime;
        }
        prime++;
    }

exception:
    return val;
} /* adts_prime_ceiling() */


/*
 ****************************************************************************
 * \details
 *
 ****************************************************************************
 */
size_t
adts_prime_floor( const size_t limit )
{
    size_t num   = 0;
    size_t val   = 0;
    size_t prime = 3;

    if (unlikely(2 >= limit)) {
        val = 2;
        goto exception;
    }

    /* Get the prime which is <= to the limit */
    prime = adts_prime_ceiling(limit);

    /* Increment the current prime to search for next */
    prime++;

    /* Prime # is divisible only by 1 and self */
    for (;;) {
        if (adts_is_prime(prime)) {
            val = prime;
            break;
        }
        prime++;
    }

exception:
    return val;
} /* adts_prime_floor() */



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
        const size_t arr[] = {0, 1, 2, 3, 4, 5, 30, 33, 50, 4096};
        const size_t elems = sizeof(arr) / sizeof(arr[0]);

        for (size_t idx = 0; idx < elems; idx++) {
            size_t val = 0;

            printf("\n");
            CDISPLAY("Input:  %d", arr[idx]);
            val = adts_prime_ceiling(arr[idx]);
            CDISPLAY("Output: %d", val);

            /* Sanity check */
            assert(arr[idx] >= val);
        }
    }

    CDISPLAY("=========================================================");
    {
        const size_t arr[] = {0, 1, 2, 3, 4, 5, 30, 33, 50, 4096};
        const size_t elems = sizeof(arr) / sizeof(arr[0]);

        for (size_t idx = 0; idx < elems; idx++) {
            size_t val = 0;

            printf("\n");
            CDISPLAY("Input:  %d", arr[idx]);
            val = adts_prime_floor(arr[idx]);
            CDISPLAY("Output: %d", val);

            /* Sanity check */
            assert(arr[idx] <= val);
        }
    }

    CDISPLAY("=========================================================");
    {
        const size_t arr[] = {0, 1, 2, 3, 4, 5, 30, 33, 50, 4096};
        const size_t elems = sizeof(arr) / sizeof(arr[0]);

        for (size_t idx = 0; idx < elems; idx++) {
            bool rc = 0;

            printf("\n");
            CDISPLAY("Input:  %d", arr[idx]);
            rc = adts_is_prime(arr[idx]);
            CDISPLAY("Output: %s", (rc) ? "true" : "false");
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
utest_adts_math( void )
{
    utest_control();

    return;
} /* utest_adts_math() */
