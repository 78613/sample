#include <adts_matrix.h>
#include <adts_snapshot.h>
#include <adts_display.h>


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
 *  Passing a 2D array as argument requires that the col and row be parameters
 *  be visible _BEFORE_ the 2d array parameter.
 *
 *  While [col] may be omitted per C99 variable length flexibility, we make it explicit here for clarity.
 *  The [row] is explicit _AND_ required for valid input.
 *
 **************************************************************************
 */
void
adts_matrix_private_8( size_t           col,
                       size_t           row,
                       char             mtx[col][row],
                       adts_snapshot_t *p_snap ) /* caller details */
{
    printf("\n");
    printf("vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv\n");
    adts_snapshot_display(p_snap);

    printf("\n");
    for (int c = 0; c < col; c++) {
        for (int r = 0; r < row; r++) {
            printf("%c", mtx[c][r]);
        }
        printf("\n");
    }
    printf("\n");
    printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");

    return;
} /* adts_matrix_private_8() */


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
        #define _COL (5)
        #define _ROW (5)
        char mtx[_COL][_ROW] = { "AAAAA",
                                 "BBBBB",
                                 "CCCCC",
                                 "DDDDD",
                                 "EEEEE"
                               };

        adts_matrix(_COL, _ROW, mtx);
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
utest_adts_matrix( void )
{

    utest_control();

    return;
} /* utest_adts_matrix() */
