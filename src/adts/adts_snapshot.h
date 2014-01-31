

#ifndef _H_ADTS_SNAPSHOT
#define _H_ADTS_SNAPSHOT

#include <inttypes.h>

/**
 **************************************************************************
 * \details
 *
 **************************************************************************
 */
#define ADTS_SNAPSHOT_BYTES (128)


/**
 **************************************************************************
 * \details
 *
 **************************************************************************
 */
typedef struct {
    char reserved[ ADTS_SNAPSHOT_BYTES ];
} adts_snapshot_t;


/**
 ****************************************************************************
 * \brief
 *   Private interface for snapshot services
 *
 * \details
 *   This function should not be called directly.  Consumers must use the
 *   macro services defined below.
 *
 ****************************************************************************
 */
void
adts_snapshot_private( adts_snapshot_t *p_adt_snap,
                       int32_t          line,
                       char            *p_file,
                       char            *p_func,
                       char            *p_date,
                       char            *p_time );
void
adts_snapshot_display( adts_snapshot_t *p_adt_snap );

void
utest_adts_snapshot( void );


/**
 **************************************************************************
 * \brief
 *   Public interface
 *
 * \details
 *
 **************************************************************************
 */
#define adts_snapshot( _p_adts_snap )      \
    adts_snapshot_private( (_p_adts_snap), \
                           __LINE__,       \
                           __FILE__,       \
                           __FUNCTION__,   \
                           __DATE__,       \
                           __TIME__ );



#endif /* _H_ADTS_SNAPSHOT */

