/*******************************************************************************
 * ztqCmdParser.h
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


#ifndef _ZTQCMDPARSER_H_
#define _ZTQCMDPARSER_H_ 1

/**
 * @file ztqCmdParser.h
 * @brief Command Parser for Gliese SQL 
 */

/**
 * @defgroup ztqCmdParser Command Parser
 * @ingroup ztq
 * @{
 */

#ifndef YYSTYPE
#define YYSTYPE       void *
#endif
#ifndef YY_EXTRA_TYPE
#define YY_EXTRA_TYPE ztqCmdParseParam *
#endif

YYSTYPE ztqMakeCmdExecAssignTree( ztqCmdParseParam * aParam, ztqHostVariable * aHostVariable );
YYSTYPE ztqMakeCmdExecSqlTree( ztqCmdParseParam * aParam );
YYSTYPE ztqMakeCmdPrepareSqlTree( ztqCmdParseParam * aParam );
YYSTYPE ztqMakeCmdDynamicSqlTree( ztqCmdParseParam * aParam, YYSTYPE aHostVariable );
YYSTYPE ztqMakeCmdEditTree( ztqCmdParseParam * aParam,
                            stlChar          * aFilename,
                            stlChar          * aHistoryNumStr);
YYSTYPE ztqMakeCmdSpoolTree( ztqCmdParseParam * aParam,
                             stlChar          * aFilename,
                             ztqSpoolFlag       aSpoolFlag );
YYSTYPE ztqMakeCmdExecPreparedTree( ztqCmdParseParam * aParam );
YYSTYPE ztqMakeCmdPrintTree( ztqCmdParseParam * aParam, YYSTYPE aIdentifier );
YYSTYPE ztqMakeCmdDeclareVarTree( ztqCmdParseParam * aParam, YYSTYPE aIdentifier, ztqDataType * aDataType );
YYSTYPE ztqMakeCmdSetCallstackTree( ztqCmdParseParam * aParam, stlBool aOn );
YYSTYPE ztqMakeCmdSetVerticalTree( ztqCmdParseParam * aParam, stlBool aOn );
YYSTYPE ztqMakeCmdSetTimingTree( ztqCmdParseParam * aParam, stlBool aOn );
YYSTYPE ztqMakeCmdSetVerboseTree( ztqCmdParseParam * aParam, stlBool aOn );
YYSTYPE ztqMakeCmdSetColorTree( ztqCmdParseParam * aParam, stlBool aOn );
YYSTYPE ztqMakeCmdSetErrorTree( ztqCmdParseParam * aParam, stlBool aOn );
YYSTYPE ztqMakeCmdSetAutocommitTree( ztqCmdParseParam * aParam, stlBool aOn );
YYSTYPE ztqMakeCmdSetResultTree( ztqCmdParseParam * aParam, stlBool aOn );
YYSTYPE ztqMakeCmdSetLinesizeTree( ztqCmdParseParam * aParam, stlChar * aLineSizeStr );
YYSTYPE ztqMakeCmdSetPagesizeTree( ztqCmdParseParam * aParam, stlChar * aPageSizeStr );
YYSTYPE ztqMakeCmdSetColsizeTree( ztqCmdParseParam * aParam, stlChar * aColSizeStr );
YYSTYPE ztqMakeCmdSetNumsizeTree( ztqCmdParseParam * aParam, stlChar * aNumSizeStr );
YYSTYPE ztqMakeCmdSetDdlsizeTree( ztqCmdParseParam * aParam, stlChar * aDdlSizeStr );
YYSTYPE ztqMakeCmdSetHistoryTree( ztqCmdParseParam * aParam, stlChar * aHistorySizeStr );
YYSTYPE ztqMakeCmdPrintHistoryTree( ztqCmdParseParam * aParam );
YYSTYPE ztqMakeCmdImportTree( ztqCmdParseParam * aParam, stlChar * aImportFile );
YYSTYPE ztqMakeCmdExecHistoryTree( ztqCmdParseParam * aParam, stlChar * aHistoryNumStr );
YYSTYPE ztqMakeCmdQuitTree( ztqCmdParseParam * aParam );
YYSTYPE ztqMakeCmdHelpTree( ztqCmdParseParam * aParam );
YYSTYPE ztqMakeCmdDescTree( ztqCmdParseParam * aParam, ztqIdentifier * aIdentifierList );
YYSTYPE ztqMakeCmdIdescTree( ztqCmdParseParam * aParam, ztqIdentifier * aIdentifierList );
YYSTYPE ztqMakeCharType( ztqCmdParseParam * aParam, stlInt32 aDataType, dtlStringLengthUnit aStringLengthUnit, YYSTYPE aNumStr );
YYSTYPE ztqMakeNumberType( ztqCmdParseParam * aParam, stlInt32 aDataType, stlInt32 aPrecision, stlInt32 aScale );
YYSTYPE ztqMakeNumericType( ztqCmdParseParam * aParam, stlInt32 aDataType, YYSTYPE aPrecisionStr, YYSTYPE aScaleStr );
YYSTYPE ztqMakeDateType( ztqCmdParseParam * aParam, stlInt32 aDataType, stlInt32 aPrecision, stlInt32 aScale );
YYSTYPE ztqMakeRowIdType( ztqCmdParseParam * aParam, stlInt32 aDataType, stlInt32 aPrecision, stlInt32 aScale );
YYSTYPE ztqMakeTemporalType( ztqCmdParseParam * aParam, stlInt32 aDataType, YYSTYPE aPrecisionStr, stlInt32 aScale );
YYSTYPE ztqMakeIntervalType( ztqCmdParseParam * aParam, stlInt32 aDataType, YYSTYPE aPrecisionStr, stlInt32 aScale );
YYSTYPE ztqMakeIntervalSecondType( ztqCmdParseParam * aParam, stlInt32 aDataType, YYSTYPE aPrecisionStr, YYSTYPE aScaleStr );

YYSTYPE ztqMakeIdentifier( ztqCmdParseParam * aParam, YYSTYPE aIdentifierStr );
YYSTYPE ztqAddIdentifier( ztqCmdParseParam * aParam, ztqIdentifier * aIdentifierList, ztqIdentifier * aIdentifier );
YYSTYPE ztqMakeCmdExplainPlanTree( ztqCmdParseParam * aParam, stlUInt16 aExplainPlanType );

YYSTYPE ztqMakeCmdAllocStmtTree( ztqCmdParseParam * aParam,
                                 YYSTYPE            aIdentifier );
YYSTYPE ztqMakeCmdFreeStmtTree( ztqCmdParseParam * aParam,
                                YYSTYPE            aIdentifier );
YYSTYPE ztqMakeCmdUseStmtExecSqlTree( ztqCmdParseParam * aParam,
                                      YYSTYPE            aIdentifier );
YYSTYPE ztqMakeCmdUseStmtPrepareSqlTree( ztqCmdParseParam * aParam,
                                         YYSTYPE            aIdentifier );
YYSTYPE ztqMakeCmdUseStmtExecPreparedTree( ztqCmdParseParam * aParam,
                                           YYSTYPE            aIdentifier );
YYSTYPE ztqMakeCmdStartupTree( ztqCmdParseParam * aParam,
                               ztqStartupPhase    aStartupPhase );
YYSTYPE ztqMakeCmdShutdownTree( ztqCmdParseParam * aParam,
                                ztqShutdownMode    aShutdownMode );
YYSTYPE ztqMakeCmdConnectTree( ztqCmdParseParam * aParam,
                               YYSTYPE            aUserId,
                               YYSTYPE            aPassword,
                               YYSTYPE            aNewPassword,
                               ztqConnectAsUser   aAsUser );

YYSTYPE ztqMakeCmdXaOpenTree( ztqCmdParseParam * aParam, YYSTYPE aXaInfo, ztqXaFlag * aXaFlags );
YYSTYPE ztqMakeCmdXaCloseTree( ztqCmdParseParam * aParam, YYSTYPE aXaInfo, ztqXaFlag * aXaFlags );
YYSTYPE ztqMakeCmdXaStartTree( ztqCmdParseParam * aParam, YYSTYPE aXid, ztqXaFlag * aXaFlags );
YYSTYPE ztqMakeCmdXaEndTree( ztqCmdParseParam * aParam, YYSTYPE aXid, ztqXaFlag * aXaFlags );
YYSTYPE ztqMakeCmdXaCommitTree( ztqCmdParseParam * aParam, YYSTYPE aXid, ztqXaFlag * aXaFlags );
YYSTYPE ztqMakeCmdXaRollbackTree( ztqCmdParseParam * aParam, YYSTYPE aXid, ztqXaFlag * aXaFlags );
YYSTYPE ztqMakeCmdXaPrepareTree( ztqCmdParseParam * aParam, YYSTYPE aXid, ztqXaFlag * aXaFlags );
YYSTYPE ztqMakeCmdXaForgetTree( ztqCmdParseParam * aParam, YYSTYPE aXid, ztqXaFlag * aXaFlags );
YYSTYPE ztqMakeCmdXaRecoverTree( ztqCmdParseParam * aParam, YYSTYPE aCountStr, ztqXaFlag * aXaFlags );
YYSTYPE ztqMakeXaFlag( ztqCmdParseParam * aParam, stlInt64 aXaFlag );
YYSTYPE ztqAddXaFlag( ztqCmdParseParam * aParam, ztqXaFlag * aXaFlagList, ztqXaFlag * aXaFlag );
YYSTYPE ztqXaXidFromNumber( ztqCmdParseParam * aParam, YYSTYPE aXid );
YYSTYPE ztqXaXidFromString( ztqCmdParseParam * aParam, YYSTYPE aXid );

YYSTYPE ztqMakeCmdDDLTree( ztqCmdParseParam * aParam,
                           ztqCommandType     aCommandType,
                           ztqIdentifier    * aIdentifierList,
                           ztqPrintDDLType    aDDLType );

stlInt32 ztqCmderror( ztqCmdParseParam * aParam,
                      void             * aScanner,
                      const stlChar    * aMsg );

stlStatus ztqCmdParseIt( stlChar         * aCmd,
                         stlAllocator    * aAllocator,
                         ztqParseTree   ** aParseTree,
                         stlErrorStack   * aErrorStack );

stlInt32 ztqCmdlex( YYSTYPE *, void * );
stlInt32 ztqCmdlex_init( void ** );
stlInt32 ztqCmdlex_destroy( void * );
void     ztqCmdset_extra( YY_EXTRA_TYPE, void * );
stlInt32 ztqCmdparse( ztqCmdParseParam *, void * );

/** @} */

#endif /* _ZTQCMDPARSER_H_ */
