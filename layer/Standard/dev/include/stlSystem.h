/*******************************************************************************
 * stlSystem.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: stlSystem.h 388 2011-03-24 07:39:23Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _STLSYSTEM_H_
#define _STLSYSTEM_H_ 1

/**
 * @file stlSystem.h
 * @brief Standard Layer System(OS/HARDWARE) Routines
 */

#include <stlDef.h>

STL_BEGIN_DECLS


/**
 * @defgroup stlSystem System(os/hardware)
 * @ingroup STL 
 * @{
 */

stlStatus stlGetCpuInfo( stlCpuInfo    * aCpuInfo,
                         stlErrorStack * aErrorStack );

stlStatus stlGetResourceLimit( stlResourceLimitType    aLimitType,
                               stlResourceLimit      * aLimit,
                               stlErrorStack         * aErrorStack );

stlStatus stlSetResourceLimit( stlResourceLimitType    aLimitType,
                               stlResourceLimit      * aLimit,
                               stlErrorStack         * aErrorStack );

/** @} */
    
STL_END_DECLS

#endif /* _STLSYSTEM_H_ */
