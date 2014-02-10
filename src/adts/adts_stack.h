
#ifndef _H_ADTS_STACK
#define _H_ADTS_STACK

#include <string.h>
#include <stdbool.h>
#include <inttypes.h>

/**
 **************************************************************************
 * \details
 *
 **************************************************************************
 */
#define ADTS_STACK_BYTES (64)


/**
 **************************************************************************
 * \details
 *
 **************************************************************************
 */
typedef struct {
    const char reserved[ ADTS_STACK_BYTES ];
} adts_stack_t;




/**
 **************************************************************************
 * \details
 *
 **************************************************************************
 */
bool
adts_stack_is_empty( adts_stack_t *p_adts_stack );

bool
adts_stack_is_not_empty( adts_stack_t *p_adts_stack );

size_t
adts_stack_entries( adts_stack_t *p_adts_stack );

void *
adts_stack_peek( adts_stack_t *p_adts_stack );

void *
adts_stack_pop( adts_stack_t *p_adts_stack );

int32_t
adts_stack_push( adts_stack_t *p_adts_stack,
                 void         *p_data,
                 size_t        bytes );

void
adts_stack_destroy( adts_stack_t *p_adts_stack );

adts_stack_t *
adts_stack_create( void );

void
utest_adts_stack( void );


#endif /* _H_ADTS_STACK */

