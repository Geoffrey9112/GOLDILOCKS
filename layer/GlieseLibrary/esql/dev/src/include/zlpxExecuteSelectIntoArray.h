/*******************************************************************************
 * zlpxExecuteSelectIntoArray.h
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

#ifndef _ZLPXEXECUTESELECTINTOARRAY_H_
#define _ZLPXEXECUTESELECTINTOARRAY_H_ 1

/**
 * @file zlpxExecuteSelectIntoArray.h
 * @brief Gliese Embedded SQL in C precompiler SQL execution library function header file.
 */

#include <stl.h>
#include <goldilocks.h>
#include <goldilocksesql.h>
#include <zlplDef.h>

/************************************************************
 * Transaction
 ************************************************************/

stlStatus zlpxExecuteSelectIntoArray(zlplSqlContext   *aSqlContext,
                                     zlplSqlArgs      *aSqlArgs);

stlBool zlpxIsSelectIntoArray(zlplSqlContext   *aSqlContext,
                              zlplSqlArgs      *aSqlArgs);

stlInt32 zlpxGetParamCount(stlInt32          aHostVarCount,
                           zlplSqlHostVar   *aHostVar,
                           zlplParamIOType   aParamIoType);

stlStatus zlpxCopyParamList( zlplSqlHostVar   *aDestHostVar,
                             zlplSqlHostVar   *aSrcHostVar,
                             stlInt32          aParamCount,
                             zlplParamIOType   aParamIoType );

stlStatus zlpxRemoveIntoClause( zlplSqlContext   *aSqlContext,
                                stlChar          *aSrcSqlStmt,
                                stlChar          *aDestSqlStmt );

#endif /* _ZLPXEXECUTESELECTINTOARRAY_H_ */
