

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
    const char reserved[ ADTS_LIST_BYTES ];
} adts_list_t;

typedef struct {
    void   *p_data;
    size_t  bytes;
} adts_list_node_public_t;

typedef union {
    const char                    reserved[ ADTS_LIST_ELEM_BYTES ];
    const adts_list_node_public_t pub;
} adts_list_node_t;



void
utest_adts_list( void );


#endif /* _H_ADTS_LIST */

