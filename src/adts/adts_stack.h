#pragma once

#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <adts_snapshot.h>


/**
 **************************************************************************
 * \details
 *
 **************************************************************************
 */
#define ADTS_STACK_BYTES (128)


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
 *   stack display srevice
 *
 **************************************************************************
 */
#define adts_stack_display( _p_stack, _p_message ) \
    do {                                           \
        adts_snapshot_t  _snap   = {0};            \
        adts_snapshot_t *_p_snap = &(_snap);       \
                                                   \
        /* Get the call properties */              \
        adts_snapshot(_p_snap);                    \
                                                   \
        /* Perform the hexdump */                  \
        adts_stack_display_worker( _p_stack,       \
                                   _p_message,     \
                                   _p_snap );      \
    } while (0);


/**
 **************************************************************************
 * \details
 *
 **************************************************************************
 */
void
adts_stack_display_worker( adts_stack_t   *p_adts_stack,
                           char            *p_msg,
                           adts_snapshot_t *p_snap );
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



/**
 **************************************************************************
 * \details
 *   Unit Test prototypes
 *
 **************************************************************************
 */
void
utest_adts_stack( void );
void
utest_adts_stack_public( void );

