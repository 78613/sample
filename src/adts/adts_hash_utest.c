
#include <errno.h>
#include <assert.h>

#include <adts_hash.h>
#include <adts_hexdump.h>
#include <adts_services.h>


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
 * \details
 *   this is clearly not the best hash function but it exists in the case
 ****************************************************************************
 */
static size_t
utest_hash_function( adts_hash_t *p_hash,
                     const void  *p_key )
{
    size_t idx = (int32_t) p_key % p_hash->pub.elems_limit;

    return idx;
} /* utest_hash_function() */


/*
 ****************************************************************************
 *  Generate a set of collisions based on the hashtbl limit properties
 ****************************************************************************
 */
static void
utest_hash_generate_collisions( adts_hash_t   *p_hash,
                                const int32_t  column, /* 0-n */
                                const size_t   elems,
                                size_t         out[] )
{
    CDISPLAY("Generate");
    size_t idx   = 0;
    size_t val   = 0;
    size_t next  = column;
    size_t limit = p_hash->pub.elems_limit;

    /* count for the requested elements */
    for (int32_t cnt = 0; cnt < elems; cnt++) {
        val      = next;  /* key value */
        out[cnt] = val;   /* insert into output array */
        next    += limit; /* next collision value */
    }

    return;
} /* utest_hash_generate_collisions() */


/*
 ****************************************************************************
 * test control
 *
 ****************************************************************************
 */
static void
utest_control( void )
{
    #define UTEST_ELEMS (32)

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: create destroy");
        adts_hash_t       *p_hash = NULL;
        adts_hash_create_t op     = {0};

        op.p_func = utest_hash_function;

        p_hash = adts_hash_create(&op);
        assert(p_hash);
        adts_hash_destroy(p_hash);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: display empty");
        adts_hash_t       *p_hash = NULL;
        adts_hash_create_t op     = {0};

        op.p_func = utest_hash_function;

        p_hash = adts_hash_create(&op);
        assert(p_hash);
        adts_hash_display(p_hash, NULL);
        adts_hash_destroy(p_hash);
    }


    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: remove empty");
        int32_t            rc     = 0;
        adts_hash_t       *p_hash = NULL;
        adts_hash_create_t op     = {0};

        op.p_func = utest_hash_function;

        p_hash = adts_hash_create(&op);
        assert(p_hash);
        rc = adts_hash_remove(p_hash, 5);
        assert(rc);
        adts_hash_destroy(p_hash);
    }


    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: insert one");
        int32_t                  rc     = 0;
        adts_hash_t             *p_hash = NULL;
        adts_hash_node_t         node   = {0};
        adts_hash_create_t       op     = {0};
        adts_hash_node_public_t  input  = {0};

        op.p_func = utest_hash_function;

        p_hash = adts_hash_create(&op);
        assert(p_hash);

        input.p_data = -1;
        input.bytes  = -1;
        input.p_key  = 5;
        rc = adts_hash_insert(p_hash, &(node), &(input));
        assert(0 == rc);

        adts_hash_display(p_hash, NULL);
        adts_hash_destroy(p_hash);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: duplicate keys");
        size_t                   key[]                = {5, 5, 5};
        int32_t                  rc                   = 0;
        adts_hash_t             *p_hash               = NULL;
        adts_hash_node_t         node[ UTEST_ELEMS ]  = {0};
        adts_hash_create_t       op                   = {0};
        adts_hash_node_public_t  input[ UTEST_ELEMS ] = {0};

        op.p_func = utest_hash_function;

        p_hash = adts_hash_create(&op);
        assert(p_hash);

        for (int32_t i = 0; i < 3; i++) {
            input[i].p_data = -1;
            input[i].bytes  = sizeof(node[i]);
            input[i].p_key  = key[i];

            rc = adts_hash_insert(p_hash, &(node[i]), input);
            if (0 < i) {
                /* Second entry should fail with duplicate detection */
                assert(rc);
            }
        }

        adts_hash_display(p_hash, NULL);
        adts_hash_destroy(p_hash);
    }


    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: generate collisions");
        size_t                   key[ UTEST_ELEMS ]   = {0};
        size_t                   elems                = 3;
        size_t                   col                  = 3;
        int32_t                  rc                   = 0;
        adts_hash_t             *p_hash               = NULL;
        adts_hash_node_t         node[ UTEST_ELEMS ]  = {0};
        adts_hash_create_t       op                   = {0};
        adts_hash_node_public_t  input[ UTEST_ELEMS ] = {0};

        op.p_func = utest_hash_function;

        p_hash = adts_hash_create(&op);
        assert(p_hash);

        utest_hash_generate_collisions(p_hash, col, elems, key);

        for (int32_t i = 0; i < elems; i++) {
            input[i].p_data = -1;
            input[i].bytes  = sizeof(node[i]);
            input[i].p_key  = key[i];
            CDISPLAY(" ");
            CDISPLAY("%p %p %p",
                    input[i].p_data, input[i].bytes, input[i].p_key);

            rc = adts_hash_insert(p_hash, &(node[i]), &(input[i]));
            assert(0 == rc);
        }
        adts_hash_display(p_hash, NULL);
        adts_hash_destroy(p_hash);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: insert one -> remove one");
        int32_t                  rc     = 0;
        adts_hash_t             *p_hash = NULL;
        adts_hash_node_t         node   = {0};
        adts_hash_create_t       op     = {0};
        adts_hash_node_public_t  input  = {0};

        op.p_func = utest_hash_function;

        p_hash = adts_hash_create(&op);
        assert(p_hash);

        input.p_data = -1;
        input.bytes  = -1;
        input.p_key  = 5;
        rc = adts_hash_insert(p_hash, &(node), &(input));
        assert(0 == rc);

        adts_hash_display(p_hash, NULL);
        rc = adts_hash_remove(p_hash, 5);
        assert(0 == rc);

        printf("\n");
        adts_hash_display(p_hash, NULL);
        adts_hash_destroy(p_hash);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: generate collisions -> remove from collisions T -> H");
        size_t                   key[ UTEST_ELEMS ]   = {0};
        size_t                   elems                = 3;
        size_t                   col                  = 3;
        int32_t                  rc                   = 0;
        adts_hash_t             *p_hash               = NULL;
        adts_hash_node_t         node[ UTEST_ELEMS ]  = {0};
        adts_hash_create_t       op                   = {0};
        adts_hash_node_public_t  input[ UTEST_ELEMS ] = {0};

        op.p_func = utest_hash_function;

        p_hash = adts_hash_create(&op);
        assert(p_hash);

        utest_hash_generate_collisions(p_hash, col, elems, key);


        for (int32_t i = 0; i < elems; i++) {
            input[i].p_data = -1;
            input[i].bytes  = sizeof(node[i]);
            input[i].p_key  = key[i];

            rc = adts_hash_insert(p_hash, &(node[i]), &(input[i]));
            assert(0 == rc);
        }
        adts_hash_display(p_hash, NULL);

        for (int32_t i = 0; i < elems; i++) {
            rc = adts_hash_remove(p_hash, key[i]);
            assert(0 == rc);
            adts_hash_display(p_hash, NULL);
        }

        adts_hash_display(p_hash, NULL);
        adts_hash_destroy(p_hash);
    }


    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: generate collisions -> remove from collisions H -> T");
        size_t                   key[ UTEST_ELEMS ]   = {0};
        size_t                   elems                = 3;
        size_t                   col                  = 3;
        int32_t                  rc                   = 0;
        adts_hash_t             *p_hash               = NULL;
        adts_hash_node_t         node[ UTEST_ELEMS ]  = {0};
        adts_hash_create_t       op                   = {0};
        adts_hash_node_public_t  input[ UTEST_ELEMS ] = {0};

        op.p_func = utest_hash_function;

        p_hash = adts_hash_create(&op);
        assert(p_hash);

        utest_hash_generate_collisions(p_hash, col, elems, key);

        for (int32_t i = 0; i < elems; i++) {
            input[i].p_data = -1;
            input[i].bytes  = sizeof(node[i]);
            input[i].p_key  = key[i];

            rc = adts_hash_insert(p_hash, &(node[i]), &(input[i]));
            assert(0 == rc);
        }
        adts_hash_display(p_hash, NULL);

        for (int32_t i = 2; i >= 0; i--) {
            rc = adts_hash_remove(p_hash, key[i]);
            assert(0 == rc);
            adts_hash_display(p_hash, NULL);
        }

        adts_hash_display(p_hash, NULL);
        adts_hash_destroy(p_hash);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: generate collisions -> remove from collisions MID");
        size_t                   key[ UTEST_ELEMS ]   = {0};
        size_t                   elems                = 3;
        size_t                   col                  = 3;
        int32_t                  rc                   = 0;
        adts_hash_t             *p_hash               = NULL;
        adts_hash_node_t         node[ UTEST_ELEMS ]  = {0};
        adts_hash_create_t       op                   = {0};
        adts_hash_node_public_t  input[ UTEST_ELEMS ] = {0};

        op.p_func = utest_hash_function;

        p_hash = adts_hash_create(&op);
        assert(p_hash);

        utest_hash_generate_collisions(p_hash, col, elems, key);

        for (int32_t i = 0; i < elems; i++) {
            input[i].p_data = -1;
            input[i].bytes  = sizeof(node[i]);
            input[i].p_key  = key[i];

            rc = adts_hash_insert(p_hash, &(node[i]), &(input[i]));
            assert(0 == rc);
        }
        adts_hash_display(p_hash, NULL);

        rc = adts_hash_remove(p_hash, key[1]);
        assert(0 == rc);

        adts_hash_display(p_hash, NULL);
        adts_hash_destroy(p_hash);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: find");
        size_t                   key[ UTEST_ELEMS ]   = {0};
        size_t                   elems                = 3;
        size_t                   col                  = 3;
        int32_t                  rc                   = 0;
        adts_hash_t             *p_hash               = NULL;
        adts_hash_node_t         node[ UTEST_ELEMS ]  = {0};
        adts_hash_node_t        *p_out                = NULL;
        adts_hash_create_t       op                   = {0};
        adts_hash_node_public_t  input[ UTEST_ELEMS ] = {0};

        op.p_func = utest_hash_function;

        p_hash = adts_hash_create(&op);
        assert(p_hash);

        utest_hash_generate_collisions(p_hash, col, elems, key);

        for (int32_t i = 0; i < elems; i++) {
            input[i].p_data = -1;
            input[i].bytes  = sizeof(node[i]);
            input[i].p_key  = key[i];

            rc = adts_hash_insert(p_hash, &(node[i]), &(input[i]));
            assert(0 == rc);
        }
        adts_hash_display(p_hash, NULL);

        p_out = adts_hash_find(p_hash, key[1]);
        assert(p_out);
        assert(p_out->pub.p_key == key[1]);

        rc = adts_hash_remove(p_hash, key[1]);
        assert(0 == rc);

        adts_hash_display(p_hash, NULL);
        adts_hash_destroy(p_hash);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: trigger resize grow");
        size_t                   key[]  = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
        size_t                   elems  = sizeof(key) / sizeof(key[0]);
        int32_t                  rc     = 0;
        adts_hash_t             *p_hash = NULL;
        adts_hash_node_t         node[ UTEST_ELEMS ]  = {0};
        adts_hash_create_t       op                   = {0};
        adts_hash_node_public_t  input[ UTEST_ELEMS ] = {0};

        op.p_func = utest_hash_function;

        p_hash = adts_hash_create(&op);
        assert(p_hash);

        //adts_hash_display(p_hash, "Start");
        for (int32_t i = 0; i < elems; i++) {
            input[i].p_data = -1;
            input[i].bytes  = sizeof(node[i]);
            input[i].p_key  = key[i];

            rc = adts_hash_insert(p_hash, &(node[i]), &(input[i]));
            assert(0 == rc);
        }

        adts_hash_display(p_hash, "End");
        adts_hash_destroy(p_hash);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: trigger resize grow -> shrink");
        size_t                   key[]  = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
        size_t                   elems  = sizeof(key) / sizeof(key[0]);
        int32_t                  rc     = 0;
        adts_hash_t             *p_hash = NULL;
        adts_hash_node_t         node[ UTEST_ELEMS ]  = {0};
        adts_hash_create_t       op                   = {0};
        adts_hash_node_public_t  input[ UTEST_ELEMS ] = {0};

        op.p_func = utest_hash_function;

        p_hash = adts_hash_create(&op);
        assert(p_hash);

        for (int32_t i = 0; i < elems; i++) {
            input[i].p_data = -1;
            input[i].bytes  = sizeof(node[i]);
            input[i].p_key  = key[i];

            rc = adts_hash_insert(p_hash, &(node[i]), &(input[i]));
            assert(0 == rc);
            CDISPLAY("load: %f  %2i / %2i",
                    p_hash->pub.stats.loadfactor,
                    p_hash->pub.elems_curr,
                    p_hash->pub.elems_limit);
        }
        adts_hash_display(p_hash, NULL);

        for (int32_t i = 0; i < elems; i++) {
            rc = adts_hash_remove(p_hash, key[i]);
            assert(0 == rc);
            //adts_hash_display(p_hash, NULL);
            CDISPLAY("load: %f  %2i / %2i",
                    p_hash->pub.stats.loadfactor,
                    p_hash->pub.elems_curr,
                    p_hash->pub.elems_limit);
        }

        adts_hash_display(p_hash, NULL);
        adts_hash_destroy(p_hash);
    }

    //test grow -> find
    //test shrink -> find


    return;
} /* utest_control() */


/*
 ****************************************************************************
 * test public entrypoint
 *
 ****************************************************************************
 */
void
utest_adts_hash_public( void )
{
    utest_control();

    return;
} /* utest_adts_hash_public() */



