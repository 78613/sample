

#ifndef _H_ADTS_LIST
#define _H_ADTS_LIST

#include <string.h>
#include <stdbool.h>
#include <inttypes.h>

/**
 **************************************************************************
 * \details
 *
 **************************************************************************
 */
#define ADTS_LIST_BYTES      (32)
#define ADTS_LIST_ELEM_BYTES (32)


/**
 **************************************************************************
 * \details
 *
 **************************************************************************
 */
typedef struct {
    char reserved[ ADTS_LIST_BYTES ];
} adts_list_t;

typedef struct {
    char reserved[ ADTS_LIST_ELEM_BYTES ];
} adts_list_elem_t;



void
utest_adts_list( void );


#endif /* _H_ADTS_LIST */

