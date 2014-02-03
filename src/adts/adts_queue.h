

#ifndef _H_ADTS_QUEUE
#define _H_ADTS_QUEUE

#include <string.h>
#include <stdbool.h>
#include <inttypes.h>


/**
 **************************************************************************
 * \details
 *
 **************************************************************************
 */
#define ADTS_QUEUE_BYTES (32)


/**
 **************************************************************************
 * \details
 *
 **************************************************************************
 */
typedef struct {
    char reserved[ ADTS_QUEUE_BYTES ];
} adts_queue_t;




/**
 **************************************************************************
 * \details
 *
 **************************************************************************
 */
bool
adts_queue_is_empty( adts_queue_t *p_adts_queue );

bool
adts_queue_is_not_empty( adts_queue_t *p_adts_queue );

size_t
adts_queue_entries( adts_queue_t *p_adts_queue );

void
adts_queue_display( adts_queue_t *p_adts_queue );

void *
adts_queue_dequeue( adts_queue_t *p_adts_queue );

int32_t
adts_queue_enqueue( adts_queue_t *p_adts_queue,
                    void         *p_data,
                    size_t        bytes );

void
adts_queue_destroy( adts_queue_t *p_adts_queue );

adts_queue_t *
adts_queue_create( void );

void
utest_adts_queue( void );



#endif /* _H_ADTS_QUEUE */


