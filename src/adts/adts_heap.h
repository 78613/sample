

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
#define ADTS_HEAP_BYTES (64)


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
    char reserved[ ADTS_HEAP_BYTES ];
} adts_heap_t;


/**
 **************************************************************************
 * \details
 *
 **************************************************************************
 */
void
utest_adts_heap( void );



#endif /* _H_ADTS_HEAP */


