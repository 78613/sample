#pragma once

#include <string.h>
#include <stdbool.h>
#include <inttypes.h>


/*!
 * \brief Shuffle array elements.
 *
 * \param[in] arr[] - Input array.
 * \param[in] elems - Elements in input array.
 *
 * \return none
 *
 * \note Time:  O(n)
 * \note Space: O(1)
 */
void
adts_shuffle( int32_t arr[],
              size_t  elems );

/*!
 * \brief Determine is array is unsorted
 *
 * \param[in] arr[] - Input array.
 * \param[in] elems - Elements in input array.
 *
 * \return true  - Input array is unsorted.
 * \return false - Input array is sorted.
 *
 * \note Time:  O(n)
 * \note Space: O(1)
 */
bool
adts_arr_is_not_sorted( int32_t arr[],
                        size_t  elems );

/*!
 * \brief Determine is array is sorted
 *
 * \param[in] arr[] - Input array.
 * \param[in] elems - Elements in input array.
 *
 * \return false - Input array is unsorted.
 * \return true  - Input array is sorted.
 *
 * \note Time:  O(n)
 * \note Space: O(1)
 */
bool
adts_arr_is_sorted( int32_t arr[],
                    size_t  elems );

/*!
 * \brief Display array elements in ascending order.
 *
 * \param[in] arr[] - Input array.
 * \param[in] elems - Elements in input array.
 *
 * \return void
 *
 * \note Time:  O(n)
 * \note Space: O(1)
 */
void
adts_array_display( int32_t       arr[],
                    const size_t  elems );

/*!
 * \brief Array sort using the shell algorithm.
 *
 * \param[in] arr[] - Input array.
 * \param[in] elems - Elements in input array.
 *
 * \return void
 *
 * \note Time:      O(n^3/2) - h = 3 intrval, subject to change in the future
 * \note Space:     O(1)
 * \note Stability: no
 */
void
adts_sort_shell( int32_t       arr[],
                 const size_t  elems );


/*!
 * \brief Extended array sort using the shell algorithm.
 *
 * This implementation allows for consumer selected upper and lower bounds.
 *
 * \param[in] arr[] - Input array.
 * \param[in] elems - Elements in input array.
 *
 * \return void
 *
 * \note Time:      O(n^3/2) - h = 3 intrval, subject to change in the future
 * \note Space:     O(1)
 * \note Stability: no
 */
void
adts_sort_shell_ext( int32_t arr[],
                     size_t  lo,
                     size_t  hi );

/*!
 * \brief Array sort using the insertion algorithm.
 *
 * \param[in] arr[] - Input array.
 * \param[in] elems - Elements in input array.
 *
 * \return void
 *
 * \note Time:      O(n^2)
 * \note Space:     O(1)
 * \note Stability: yes
 */
void
adts_sort_insertion( int32_t arr[],
                     size_t  elems );

/*!
 * \brief Extended array sort using the insertion algorithm.
 *
 * This implementation allows for consumer selected upper and lower bounds.
 *
 * \param[in] arr[] - Input array.
 * \param[in] elems - Elements in input array.
 *
 * \return void
 *
 * \note Time:      O(n^2)
 * \note Space:     O(1)
 * \note Stability: yes
 */
void
adts_sort_insertion_ext( int32_t arr[],
                         size_t  lo,
                         size_t  hi );
/*!
 * \brief Array sort using the merge algorithm.
 *
 * \param[in] arr[] - Input array.
 * \param[in] elems - Elements in input array.
 *
 * \return void
 *
 * \note Time:      O(n log n)
 * \note Space:     O(2n)
 * \note Stability: yes
 */
int32_t
adts_sort_merge( int32_t arr[],
                 size_t  elems );

/*!
 * \brief Array sort using the merge algorithm.
 *
 * \param[in] arr[] - Input array.
 * \param[in] elems - Elements in input array.
 *
 * \return void
 *
 * \note Time:      O(n log n) - Average O(n^2) worst case very unlikely
 * \note Space:     O(1)
 * \note Stability: yes
 */
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
void
utest_adts_sort_public( void );

