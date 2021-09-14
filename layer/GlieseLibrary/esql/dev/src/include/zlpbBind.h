/*******************************************************************************
 * zlpbBind.h
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

#ifndef _ZLPBBIND_H_
#define _ZLPBBIND_H_ 1

/**
 * @file zlpbBind.h
 * @brief Gliese Embedded SQL in C precompiler library internal header file.
 */

#include <dtl.h>
#include <goldilocks.h>
#include <goldilocksesql.h>
#include <zlplDef.h>


/************************************************************
 * Bind
 ************************************************************/

stlInt32 zlpbGetDataSize( zlplCDataType aDataType );
stlInt32 zlpbGetDataBufferLength( zlplSqlHostVar *aHostVar );

stlStatus zlpbAdjustDataType( zlplSqlContext  * aSqlContext,
                              zlplSqlArgs     * aSqlArgs );

stlStatus zlpbAdjustDataTypeHostVar( stlInt32        aHostVarCnt,
                                     zlplSqlHostVar *aHostVar );

stlStatus zlpbBindParams( SQLHANDLE          aStmtHandle,
                          zlplSqlArgs      * aSqlArgs,
                          stlInt32           aHostCnt,
                          zlplSqlHostVar   * aHostVar,
                          stlChar         ** aValueArray,
                          SQLLEN          ** aSymbolIndArray,
                          zlplParamIOType  * aDynParamIOType );

stlStatus zlpbBindCols( SQLHANDLE         aStmtHandle,
                        zlplSqlArgs     * aSqlArgs,
                        stlInt32          aHostCnt,
                        zlplSqlHostVar  * aHostVar,
                        stlChar        ** aValueArray,
                        SQLLEN         ** aSymbolIndArray );

stlStatus zlpbSetBeforeInfo(zlplSqlContext   * aSqlContext,
                            zlplSqlArgs      * aSqlArgs,
                            SQLHANDLE          aStmtHandle,
                            stlInt32           aHostVarCount,
                            zlplSqlHostVar   * aHostVar,
                            stlInt32           aArrayIdx,
                            SQLULEN          * aParamsProcessed,
                            SQLUSMALLINT     * aParamStatusArray,
                            stlChar         ** aValueArray,
                            SQLLEN          ** aSymbolIndArray,
                            zlplParamIOType  * aDynParamIOType,
                            stlBool            aIsBindParam );

stlStatus zlpbSetIntervalLeadingPrecision(zlplSqlArgs *aSqlArgs,
                                          SQLHANDLE    aStmtHandle,
                                          stlInt32     aColumnIdx,
                                          stlBool      aIsBindParam);

stlStatus zlpbSetNumberTypeInfo(zlplSqlArgs *aSqlArgs,
                                SQLHANDLE    aStmtHandle,
                                stlInt32     aColumnIdx,
                                stlBool      aIsBindParam,
                                stlInt32     aPrecision,
                                stlInt32     aScale);

stlStatus zlpbSetNumberPrecision(zlplSqlArgs *aSqlArgs,
                                 SQLHANDLE    aStmtHandle,
                                 stlInt32     aDescType,
                                 stlInt32     aColumnIdx,
                                 stlInt32     aPrecision,
                                 stlInt32     aScale);
stlStatus zlpbSetCharBufLen(zlplSqlArgs *aSqlArgs,
                            SQLHANDLE    aStmtHandle,
                            stlInt32     aColumnIdx,
                            SQLLEN       aBufLen);

stlStatus zlpbSetAfterInfo( zlplSqlContext   * aSqlContext,
                            zlplSqlArgs      * aSqlArgs,
                            SQLHANDLE          aStmtHandle,
                            stlInt32           aUnsafeNull,
                            stlInt32           aHostVarCount,
                            zlplSqlHostVar   * aHostVar,
                            stlInt32           aArrayIdx,
                            stlChar         ** aValueArray,
                            SQLLEN          ** aSymbolIndArray,
                            zlplParamIOType  * aDynParamIOType );

#endif /* _ZLPBBIND_H_ */
