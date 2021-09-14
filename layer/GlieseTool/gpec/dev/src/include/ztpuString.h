/*******************************************************************************
 * ztpuString.h
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


#ifndef _ZTPUSTRING_H_
#define _ZTPUSTRING_H_ 1

#include <dtl.h>

/**
 * @file ztpuString.h
 * @brief Gliese Embedded SQL in C precompiler utility functions
 */

/**
 * @defgroup ztpuString Gliese Embedded SQL in C precompiler string utility functions
 * @ingroup ztp
 * @{
 */


stlStatus ztpuTrimSqlStmt(stlChar *aSrcSqlStmt, stlChar *aDestSqlStmt);
stlChar *ztpuMakeStaticConnString( stlAllocator   *aAllocator,
                                   stlErrorStack  *aErrorStack,
                                   stlChar        *aConnName );

stlStatus ztpuAppendErrorMsg( stlAllocator   *aAllocator,
                              stlErrorStack  *aErrorStack,
                              stlChar       **aOrgMsg,
                              stlChar        *aMsg );

/** @} */

#endif /* _ZTPUSTRING_H_ */
