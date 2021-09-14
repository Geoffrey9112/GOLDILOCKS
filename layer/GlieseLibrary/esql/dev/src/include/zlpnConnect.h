/*******************************************************************************
 * zlpnConnect.h
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

#ifndef _ZLPNCONNECT_H_
#define _ZLPNCONNECT_H_ 1

/**
 * @file zlpnConnect.h
 * @brief Gliese Embedded SQL in C precompiler connection library internal header file.
 */

#include <stl.h>
#include <goldilocks.h>
#include <goldilocksesql.h>
#include <zlplDef.h>


/************************************************************
 * Connection
 ************************************************************/

stlStatus zlpnConnectContext(zlplSqlContext  **aSqlContext,
                             zlplSqlArgs      *aSqlArgs,
                             stlChar          *aConnString,
                             stlChar          *aUserName,
                             stlChar          *aPassword);

stlStatus zlpnDisconnectContext(zlplSqlContext   *aSqlContext,
                                zlplSqlArgs      *aSqlArgs,
                                stlChar          *aConnObject);

stlStatus zlpnDisconnectAll(zlplSqlArgs      *aSqlArgs);

stlStatus zlpnAllocContext(zlplSqlContext **aSqlContext,
                           zlplSqlArgs     *aSqlArgs);

stlStatus zlpnFreeContext(zlplSqlContext **aSqlContext,
                          zlplSqlArgs     *aSqlArgs);

void zlpnLockContext( zlplSqlContext *aSqlContext );

void zlpnUnlockContext( zlplSqlContext *aSqlContext );


#endif /* _ZLPNCONNECT_H_ */
