
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include <inttypes.h>

#include <adts_hexdump.h>
#include <adts_snapshot.h>
#include <adts_services.h>



/******************************************************************************
 * ####### #     # #     #  #####  #######   ###   ####### #     #  #####
 * #       #     # ##    # #     #    #       #    #     # ##    # #     #
 * #       #     # # #   # #          #       #    #     # # #   # #
 * #####   #     # #  #  # #          #       #    #     # #  #  #  #####
 * #       #     # #   # # #          #       #    #     # #   # #       #
 * #       #     # #    ## #     #    #       #    #     # #    ## #     #
 * #        #####  #     #  #####     #      ###   ####### #     #  #####
******************************************************************************/


/**
 **************************************************************************
 * \details
 *
 *      THIS IS A !!!TEMPORARY!!! HEXDUMP STUB TO BE REPLACED SHORTLY
 *
 *      It is used to resolve the abstract data type API such that we can
 *      seamlessly replace the internal implementation in the near future
 *
 *      Source: http://www.alexonlinux.com/hex-dump-functions
 *
 *
 **************************************************************************
 */
static void
temporary_hexdump_stubfunc( char    *p_data,
                            int32_t  bytes )
{
    int32_t  j           = 0;    // index in line...
    char     temp[8]     = {0};
    char     buffer[128] = {0};
    char    *ascii       = NULL;

    memset(buffer, 0, 128);

    // Printing the ruler...
    printf("        +0          +4          +8          +c            0   4   8   C   \n");

    // Hex portion of the line is 8 (the padding) + 3 * 16 = 52 chars long
    // We add another four bytes padding and place the ASCII version...
    ascii = buffer + 58;
    memset(buffer, ' ', 58 + 16);
    buffer[58 + 16] = '\n';
    buffer[58 + 17] = '\0';
    buffer[0] = '+';
    buffer[1] = '0';
    buffer[2] = '0';
    buffer[3] = '0';
    buffer[4] = '0';
    for (int32_t i = 0, j = 0; i < bytes; i++, j++) {
        if (j == 16) {
            printf("%s", buffer);
            memset(buffer, ' ', 58 + 16);

            sprintf(temp, "+%04x", i);
            memcpy(buffer, temp, 5);

            j = 0;
        }

        sprintf(temp, "%02x", 0xff & p_data[i]);
        memcpy(buffer + 8 + (j * 3), temp, 2);
        if ((p_data[i] > 31) && (p_data[i] < 127)) {
            ascii[j] = p_data[i];
        }else {
            ascii[j] = '.';
        }
    }

    if (j != 0) {
        printf("%s", buffer);
    }
}


/**
 **************************************************************************
 * \details
 *
 *
 **************************************************************************
 */
void
adts_hexdump_private( char            *p_data,
                      size_t           bytes,
                      char            *p_msg,
                      adts_snapshot_t *p_snap ) /* caller details */
{

    printf("\n");
    printf("---------------------------------------------------------------\n");
    adts_snapshot_display(p_snap);

    printf("\n");
    if (p_msg) {
        printf("  Message: \"%s\"\n", p_msg);
    }
    printf("---------------------------------------------------------------\n");
    printf("\n");

    temporary_hexdump_stubfunc(p_data, bytes);

    return;
} /* adts_hexdump() */





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
 *
 *
 ****************************************************************************
 */
static void
utest_control( void )
{
    CDISPLAY("=========================================================");
    {
        char arr[125] = {0};

        adts_hexdump(arr, sizeof(arr), "foobar");

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
utest_adts_hexdump( void )
{

    utest_control();

    return;
} /* utest_adts_hexdump() */
