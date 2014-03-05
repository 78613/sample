#include <errno.h>
#include <assert.h>

#include <adts_stack.h>
#include <adts_hexdump.h>
#include <adts_services.h>


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
static void
utest_control( void )
{
    size_t key[] = {-1,0,1,2,3,4,5,6,7,8,9};
    size_t elems = sizeof(key) / sizeof(key[0]);
#if 0
    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: simple func test");

        char          foo[64] = {0};
        adts_stack_t *p_stack = NULL;

        p_stack = adts_stack_create();
        (void) adts_stack_push(p_stack, &(foo), sizeof(foo));
        (void) adts_stack_peek(p_stack);
        adts_stack_display(p_stack, NULL);
        (void) adts_stack_pop(p_stack);
        adts_stack_destroy(p_stack);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: stack push");
        int32_t       rc      = 0;
        adts_stack_t *p_stack = NULL;

        p_stack = adts_stack_create();
        assert(p_stack);

        for (size_t idx = 0; idx < elems; idx++) {
            rc = adts_stack_push(p_stack, key[idx], sizeof(key[idx]));
            assert(0 == rc);
            adts_stack_display(p_stack, NULL);
        }

        adts_stack_destroy(p_stack);
    }

    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: stack push -> peek validate");

        void         *val     = 0;
        int32_t       rc      = 0;
        adts_stack_t *p_stack = NULL;

        p_stack = adts_stack_create();
        assert(p_stack);

        for (size_t idx = 0; idx < elems; idx++) {
            rc = adts_stack_push(p_stack, key[idx], sizeof(key[idx]));
            assert(0 == rc);
        }

        adts_stack_display(p_stack, NULL);

        val = adts_stack_peek(p_stack);
        assert(key[elems - 1] == val);

        adts_stack_destroy(p_stack);
    }
#endif
    CDISPLAY("=========================================================");
    {
        CDISPLAY("Test: stack push -> pop");

        void         *val     = 0;
        int32_t       rc      = 0;
        adts_stack_t *p_stack = NULL;

        p_stack = adts_stack_create();
        assert(p_stack);

        for (size_t idx = 0; idx < elems; idx++) {
            rc = adts_stack_push(p_stack, key[idx], sizeof(key[idx]));
            assert(0 == rc);
        }

        adts_stack_display(p_stack, NULL);

        for (int32_t idx = (elems - 1); idx >= 0; idx--) {
            (void) adts_stack_pop(p_stack);
            adts_stack_display(p_stack, NULL);
        }

        adts_stack_destroy(p_stack);
    }

    return;
} /* utest_control() */


/*
 ****************************************************************************
 * test public entrypoint
 *
 ****************************************************************************
 */
void
utest_adts_stack_public( void )
{
    utest_control();

    return;
} /* utest_adts_stack_public() */


