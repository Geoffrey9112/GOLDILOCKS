/*******************************************************************************
 * stlShm.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _STLSHM_H_
#define _STLSHM_H_ 1

/**
 * @file stlShm.h
 * @brief Standard Layer Shared Memory Routines
 */

#include <stlDef.h>

STL_BEGIN_DECLS


/**
 * @defgroup stlShm Shared Memory
 * @ingroup STL 
 * @{
 */

stlStatus stlCreateShm(stlShm        * aShm,
                       stlChar       * aShmName,
                       stlInt32        aShmKey,
                       void          * aShmAddr,
                       stlSize         aReqSize,
                       stlBool         aHugeTlb,
                       stlErrorStack * aErrorStack);

stlStatus stlAttachShm(stlShm        * aShm,
                       stlChar       * aShmName,
                       stlInt32        aShmKey,
                       void          * aShmAddr,
                       stlErrorStack * aErrorStack);

stlStatus stlDetachShm(stlShm        * aShm,
                       stlErrorStack * aErrorStack);

stlStatus stlDestroyShm(stlShm        * aShm,
                        stlErrorStack * aErrorStack);

void * stlGetShmBaseAddr(stlShm * aShm);
stlUInt32 stlGetShmKey(stlShm * aShm);

stlSize stlGetShmSize(stlShm * aShm);
stlBool stlSupportShareAddressShm();

/** @} */
    
STL_END_DECLS

#endif /* _STLSHM_H_ */
