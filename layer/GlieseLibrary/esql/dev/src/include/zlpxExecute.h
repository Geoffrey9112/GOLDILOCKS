/*******************************************************************************
 * zlpxExecute.h
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

#ifndef _ZLPXEXECUTE_H_
#define _ZLPXEXECUTE_H_ 1

/**
 * @file zlpxExecute.h
 * @brief Gliese Embedded SQL in C precompiler SQL execution library function header file.
 */

#include <stl.h>
#include <goldilocks.h>
#include <goldilocksesql.h>
#include <zlplDef.h>

/************************************************************
 * Transaction
 ************************************************************/

stlStatus zlpxEndTranContext(zlplSqlContext   *aSqlContext,
                             zlplSqlArgs      *aSqlArgs,
                             stlBool           aIsCommit,
                             stlBool           aIsRelease);

stlStatus zlpxSetAutocommitContext(zlplSqlContext   *aSqlContext,
                                   zlplSqlArgs      *aSqlArgs,
                                   stlBool           aIsOn);

stlStatus zlpxExecuteContext(zlplSqlContext   *aSqlContext,
                             zlplSqlArgs      *aSqlArgs);
stlStatus zlpxCreateSqlSymbol( zlplSqlContext       * aSqlContext,
                               zlplSqlArgs          * aSqlArgs,
                               stlBool                aDoPrepare,
                               zlplSqlStmtSymbol   ** aQuerySymbol );

stlStatus zlpxPrepareSqlStmt(zlplSqlContext       * aSqlContext,
                             zlplSqlArgs          * aSqlArgs,
                             zlplSqlStmtSymbol    * aQuerySymbol);

stlStatus zlpxAllocHostVarBuffer(zlplSqlContext       * aSqlContext,
                                 zlplSqlArgs          * aSqlArgs,
                                 stlInt32               aArraySize,
                                 zlplSqlStmtSymbol    * aSqlSymbol);

stlStatus zlpxBindSqlStmt(zlplSqlContext       * aSqlContext,
                          zlplSqlArgs          * aSqlArgs,
                          stlInt32               aArraySize,
                          zlplSqlStmtSymbol    * aQuerySymbol);

stlBool zlpxIsEqualHostVar(zlplSqlHostVar  *aHostVar1,
                           zlplSqlHostVar  *aHostVar2,
                           stlInt32         aHostVarCount);

stlStatus zlpxAllocHostVarTempBuffer(zlplSqlContext       * aSqlContext,
                                     zlplSqlStmtSymbol    * aQuerySymbol,
                                     stlInt32               aArraySize,
                                     stlInt32               aHostVarCnt,
                                     zlplSqlHostVar       * aHostVar);

stlStatus zlpxFreeHostVarTempBuffer(zlplSqlContext       * aSqlContext,
                                    zlplSqlStmtSymbol    * aQuerySymbol );

#endif /* _ZLPXEXECUTE_H_ */
