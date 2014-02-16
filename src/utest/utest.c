
#include <stdint.h>

#include <adts_bits.h>
#include <adts_hash.h>
#include <adts_heap.h>
#include <adts_list.h>
#include <adts_math.h>
#include <adts_time.h>
#include <adts_tree.h>
#include <adts_sort.h>
#include <adts_stack.h>
#include <adts_queue.h>
#include <adts_hexdump.h>
#include <adts_snapshot.h>
#include <adts_services.h>

#include <pe.h>
#include <cci.h>
#include <epi.h>
#include <misc.h>


static void
utest_adts( void )
{
//    utest_adts_bits();
//    utest_adts_time();
    utest_adts_list();
//    utest_adts_heap();
//    utest_adts_math();
//    utest_adts_hash();
//    utest_adts_sort();
//    utest_adts_tree();
//    utest_adts_stack();
//    utest_adts_queue();
//    utest_adts_graph();
//    utest_adts_hexdump();
//    utest_adts_snapshot();

    return;
} /* utest_adts() */


static void
utest_control( void )
{
    utest_adts();
    //utest_cci();
    //utest_epi();
    //utest_pe();
    //utest_misc();

    return;
} /* utest_control() */


int32_t
main( void )
{
    int32_t rc = 0;

    CDISPLAY("=========================================================");

    utest_control();

    return rc;
} /* main() */

