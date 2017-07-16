
#include <adts_memory.h>


/*
 ****************************************************************************
 *
 ****************************************************************************
 */
void *
adts_mem_zalloc( size_t bytes )
{
    void      *p_mem  = NULL;
    void     **pp_mem = &(p_mem);
    int32_t    rc     = 0;
    int32_t    align  = getpagesize(); /**< platform alignment */

    rc = posix_memalign(pp_mem, align, bytes);
    if (rc) {
        p_mem = NULL;
        goto exception;
    }
    memset(p_mem, 0, bytes);

exception:
    return p_mem;
} /* adts_mem_zalloc() */
