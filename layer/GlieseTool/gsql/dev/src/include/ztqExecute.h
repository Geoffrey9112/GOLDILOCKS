/*******************************************************************************
 * ztqExecute.h
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


#ifndef _ZTQEXECUTE_H_
#define _ZTQEXECUTE_H_ 1

/**
 * @file ztqExecute.h
 * @brief SQL Execute  Definition
 */

/**
 * @defgroup ztqExecute Executor
 * @ingroup ztq
 * @{
 */

stlStatus ztqOpenDatabase( stlChar       * aDsn,
                           stlChar       * aUserId,
                           stlChar       * aPasswd,
                           stlChar       * aNewPassword,
                           stlBool         aStartUp,
                           stlAllocator  * aAllocator,
                           stlErrorStack * aErrorStack );

stlStatus ztqCloseDatabase( stlErrorStack * aErrorStack );

stlStatus ztqPrepareSql( stlChar       * aSqlString,
                         stlAllocator  * aAllocator,
                         ztqSqlStmt    * aZtqSqlStmt,
                         stlErrorStack * aErrorStack );

stlStatus ztqExecuteSql( stlChar       * aSqlString,
                         stlAllocator  * aAllocator,
                         ztqSqlStmt    * aZtqSqlStmt,
                         stlBool         aDirectExecute,
                         stlBool         aPrintResult,
                         stlErrorStack * aErrorStack );

stlStatus ztqPrepareExecuteSql( stlChar       * aSqlString,
                                stlAllocator  * aAllocator,
                                ztqParseTree  * aParseTree,
                                stlBool         aPrintResult,
                                stlErrorStack * aErrorStack );

stlStatus ztqDirectExecuteSql( stlChar       * aSqlString,
                               stlAllocator  * aAllocator,
                               ztqParseTree  * aParseTree,
                               stlBool         aPrintResult,
                               stlErrorStack * aErrorStack );

stlStatus ztqAllocSqlStmt( ztqSqlStmt    ** aZtqSqlStmt,
                           ztqParseTree   * aParseTree,
                           SQLHSTMT         aOdbcStmt,
                           stlAllocator   * aAllocator,
                           stlErrorStack  * aErrorStack );

stlStatus ztqDestPreparedStmt( stlErrorStack * aErrorStack );

stlStatus ztqFreeSqlStmt( ztqSqlStmt    * aZtqSqlStmt,
                          stlErrorStack * aErrorStack );

stlStatus ztqExecuteTran( stlBool         aIsCommit,
                          stlErrorStack * aErrorStack );

stlStatus ztqExecuteAutocommit( stlBool         aAutocommit,
                                stlErrorStack * aErrorStack );

stlStatus ztqBindParameters( SQLHSTMT          aStmtHandle,
                             ztqHostVariable * aHostVarList,
                             stlAllocator    * aAllocator,
                             stlErrorStack   * aErrorStack );

stlStatus ztqBuildTargetColumns( SQLHSTMT           aStmtHandle,
                                 SQLSMALLINT        aColumnCount,
                                 ztqTargetColumn ** aTargetColumn,
                                 stlAllocator     * aAllocator,
                                 stlErrorStack    * aErrorStack );

stlStatus ztqAllocRowBuffer( ztqRowBuffer   ** aRowBuffer,
                             SQLSMALLINT       aColumnCount,
                             stlInt32          aLineBufferCount,
                             ztqTargetColumn * aTargetColumn,
                             stlAllocator    * aAllocator,
                             stlErrorStack   * aErrorStack );

stlStatus ztqPrintOutParamIfExist( ztqRowBuffer    * aRowBuffer,
                                   ztqHostVariable * aHostVarList,
                                   stlBool           aPrintResult,
                                   stlAllocator    * aAllocator,
                                   stlErrorStack   * aErrorStack );

stlStatus ztqMakeRowString( ztqRowBuffer    * aRowBuffer,
                            stlInt32          aLineIdx,
                            ztqTargetColumn * aTargetColumn,
                            stlBool           aPrintResult,
                            stlErrorStack   * aErrorStack );

stlStatus ztqMakeColString( ztqColBuffer    * aColBuffer,
                            stlInt32          aLineIdx,
                            ztqTargetColumn * aTargetColumn,
                            stlErrorStack   * aErrorStack );

void ztqSetInitRowStringLen( ztqRowBuffer    * aRowBuffer,
                             ztqTargetColumn * aTargetColumn );

stlStatus ztqGetParameterColSizeAndDecimalDigit( stlInt32          aDataType,
                                                 stlInt64          aPrecision,
                                                 stlInt64          aScale,
                                                 SQLULEN         * aColumnSize,
                                                 SQLSMALLINT     * aDecimalDigits,
                                                 stlErrorStack   * aErrorStack );

stlStatus ztqSetIntervalLeadingPrecision( SQLHSTMT         aStmtHandle,
                                          SQLUSMALLINT     aParameterNumber,
                                          stlInt32         aDataType,
                                          stlInt32         aLeadingPrecision,
                                          stlErrorStack  * aErrorStack );

stlStatus ztqSetCharacterLengthUnits( SQLHSTMT              aStmtHandle,
                                      SQLUSMALLINT          aParameterNumber,
                                      stlInt32              aDataType,
                                      dtlStringLengthUnit   aStringLengthUnit,
                                      stlErrorStack       * aErrorStack );

stlStatus ztqBuildNLSToCharFormatInfo( stlAllocator  * aAllocator,
                                       stlErrorStack * aErrorStack );

stlStatus ztqGetNlsDateTimeFormatString( dtlDataType     aDataType,
                                         stlChar       * aPropertyName,
                                         stlChar       * aValue,
                                         stlAllocator  * aAllocator,
                                         stlErrorStack * aErrorStack );

stlStatus ztqGetNLSFormatPropertyID( stlChar       * aPropertyName,
                                     stlInt16      * aPropertyID,
                                     stlAllocator  * aAllocator,
                                     stlErrorStack * aErrorStack );

stlStatus ztqGetNLSFormatString( stlInt16        aPropertyID,
                                 stlChar       * aValue,
                                 stlAllocator  * aAllocator,
                                 stlErrorStack * aErrorStack );

stlStatus ztqMakeColFormatString( ztqTargetColumn * aTargetColumn,
                                  stlChar         * aFormatString,
                                  stlInt64        * aFormatStringLen,
                                  stlErrorStack   * aErrorStack );

stlStatus ztqResizeNumberString( ztqTargetColumn * aTargetColumn,
                                 stlInt64          aAvailableSize,
                                 stlChar         * aNumberString,
                                 stlInt64        * aNumberStringLen,
                                 stlErrorStack   * aErrorStack );

/** @} */

#endif /* _ZTQEXECUTE_H_ */

