
#include <adts.h>
#include <utest.h>

#include <pe.h>
#include <cci.h>
#include <epi.h>
#include <misc.h>
#include <wtest.h>
#include <coursera.h>


/*
 ****************************************************************************
 * test control
 *
 ****************************************************************************
 */
static void
utest_control( void )
{
    //utest_epi();
    //utest_pe();
    //utest_misc();
    //utest_coursera();
    //utest_wtest();

    //utest_adts_rbt();
    //utest_adts_eyec();
    //utest_adts_bits();
    //utest_adts_time();
    //utest_adts_list();
    //utest_adts_heap();
    //utest_adts_math();
    //utest_adts_hash();
    //utest_adts_sort();
    //utest_adts_tree();
    //utest_adts_trie();
    //utest_adts_time();
	//utest_adts_meas();
    //utest_adts_cycles();
    //utest_adts_stack();
    //utest_adts_queue();
    //utest_adts_graph();
    //utest_adts_matrix();
    //utest_adts_hexdump();
    //utest_adts_snapshot();

    //utest_wtest();

    //utest_cci();
    utest_cci_c1();
    utest_cci_c2();

    return;
} /* utest_control() */


/*
 ****************************************************************************
 * test control
 *
 ****************************************************************************
 */
int32_t
main( void )
{
    int32_t rc = 0;

    CDISPLAY("=========================================================");
    utest_control();

    return rc;
} /* main() */

