/*******************************************************************************
 * zlpnConnStr.h
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

#ifndef _ZLPNCONNSTR_H_
#define _ZLPNCONNSTR_H_ 1

/**
 * @file zlpnConnStr.h
 * @brief Gliese Embedded SQL in C precompiler connection library internal header file.
 */

#include <stl.h>
#include <goldilocks.h>
#include <goldilocksesql.h>
#include <zlplDef.h>


extern stlStaticHash   *gConnObjSymTab;
extern stlSpinLock      gConnStrLatch;

/************************************************************
 * Connection Object
 ************************************************************/

stlStatus zlpnCreateSqlContextHash( stlErrorStack *aErrorStack );

stlStatus zlpnDestroySqlContextHash( stlErrorStack *aErrorStack );

stlInt32 zlpnCompareConnStrFunc(void *aKeyA, void *aKeyB);

stlUInt32 zlpnHashConnStrFunc(void * aKey, stlUInt32 aBucketCount);

stlStatus zlpnAddSqlContextToHash( zlplSqlContext  *aSqlContext,
                                   stlErrorStack   *aErrorStack );

stlStatus zlpnAddUniqueSqlContextToHash( zlplSqlContext   *aSqlContext,
                                         stlBool          *aIsDuplicate,
                                         stlErrorStack    *aErrorStack );

stlStatus zlpnLookupSqlContext( stlChar          *aConnStr,
                                zlplSqlContext  **aSqlContext,
                                stlErrorStack    *aErrorStack );

zlplSqlContext *zlpnLookupConnStrSymbolDebug( stlChar *aConnStr );

stlStatus zlpnDeleteSqlContextFromHash( zlplSqlContext   *aSqlContext,
                                        stlErrorStack    *aErrorStack );

stlStatus zlpnGetRealContext( zlplSqlContext   *aGivenSqlContext,
                              zlplSqlArgs      *aSqlArgs,
                              zlplSqlContext  **aRealSqlContext );

stlStatus zlpnPutContextList( zlplSqlContext *aSqlContext );

stlStatus zlpnGetContextList( zlplSqlContext **aSqlContext );

stlStatus zlpnUnlinkContextList( zlplSqlContext *aSqlContext );

#endif /* _ZLPNCONNSTR_H_ */
