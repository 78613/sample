

#ifndef _H_ADTS_HEAP
#define _H_ADTS_HEAP

#include <string.h>
#include <stdbool.h>
#include <inttypes.h>


/**
 **************************************************************************
 * \details
 *
 **************************************************************************
 */
#define ADTS_HEAP_BYTES      (64)
#define ADTS_HEAP_NODE_BYTES (32)


/**
 **************************************************************************
 * \details
 *
 **************************************************************************
 */
typedef enum {
    ADTS_HEAP_MIN = 0x11111111,
    ADTS_HEAP_MAX = 0x22222222,
} adts_heap_type_t;

typedef struct {
    const char reserved[ ADTS_HEAP_NODE_BYTES ];
} adts_heap_node_t;

typedef struct {
    const char reserved[ ADTS_HEAP_BYTES ];
} adts_heap_t;


/**
 **************************************************************************
 * \details
 *
 **************************************************************************
 */
bool
adts_heap_is_empty( adts_heap_t *p_adts_heap );

bool
adts_heap_is_not_empty( adts_heap_t *p_adts_heap );

size_t
adts_heap_entries( adts_heap_t *p_adts_heap );

void
adts_heap_display( adts_heap_t *p_adts_heap );

adts_heap_node_t *
adts_heap_peek( adts_heap_t *p_adts_heap );

adts_heap_node_t *
adts_heap_pop( adts_heap_t *p_adts_heap );

int32_t
adts_heap_push( adts_heap_t       *p_adts_heap,
                adts_heap_node_t  *p_adts_node_heap,
                void              *p_data,
                size_t             bytes,
                int64_t            key );
void
adts_heap_destroy( adts_heap_t *p_adts_heap );

adts_heap_t *
adts_heap_create( adts_heap_type_t type );

void
utest_adts_heap( void );



#endif /* _H_ADTS_HEAP */


