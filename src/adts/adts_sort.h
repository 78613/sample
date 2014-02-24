
#ifndef _H_ADTS_SORT
#define _H_ADTS_SORT

#include <string.h>
#include <stdbool.h>
#include <inttypes.h>



/**
 **************************************************************************
 * \details
 *   Prototypes
 *
 **************************************************************************
 */
void
adts_shuffle( int32_t arr[],
              size_t  elems );
bool
adts_arr_is_not_sorted( int32_t arr[],
                        size_t  elems );
bool
adts_arr_is_sorted( int32_t arr[],
                    size_t  elems );
void
adts_array_display( int32_t       arr[],
                    const size_t  elems );
void
adts_sort_shell( int32_t       arr[],
                 const size_t  elems );
void
adts_sort_shell_ext( int32_t arr[],
                     size_t  lo,
                     size_t  hi );
void
adts_sort_insertion( int32_t arr[],
                     size_t  elems );
int32_t
adts_sort_merge( int32_t arr[],
                 size_t  elems );
void
adts_sort_quick( int32_t arr[],
                 size_t  elems );



/**
 **************************************************************************
 * \details
 *   Test entrypoint
 *
 **************************************************************************
 */
void
utest_adts_sort( void );


#endif /* _H_ADTS_SORT */
