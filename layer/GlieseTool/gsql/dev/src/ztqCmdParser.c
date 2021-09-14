/*******************************************************************************
 * ztqCmdParser.c
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

#include <stl.h>
#include <goldilocks.h>
#include <sqlext.h>
#include <ztqDef.h>
#include <ztqCmdParser.h>

/**
 * @file ztqCmdParser.c
 * @brief Parser Routines
 */

/**
 * @addtogroup ztqCmdParser
 * @{
 */

YYSTYPE ztqMakeCmdExecSqlTree( ztqCmdParseParam * aParam )
{
    ztqCmdExecSqlTree * sExecSqlTree = NULL;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdExecSqlTree ),
                           (void**)&sExecSqlTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sExecSqlTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_EXEC_SQL;
    sExecSqlTree->mSqlStartPos = aParam->mSqlPosition;

    return (YYSTYPE)sExecSqlTree;
}


YYSTYPE ztqMakeCmdPrepareSqlTree( ztqCmdParseParam * aParam )
{
    ztqCmdPrepareSqlTree * sPrepareSqlTree = NULL;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdPrepareSqlTree ),
                           (void**)&sPrepareSqlTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sPrepareSqlTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_PREPARE_SQL;
    sPrepareSqlTree->mSqlStartPos = aParam->mSqlPosition;

    return (YYSTYPE)sPrepareSqlTree;
}


YYSTYPE ztqMakeCmdDynamicSqlTree( ztqCmdParseParam * aParam,
                                  YYSTYPE            aHostVariable )
{
    ztqCmdDynamicSqlTree * sDynamicSqlTree = NULL;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdDynamicSqlTree ),
                           (void**)&sDynamicSqlTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sDynamicSqlTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_DYNAMIC_SQL;
    stlStrcpy( sDynamicSqlTree->mVariableName, aHostVariable );

    return (YYSTYPE)sDynamicSqlTree;
}


YYSTYPE ztqMakeCmdEditTree( ztqCmdParseParam * aParam,
                            stlChar          * aFilename,
                            stlChar          * aHistoryNumStr)
{
    ztqCmdEditTree        * sEditTree = NULL;
    stlInt64                sHistoryNum;
    stlChar               * sEndPtr;

    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdEditTree ),
                           (void**)&sEditTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    if( aHistoryNumStr != NULL )
    {
        if( stlStrToInt64( aHistoryNumStr,
                           STL_NTS,
                           &sEndPtr,
                           10,
                           &sHistoryNum,
                           aParam->mErrorStack )
            != STL_SUCCESS )
        {
            aParam->mErrStatus = STL_FAILURE;
            return (YYSTYPE)NULL;
        }
    }
    else
    {
        sHistoryNum = 0;
    }

    sEditTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_EDIT;
    sEditTree->mSqlFile = aFilename;
    sEditTree->mHistoryNum = sHistoryNum;

    return (YYSTYPE)sEditTree;
}

YYSTYPE ztqMakeCmdSpoolTree( ztqCmdParseParam * aParam,
                             stlChar          * aFilename,
                             ztqSpoolFlag       aSpoolFlag )
{
    ztqCmdSpoolTree       * sSpoolTree = NULL;

    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdSpoolTree ),
                           (void**)&sSpoolTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sSpoolTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_SPOOL;
    sSpoolTree->mSpoolFile = aFilename;
    sSpoolTree->mSpoolFlag = aSpoolFlag;

    return (YYSTYPE)sSpoolTree;
}

YYSTYPE ztqMakeCmdExecPreparedTree( ztqCmdParseParam * aParam )
{
    ztqCmdExecPreparedTree * sExecPreparedTree = NULL;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdExecPreparedTree ),
                           (void**)&sExecPreparedTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sExecPreparedTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_EXEC_PREPARED;

    return (YYSTYPE)sExecPreparedTree;
}

YYSTYPE ztqMakeCmdExecAssignTree( ztqCmdParseParam * aParam,
                                  ztqHostVariable  * aHostVariable )
{
    ztqCmdExecAssignTree * sExecAssignTree = NULL;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdExecAssignTree ),
                           (void**)&sExecAssignTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sExecAssignTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_EXEC_ASSIGN;
    sExecAssignTree->mHostVariable = aHostVariable;
    sExecAssignTree->mSqlStartPos = aParam->mSqlPosition;

    return (YYSTYPE)sExecAssignTree;
}

YYSTYPE ztqMakeCmdPrintTree( ztqCmdParseParam * aParam,
                             YYSTYPE            aIdentifier )
{
    ztqCmdPrintTree * sPrintTree = NULL;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdExecSqlTree ),
                           (void**)&sPrintTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sPrintTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_PRINT;

    if( aIdentifier == NULL )
    {
        sPrintTree->mName[0] = '\0';
    }
    else
    {
        stlStrncpy( sPrintTree->mName,
                    (const stlChar*)aIdentifier,
                    STL_MAX_SQL_IDENTIFIER_LENGTH );
    }

    return (YYSTYPE)sPrintTree;
}

YYSTYPE ztqMakeCmdDeclareVarTree( ztqCmdParseParam * aParam,
                                  YYSTYPE            aIdentifier,
                                  ztqDataType      * aDataType )
{
    ztqCmdDeclareVarTree * sVarTree = NULL;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdDeclareVarTree ),
                           (void**)&sVarTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sVarTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_VAR;
    sVarTree->mDataType = aDataType;
    
    stlStrncpy( sVarTree->mName,
                (const stlChar*)aIdentifier,
                STL_MAX_SQL_IDENTIFIER_LENGTH );

    return (YYSTYPE)sVarTree;
}

YYSTYPE ztqMakeCmdSetCallstackTree( ztqCmdParseParam * aParam,
                                    stlBool            aOn )
{
    ztqCmdSetCallstackTree * sCallstackTree = NULL;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdSetCallstackTree ),
                           (void**)&sCallstackTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sCallstackTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_SET_CALLSTACK;
    sCallstackTree->mOn = aOn;

    return (YYSTYPE)sCallstackTree;
}

YYSTYPE ztqMakeCmdSetVerticalTree( ztqCmdParseParam * aParam,
                                   stlBool            aOn )
{
    ztqCmdSetVerticalTree * sParseTree = NULL;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdSetVerticalTree ),
                           (void**)&sParseTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sParseTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_SET_VERTICAL;
    sParseTree->mOn = aOn;

    return (YYSTYPE)sParseTree;
}

YYSTYPE ztqMakeCmdSetTimingTree( ztqCmdParseParam * aParam,
                                 stlBool            aOn )
{
    ztqCmdSetTimingTree * sParseTree = NULL;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdSetTimingTree ),
                           (void**)&sParseTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sParseTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_SET_TIMING;
    sParseTree->mOn = aOn;

    return (YYSTYPE)sParseTree;
}

YYSTYPE ztqMakeCmdSetVerboseTree( ztqCmdParseParam * aParam,
                                  stlBool            aOn )
{
    ztqCmdSetVerboseTree * sParseTree = NULL;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdSetVerboseTree ),
                           (void**)&sParseTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sParseTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_SET_VERBOSE;
    sParseTree->mOn = aOn;

    return (YYSTYPE)sParseTree;
}

YYSTYPE ztqMakeCmdSetColorTree( ztqCmdParseParam * aParam,
                                stlBool            aOn )
{
    ztqCmdSetColorTree * sParseTree = NULL;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdSetColorTree ),
                           (void**)&sParseTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sParseTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_SET_COLOR;
    sParseTree->mOn = aOn;

    return (YYSTYPE)sParseTree;
}

YYSTYPE ztqMakeCmdSetErrorTree( ztqCmdParseParam * aParam,
                                stlBool            aOn )
{
    ztqCmdSetErrorTree * sParseTree = NULL;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdSetErrorTree ),
                           (void**)&sParseTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sParseTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_SET_ERROR;
    sParseTree->mOn = aOn;

    return (YYSTYPE)sParseTree;
}

YYSTYPE ztqMakeCmdSetLinesizeTree( ztqCmdParseParam * aParam,
                                   stlChar          * aLineSizeStr )
{
    ztqCmdSetLinesizeTree * sParseTree = NULL;
    stlInt64                sLineSize;
    stlChar               * sEndPtr;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdSetLinesizeTree ),
                           (void**)&sParseTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    if( stlStrToInt64( aLineSizeStr,
                       STL_NTS,
                       &sEndPtr,
                       10,
                       &sLineSize,
                       aParam->mErrorStack )
        != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sParseTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_SET_LINESIZE;
    sParseTree->mLineSize = sLineSize;

    return (YYSTYPE)sParseTree;
}

YYSTYPE ztqMakeCmdSetPagesizeTree( ztqCmdParseParam * aParam,
                                   stlChar          * aPageSizeStr )
{
    ztqCmdSetPagesizeTree * sParseTree = NULL;
    stlInt64                sPageSize;
    stlChar               * sEndPtr;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdSetPagesizeTree ),
                           (void**)&sParseTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    if(  stlStrToInt64( aPageSizeStr,
                        STL_NTS,
                        &sEndPtr,
                        10,
                        &sPageSize,
                        aParam->mErrorStack )
         != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sParseTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_SET_PAGESIZE;
    sParseTree->mPageSize = sPageSize;

    return (YYSTYPE)sParseTree;
}

YYSTYPE ztqMakeCmdSetColsizeTree( ztqCmdParseParam * aParam,
                                  stlChar          * aColSizeStr )
{
    ztqCmdSetColsizeTree * sParseTree = NULL;
    stlInt64               sColSize;
    stlChar              * sEndPtr;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdSetPagesizeTree ),
                           (void**)&sParseTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    if(  stlStrToInt64( aColSizeStr,
                        STL_NTS,
                        &sEndPtr,
                        10,
                        &sColSize,
                        aParam->mErrorStack )
         != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sParseTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_SET_COLSIZE;
    sParseTree->mColSize = sColSize;

    return (YYSTYPE)sParseTree;
}

YYSTYPE ztqMakeCmdSetNumsizeTree( ztqCmdParseParam * aParam,
                                  stlChar          * aNumSizeStr )
{
    ztqCmdSetNumsizeTree * sParseTree = NULL;
    stlInt64               sNumSize;
    stlChar              * sEndPtr;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdSetPagesizeTree ),
                           (void**)&sParseTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    if(  stlStrToInt64( aNumSizeStr,
                        STL_NTS,
                        &sEndPtr,
                        10,
                        &sNumSize,
                        aParam->mErrorStack )
         != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sParseTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_SET_NUMSIZE;
    sParseTree->mNumSize = sNumSize;

    return (YYSTYPE)sParseTree;
}

YYSTYPE ztqMakeCmdSetDdlsizeTree( ztqCmdParseParam * aParam,
                                  stlChar          * aDdlSizeStr )
{
    ztqCmdSetDdlsizeTree * sParseTree = NULL;
    stlInt64               sDdlSize;
    stlChar              * sEndPtr;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdSetDdlsizeTree ),
                           (void**)&sParseTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    if(  stlStrToInt64( aDdlSizeStr,
                        STL_NTS,
                        &sEndPtr,
                        10,
                        &sDdlSize,
                        aParam->mErrorStack )
         != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sParseTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_SET_DDLSIZE;
    sParseTree->mDdlSize = sDdlSize;

    return (YYSTYPE)sParseTree;
}

YYSTYPE ztqMakeCmdSetHistoryTree( ztqCmdParseParam * aParam,
                                  stlChar          * aHistorySizeStr )
{
    ztqCmdSetHistoryTree * sParseTree = NULL;
    stlInt64               sHistorySize;
    stlChar              * sEndPtr;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdSetHistoryTree ),
                           (void**)&sParseTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    if(  stlStrToInt64( aHistorySizeStr,
                        STL_NTS,
                        &sEndPtr,
                        10,
                        &sHistorySize,
                        aParam->mErrorStack )
         != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sParseTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_SET_HISTORY;
    sParseTree->mHistorySize = sHistorySize;

    return (YYSTYPE)sParseTree;
}

YYSTYPE ztqMakeCmdSetAutocommitTree( ztqCmdParseParam * aParam,
                                     stlBool            aOn )
{
    ztqCmdSetAutocommitTree * sParseTree = NULL;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdSetAutocommitTree ),
                           (void**)&sParseTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sParseTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_SET_AUTOCOMMIT;
    sParseTree->mOn = aOn;

    return (YYSTYPE)sParseTree;
}

YYSTYPE ztqMakeCmdSetResultTree( ztqCmdParseParam * aParam,
                                 stlBool            aOn )
{
    ztqCmdSetResultTree * sParseTree = NULL;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdSetResultTree ),
                           (void**)&sParseTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sParseTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_SET_RESULT;
    sParseTree->mOn = aOn;

    return (YYSTYPE)sParseTree;
}

YYSTYPE ztqMakeCmdQuitTree( ztqCmdParseParam * aParam )
{
    ztqParseTree * sQuitTree = NULL;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqParseTree ),
                           (void**)&sQuitTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sQuitTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_QUIT;

    return (YYSTYPE)sQuitTree;
}

YYSTYPE ztqMakeCmdHelpTree( ztqCmdParseParam * aParam )
{
    ztqParseTree * sHelpTree = NULL;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqParseTree ),
                           (void**)&sHelpTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sHelpTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_HELP;

    return (YYSTYPE)sHelpTree;
}

YYSTYPE ztqMakeCmdImportTree( ztqCmdParseParam * aParam,
                              stlChar          * aImportFile )
{
    ztqCmdImportTree * sParseTree = NULL;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdImportTree ),
                           (void**)&sParseTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sParseTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_IMPORT;
    stlStrncpy( sParseTree->mImportFile,
                (const stlChar*)aImportFile,
                STL_MAX_FILE_PATH_LENGTH );

    return (YYSTYPE)sParseTree;
}

YYSTYPE ztqMakeCmdPrintHistoryTree( ztqCmdParseParam * aParam )
{
    ztqParseTree * sHelpTree = NULL;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqParseTree ),
                           (void**)&sHelpTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sHelpTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_PRINT_HISTORY;

    return (YYSTYPE)sHelpTree;
}

YYSTYPE ztqMakeCmdExecHistoryTree( ztqCmdParseParam * aParam,
                                   stlChar          * aHistoryNumStr )
{
    ztqCmdExecHistoryTree * sParseTree = NULL;
    stlInt64                sHistoryNum;
    stlChar               * sEndPtr;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdExecHistoryTree ),
                           (void**)&sParseTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    if( aHistoryNumStr != NULL )
    {
        if( stlStrToInt64( aHistoryNumStr,
                           STL_NTS,
                           &sEndPtr,
                           10,
                           &sHistoryNum,
                           aParam->mErrorStack )
            != STL_SUCCESS )
        {
            aParam->mErrStatus = STL_FAILURE;
            return (YYSTYPE)NULL;
        }
    }
    else
    {
        sHistoryNum = 0;
    }

    sParseTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_EXEC_HISTORY;
    sParseTree->mHistoryNum = sHistoryNum;

    return (YYSTYPE)sParseTree;
}

YYSTYPE ztqMakeCmdDescTree( ztqCmdParseParam * aParam,
                            ztqIdentifier    * aIdentifierList )
{
    ztqCmdDescTree * sParseTree = NULL;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdDescTree ),
                           (void**)&sParseTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sParseTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_DESC;
    sParseTree->mIdentifierList = aIdentifierList;

    return (YYSTYPE)sParseTree;
}

YYSTYPE ztqMakeCmdIdescTree( ztqCmdParseParam * aParam,
                             ztqIdentifier    * aIdentifierList )
{
    ztqCmdIdescTree * sParseTree = NULL;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdIdescTree ),
                           (void**)&sParseTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sParseTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_IDESC;
    sParseTree->mIdentifierList = aIdentifierList;

    return (YYSTYPE)sParseTree;
}

YYSTYPE ztqMakeCharType( ztqCmdParseParam  * aParam,
                         stlInt32            aDataType,
                         dtlStringLengthUnit aStringLengthUnit,
                         YYSTYPE             aNumStr )
{
    ztqDataType * sDataType;
    stlChar     * sEndPtr;
    stlInt64      sPrecision;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqDataType ),
                           (void**)&sDataType,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    if( aNumStr != NULL )
    {
        if( stlStrToInt64( (const stlChar*)aNumStr,
                           STL_NTS,
                           &sEndPtr,
                           10,
                           &sPrecision,
                           aParam->mErrorStack )
            != STL_SUCCESS )
        {
            aParam->mErrStatus = STL_FAILURE;
            return (YYSTYPE)NULL;
        }
    }
    else
    {
        if( (aDataType == SQL_LONGVARCHAR) ||
            (aDataType == SQL_LONGVARBINARY) )
        {
            sPrecision = STL_INT32_MAX;
        }
        else
        {
            sPrecision = 1;
        }
    }  

    switch( aDataType )
    {
        case SQL_CHAR:
            {
                if( (sPrecision >= gDataTypeDefinition[DTL_TYPE_CHAR].mMinStringLength ) &&
                    (sPrecision <= gDataTypeDefinition[DTL_TYPE_CHAR].mMaxStringLength ) )
                {
                    /* Do Nothing */
                }
                else
                {
                    aParam->mErrStatus = STL_FAILURE;
                    return (YYSTYPE)NULL;
                }
            }
            break;
        case SQL_VARCHAR:
            {
                if( (sPrecision >= gDataTypeDefinition[DTL_TYPE_VARCHAR].mMinStringLength ) &&
                    (sPrecision <= gDataTypeDefinition[DTL_TYPE_VARCHAR].mMaxStringLength ) )
                {
                    /* Do Nothing */
                }
                else
                {
                    aParam->mErrStatus = STL_FAILURE;
                    return (YYSTYPE)NULL;
                }
            }
            break;
        case SQL_BINARY:
            {
                if( (sPrecision >= gDataTypeDefinition[DTL_TYPE_BINARY].mMinStringLength ) &&
                    (sPrecision <= gDataTypeDefinition[DTL_TYPE_BINARY].mMaxStringLength ) )
                {
                    /* Do Nothing */
                }
                else
                {
                    aParam->mErrStatus = STL_FAILURE;
                    return (YYSTYPE)NULL;
                }
            }
            break;
        case SQL_VARBINARY:
            {
                if( (sPrecision >= gDataTypeDefinition[DTL_TYPE_VARBINARY].mMinStringLength ) &&
                    (sPrecision <= gDataTypeDefinition[DTL_TYPE_VARBINARY].mMaxStringLength ) )
                {
                    /* Do Nothing */
                }
                else
                {
                    aParam->mErrStatus = STL_FAILURE;
                    return (YYSTYPE)NULL;
                }
            }
            break;
        case SQL_LONGVARCHAR:
        case SQL_LONGVARBINARY:
            {
                /* do nothing */
            }
            break;
        default:
            STL_DASSERT(0);
            break;
    }
    
    sDataType->mPrecision = sPrecision;
    sDataType->mDataType = aDataType;
    sDataType->mScale = 0;
    sDataType->mStringLengthUnit = aStringLengthUnit;
    
    return (YYSTYPE)sDataType; 
}

YYSTYPE ztqMakeNumberType( ztqCmdParseParam * aParam,
                           stlInt32           aDataType,
                           stlInt32           aPrecision,
                           stlInt32           aScale )
{
    ztqDataType * sDataType;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqDataType ),
                           (void**)&sDataType,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sDataType->mDataType = aDataType;
    sDataType->mScale = aScale;
    sDataType->mPrecision = aPrecision;
    sDataType->mStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;

    return (YYSTYPE)sDataType; 
}

YYSTYPE ztqMakeNumericType( ztqCmdParseParam * aParam,
                            stlInt32           aDataType,
                            YYSTYPE            aPrecisionStr,
                            YYSTYPE            aScaleStr )
{
    ztqDataType * sDataType;
    stlChar     * sEndPtr;
    stlInt64      sPrecision;
    stlInt64      sScale;

    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqDataType ),
                           (void**)&sDataType,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    if( aPrecisionStr != NULL )
    {
        if( stlStrToInt64( (const stlChar*)aPrecisionStr,
                           STL_NTS,
                           &sEndPtr,
                           10,
                           &sPrecision,
                           aParam->mErrorStack )
            != STL_SUCCESS )
        {
            aParam->mErrStatus = STL_FAILURE;
            return (YYSTYPE)NULL;
        }
    }
    else
    {
        sPrecision = DTL_NUMERIC_DEFAULT_PRECISION;
    }

    if( aDataType == SQL_FLOAT )
    {
        sScale = 0;
    }
    else
    {
        if( aScaleStr != NULL )
        {
            if( stlStrToInt64( (const stlChar*)aScaleStr,
                               STL_NTS,
                               &sEndPtr,
                               10,
                               &sScale,
                               aParam->mErrorStack )
                != STL_SUCCESS )
            {
                aParam->mErrStatus = STL_FAILURE;
                return (YYSTYPE)NULL;
            }
        }
        else
        {
            sScale = 0;
        }
    }

    if( aDataType == SQL_FLOAT )
    {
        if( (sPrecision >= DTL_FLOAT_MIN_PRECISION) &&
            (sPrecision <= DTL_FLOAT_MAX_PRECISION) )
        {
            /* Do Nothing */
        }
        else
        {
            aParam->mErrStatus = STL_FAILURE;
            return (YYSTYPE)NULL;
        }
    }
    else
    {
        if( ( (sPrecision >= DTL_NUMERIC_MIN_PRECISION) &&
              (sPrecision <= DTL_NUMERIC_MAX_PRECISION) )
            &&
            ( (sScale >= DTL_NUMERIC_MIN_SCALE) &&
              (sScale <= DTL_NUMERIC_MAX_SCALE) ) )
        {
            /* Do Nothing */            
        }
        else
        {
            aParam->mErrStatus = STL_FAILURE;
            return (YYSTYPE)NULL;            
        }
    }
    
    sDataType->mDataType = aDataType;
    sDataType->mScale = (stlInt32)sScale;
    sDataType->mPrecision = (stlInt32)sPrecision;
    sDataType->mStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;

    return (YYSTYPE)sDataType; 
}

YYSTYPE ztqMakeDateType( ztqCmdParseParam * aParam,
                         stlInt32           aDataType,
                         stlInt32           aPrecision,
                         stlInt32           aScale )
{
    ztqDataType * sDataType;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqDataType ),
                           (void**)&sDataType,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sDataType->mDataType = aDataType;
    sDataType->mScale = aScale;
    sDataType->mPrecision = aPrecision;
    sDataType->mStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;

    return (YYSTYPE)sDataType; 
}

YYSTYPE ztqMakeRowIdType( ztqCmdParseParam * aParam,
                          stlInt32           aDataType,
                          stlInt32           aPrecision,
                          stlInt32           aScale )
{
    ztqDataType * sDataType;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqDataType ),
                           (void**)&sDataType,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }
    
    sDataType->mDataType = aDataType;
    sDataType->mScale = aScale;
    sDataType->mPrecision = aPrecision;
    sDataType->mStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
    
    return (YYSTYPE)sDataType; 
}

YYSTYPE ztqMakeTemporalType( ztqCmdParseParam * aParam,
                             stlInt32           aDataType,
                             YYSTYPE            aPrecisionStr,
                             stlInt32           aScale )
{
    ztqDataType * sDataType;
    stlInt64      sPrecision;
    stlChar     * sEndPtr;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqDataType ),
                           (void**)&sDataType,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    if( stlStrToInt64( (const stlChar*)aPrecisionStr,
                       STL_NTS,
                       &sEndPtr,
                       10,
                       &sPrecision,
                       aParam->mErrorStack )
         != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sDataType->mDataType = aDataType;
    sDataType->mScale = aScale;
    sDataType->mStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
    
    if( ( sPrecision >= DTL_TIME_MIN_PRECISION ) &&
        ( sPrecision <= DTL_TIME_MAX_PRECISION ) )
    {
        sDataType->mPrecision = (stlInt32)sPrecision;
    }
    else
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    return (YYSTYPE)sDataType; 
}

YYSTYPE ztqMakeIntervalType( ztqCmdParseParam * aParam,
                             stlInt32           aDataType,
                             YYSTYPE            aPrecisionStr,
                             stlInt32           aScale )
{
    ztqDataType * sDataType;
    stlInt64      sPrecision;
    stlChar     * sEndPtr;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqDataType ),
                           (void**)&sDataType,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }
    
    if( stlStrToInt64( (const stlChar*)aPrecisionStr,
                       STL_NTS,
                       &sEndPtr,
                       10,
                       &sPrecision,
                       aParam->mErrorStack )
        != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }
    
    sDataType->mDataType = aDataType;
    sDataType->mStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
    
    if( (sPrecision >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) &&
        (sPrecision <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION) )
    {
        sDataType->mPrecision = (stlInt32)sPrecision;
    }
    else
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }
    
    switch( aDataType )
    {
        case SQL_INTERVAL_YEAR :
        case SQL_INTERVAL_MONTH :
        case SQL_INTERVAL_DAY :
        case SQL_INTERVAL_HOUR :
        case SQL_INTERVAL_MINUTE :
        case SQL_INTERVAL_YEAR_TO_MONTH :
        case SQL_INTERVAL_DAY_TO_HOUR :
        case SQL_INTERVAL_DAY_TO_MINUTE :
        case SQL_INTERVAL_HOUR_TO_MINUTE :            
            sDataType->mScale = DTL_SCALE_NA;
            break;
        case SQL_INTERVAL_SECOND :                
        case SQL_INTERVAL_DAY_TO_SECOND :
        case SQL_INTERVAL_HOUR_TO_SECOND :
        case SQL_INTERVAL_MINUTE_TO_SECOND :
            if( (aScale >= DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION) &&
                (aScale <= DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION) )
            {
                sDataType->mScale = aScale;
            }
            else
            {
                aParam->mErrStatus = STL_FAILURE;
                return (YYSTYPE)NULL;
            }
            break;
        default :
            STL_DASSERT(0);
            break;        
    }
    
    return (YYSTYPE)sDataType; 
}

YYSTYPE ztqMakeIntervalSecondType( ztqCmdParseParam * aParam,
                                   stlInt32           aDataType,
                                   YYSTYPE            aPrecisionStr,
                                   YYSTYPE            aScaleStr )
{
    ztqDataType * sDataType;
    stlInt64      sPrecision;
    stlInt64      sScale;
    stlChar     * sEndPtr;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqDataType ),
                           (void**)&sDataType,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    if( stlStrToInt64( (const stlChar*)aPrecisionStr,
                       STL_NTS,
                       &sEndPtr,
                       10,
                       &sPrecision,
                       aParam->mErrorStack )
         != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    if( stlStrToInt64( (const stlChar*)aScaleStr,
                       STL_NTS,
                       &sEndPtr,
                       10,
                       &sScale,
                       aParam->mErrorStack )
         != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sDataType->mDataType = aDataType;
    sDataType->mStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;

    if( (sPrecision >= DTL_INTERVAL_LEADING_FIELD_MIN_PRECISION) &&
        (sPrecision <= DTL_INTERVAL_LEADING_FIELD_MAX_PRECISION) )
    {
        sDataType->mPrecision = (stlInt32)sPrecision;
    }
    else
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    if( (sScale >= DTL_INTERVAL_FRACTIONAL_SECOND_MIN_PRECISION) &&
        (sScale <= DTL_INTERVAL_FRACTIONAL_SECOND_MAX_PRECISION) )
    {
        sDataType->mScale = (stlInt32)sScale;
    }
    else
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }   

    return (YYSTYPE)sDataType; 
}

YYSTYPE ztqMakeIdentifier( ztqCmdParseParam * aParam,
                           YYSTYPE            aIdentifierStr )
{
    ztqIdentifier * sIdentifier;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqIdentifier ),
                           (void**)&sIdentifier,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           stlStrlen( (const stlChar*)aIdentifierStr ) + 1,
                           (void**)&sIdentifier->mIdentifier,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    if( *(stlChar*)aIdentifierStr != '"' )
    {
        stlToupperString( (stlChar*)aIdentifierStr, sIdentifier->mIdentifier );
    }
    else
    {
        stlStrcpy( sIdentifier->mIdentifier, (stlChar*)aIdentifierStr );
    }

    sIdentifier->mNext = NULL;

    return (YYSTYPE)sIdentifier; 
}


YYSTYPE ztqAddIdentifier( ztqCmdParseParam * aParam,
                          ztqIdentifier    * aIdentifierList,
                          ztqIdentifier    * aIdentifier )
{
    ztqIdentifier * sIdentifier;

    STL_ASSERT( aIdentifierList != NULL );
    
    sIdentifier = aIdentifierList;

    while( sIdentifier->mNext != NULL )
    {
        sIdentifier = (ztqIdentifier*)(sIdentifier->mNext);
    }

    sIdentifier->mNext = aIdentifier;
    
    return (YYSTYPE)aIdentifierList; 
}


YYSTYPE ztqMakeXaFlag( ztqCmdParseParam * aParam,
                       stlInt64           aXaFlag )
{
    ztqXaFlag * sXaFlag;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqXaFlag ),
                           (void**)&sXaFlag,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sXaFlag->mFlag = aXaFlag;
    sXaFlag->mNext = NULL;

    return (YYSTYPE)sXaFlag; 
}


YYSTYPE ztqAddXaFlag( ztqCmdParseParam * aParam,
                      ztqXaFlag        * aXaFlagList,
                      ztqXaFlag        * aXaFlag )
{
    ztqXaFlag * sXaFlag;

    STL_ASSERT( aXaFlagList != NULL );
    
    sXaFlag = aXaFlagList;

    while( sXaFlag->mNext != NULL )
    {
        sXaFlag = (ztqXaFlag*)(sXaFlag->mNext);
    }

    sXaFlag->mNext = aXaFlag;
    
    return (YYSTYPE)aXaFlagList; 
}


YYSTYPE ztqXaXidFromNumber( ztqCmdParseParam * aParam,
                            YYSTYPE            aXid )
{
    XID * sXid;

    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( XID ),
                           (void**)&sXid,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }
    
    stlMemset( sXid->data, 0x00, STL_SIZEOF(XID) );
    
    sXid->formatID = 0;
    sXid->bqual_length = 1;
    sXid->gtrid_length = stlStrlen( aXid );
    
    stlStrncpy( sXid->data, aXid, MAXGTRIDSIZE );
    
    return (YYSTYPE)sXid; 
}


YYSTYPE ztqXaXidFromString( ztqCmdParseParam * aParam,
                            YYSTYPE            aXid )
{
    XID * sXid;

    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( XID ),
                           (void**)&sXid,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }
    
    if( dtlStringToXid( aXid,
                        (stlXid*)sXid,
                        aParam->mErrorStack )
        != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }
    
    return (YYSTYPE)sXid; 
}


YYSTYPE ztqMakeCmdExplainPlanTree( ztqCmdParseParam * aParam,
                                   stlUInt16          aExplainPlanType )
{
    ztqCmdExplainPlanTree * sExplainPlanTree = NULL;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdExplainPlanTree ),
                           (void**)&sExplainPlanTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sExplainPlanTree->mCommandType     = ZTQ_COMMAND_TYPE_CMD_EXPLAIN_PLAN;
    sExplainPlanTree->mExplainPlanType = aExplainPlanType;
    sExplainPlanTree->mSqlStartPos     = aParam->mSqlPosition;

    return (YYSTYPE)sExplainPlanTree;
}


YYSTYPE ztqMakeCmdAllocStmtTree( ztqCmdParseParam * aParam,
                                 YYSTYPE            aIdentifier )
{
    ztqCmdAllocStmtTree * sAllocStmtTree = NULL;

    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdAllocStmtTree ),
                           (void**)&sAllocStmtTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sAllocStmtTree->mCommandType   = ZTQ_COMMAND_TYPE_CMD_ALLOC_STMT;

    stlStrncpy( sAllocStmtTree->mStmtName,
                (const stlChar*)aIdentifier,
                STL_MAX_SQL_IDENTIFIER_LENGTH );
    
    return (YYSTYPE)sAllocStmtTree;
}


YYSTYPE ztqMakeCmdFreeStmtTree( ztqCmdParseParam * aParam,
                                YYSTYPE            aIdentifier )
{
    ztqCmdFreeStmtTree * sFreeStmtTree = NULL;

    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdFreeStmtTree ),
                           (void**)&sFreeStmtTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sFreeStmtTree->mCommandType   = ZTQ_COMMAND_TYPE_CMD_FREE_STMT;

    stlStrncpy( sFreeStmtTree->mStmtName,
                (const stlChar*)aIdentifier,
                STL_MAX_SQL_IDENTIFIER_LENGTH );
    
    return (YYSTYPE)sFreeStmtTree;
}


YYSTYPE ztqMakeCmdUseStmtExecSqlTree( ztqCmdParseParam * aParam,
                                      YYSTYPE            aIdentifier )
{
    ztqCmdUseStmtExecSqlTree * sUseStmtExecSqlTree = NULL;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdUseStmtExecSqlTree ),
                           (void**)&sUseStmtExecSqlTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sUseStmtExecSqlTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_USE_STMT_EXEC_SQL;

    stlStrncpy( sUseStmtExecSqlTree->mStmtName,
                (const stlChar*)aIdentifier,
                STL_MAX_SQL_IDENTIFIER_LENGTH );
    
    sUseStmtExecSqlTree->mSqlStartPos = aParam->mSqlPosition;

    return (YYSTYPE)sUseStmtExecSqlTree;
}

YYSTYPE ztqMakeCmdUseStmtPrepareSqlTree( ztqCmdParseParam * aParam,
                                         YYSTYPE            aIdentifier )
{
    ztqCmdUseStmtPrepareSqlTree * sUseStmtPrepareSqlTree = NULL;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdUseStmtPrepareSqlTree ),
                           (void**)&sUseStmtPrepareSqlTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sUseStmtPrepareSqlTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_USE_STMT_PREPARE_SQL;

    stlStrncpy( sUseStmtPrepareSqlTree->mStmtName,
                (const stlChar*)aIdentifier,
                STL_MAX_SQL_IDENTIFIER_LENGTH );
    
    sUseStmtPrepareSqlTree->mSqlStartPos = aParam->mSqlPosition;

    return (YYSTYPE)sUseStmtPrepareSqlTree;
}


YYSTYPE ztqMakeCmdUseStmtExecPreparedTree( ztqCmdParseParam * aParam,
                                           YYSTYPE            aIdentifier )
{
    ztqCmdUseStmtExecPreparedTree * sUseStmtExecPreparedTree = NULL;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdUseStmtExecPreparedTree ),
                           (void**)&sUseStmtExecPreparedTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sUseStmtExecPreparedTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_USE_STMT_EXEC_PREPARED;

    stlStrncpy( sUseStmtExecPreparedTree->mStmtName,
                (const stlChar*)aIdentifier,
                STL_MAX_SQL_IDENTIFIER_LENGTH );
    
    return (YYSTYPE)sUseStmtExecPreparedTree;
}

YYSTYPE ztqMakeCmdStartupTree( ztqCmdParseParam * aParam,
                               ztqStartupPhase    aStartupPhase )
{
    ztqCmdStartupTree * sStartupTree = NULL;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdStartupTree ),
                           (void**)&sStartupTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sStartupTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_STARTUP;
    sStartupTree->mStartupPhase = aStartupPhase;
    
    return (YYSTYPE)sStartupTree;
}

YYSTYPE ztqMakeCmdShutdownTree( ztqCmdParseParam * aParam,
                                ztqShutdownMode    aShutdownMode )
{
    ztqCmdShutdownTree * sShutdownTree = NULL;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdShutdownTree ),
                           (void**)&sShutdownTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sShutdownTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_SHUTDOWN;
    sShutdownTree->mShutdownMode = aShutdownMode;
    
    return (YYSTYPE)sShutdownTree;
}

YYSTYPE ztqMakeCmdConnectTree( ztqCmdParseParam * aParam,
                               YYSTYPE            aUserId,
                               YYSTYPE            aPassword,
                               YYSTYPE            aNewPassword,
                               ztqConnectAsUser   aAsUser )
{
    ztqCmdConnectTree * sConnectTree = NULL;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdConnectTree ),
                           (void**)&sConnectTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sConnectTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_CONNECT;

    if( aUserId == NULL )
    {
        sConnectTree->mUserId[0] = '\0';
    }
    else
    {
        stlStrncpy( sConnectTree->mUserId,
                    (const stlChar*)aUserId,
                    STL_MAX_SQL_IDENTIFIER_LENGTH );
    }
    
    if( aPassword == NULL )
    {
        sConnectTree->mPassword[0] = '\0';
    }
    else
    {
        stlStrncpy( sConnectTree->mPassword,
                    (const stlChar*)aPassword,
                    STL_MAX_SQL_IDENTIFIER_LENGTH );
    }

    if( aNewPassword == NULL )
    {
        sConnectTree->mNewPassword[0] = '\0';
    }
    else
    {
        stlStrncpy( sConnectTree->mNewPassword,
                    (const stlChar*)aNewPassword,
                    STL_MAX_SQL_IDENTIFIER_LENGTH );
    }
    
    sConnectTree->mAsUser = aAsUser;
    
    return (YYSTYPE)sConnectTree;
}



YYSTYPE ztqMakeCmdXaOpenTree( ztqCmdParseParam * aParam,
                              YYSTYPE            aXaInfo,
                              ztqXaFlag        * aXaFlags )
{
    ztqCmdXaOpenTree * sXaOpenTree;
    ztqXaFlag        * sXaFlag;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdXaOpenTree ),
                           (void**)&sXaOpenTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sXaOpenTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_XA_OPEN;
    sXaOpenTree->mFlags = TMNOFLAGS;
    
    stlStrncpy( sXaOpenTree->mXaInfo, aXaInfo, ZTQ_MAX_XAINFO_SIZE );

    sXaFlag = aXaFlags;
    
    while( sXaFlag != NULL )
    {
        sXaOpenTree->mFlags |= sXaFlag->mFlag;
        sXaFlag = sXaFlag->mNext;
    }
    
    return (YYSTYPE)sXaOpenTree;
}

YYSTYPE ztqMakeCmdXaCloseTree( ztqCmdParseParam * aParam,
                               YYSTYPE            aXaInfo,
                               ztqXaFlag        * aXaFlags )
{
    ztqCmdXaCloseTree * sXaCloseTree;
    ztqXaFlag         * sXaFlag;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdXaCloseTree ),
                           (void**)&sXaCloseTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sXaCloseTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_XA_CLOSE;
    sXaCloseTree->mFlags = TMNOFLAGS;
    
    stlStrncpy( sXaCloseTree->mXaInfo, aXaInfo, ZTQ_MAX_XAINFO_SIZE );

    sXaFlag = aXaFlags;
    
    while( sXaFlag != NULL )
    {
        sXaCloseTree->mFlags |= sXaFlag->mFlag;
        sXaFlag = sXaFlag->mNext;
    }
    
    return (YYSTYPE)sXaCloseTree;
}

YYSTYPE ztqMakeCmdXaStartTree( ztqCmdParseParam * aParam,
                               YYSTYPE            aXid,
                               ztqXaFlag        * aXaFlags )
{
    ztqCmdXaStartTree * sXaStartTree;
    ztqXaFlag         * sXaFlag;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdXaStartTree ),
                           (void**)&sXaStartTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sXaStartTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_XA_START;
    sXaStartTree->mFlags = TMNOFLAGS;
    stlMemcpy( &sXaStartTree->mXid, aXid, STL_SIZEOF(XID) );
    
    sXaFlag = aXaFlags;
    
    while( sXaFlag != NULL )
    {
        sXaStartTree->mFlags |= sXaFlag->mFlag;
        sXaFlag = sXaFlag->mNext;
    }
    
    return (YYSTYPE)sXaStartTree;
}

YYSTYPE ztqMakeCmdXaEndTree( ztqCmdParseParam * aParam,
                             YYSTYPE            aXid,
                             ztqXaFlag        * aXaFlags )
{
    ztqCmdXaEndTree * sXaEndTree;
    ztqXaFlag       * sXaFlag;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdXaEndTree ),
                           (void**)&sXaEndTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sXaEndTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_XA_END;
    sXaEndTree->mFlags = TMNOFLAGS;
    stlMemcpy( &sXaEndTree->mXid, aXid, STL_SIZEOF(XID) );
    
    sXaFlag = aXaFlags;
    
    while( sXaFlag != NULL )
    {
        sXaEndTree->mFlags |= sXaFlag->mFlag;
        sXaFlag = sXaFlag->mNext;
    }
    
    return (YYSTYPE)sXaEndTree;
}

YYSTYPE ztqMakeCmdXaCommitTree( ztqCmdParseParam * aParam,
                                YYSTYPE            aXid,
                                ztqXaFlag        * aXaFlags )
{
    ztqCmdXaCommitTree * sXaCommitTree;
    ztqXaFlag          * sXaFlag;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdXaCommitTree ),
                           (void**)&sXaCommitTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sXaCommitTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_XA_COMMIT;
    sXaCommitTree->mFlags = TMNOFLAGS;
    stlMemcpy( &sXaCommitTree->mXid, aXid, STL_SIZEOF(XID) );
    
    sXaFlag = aXaFlags;
    
    while( sXaFlag != NULL )
    {
        sXaCommitTree->mFlags |= sXaFlag->mFlag;
        sXaFlag = sXaFlag->mNext;
    }
    
    return (YYSTYPE)sXaCommitTree;
}

YYSTYPE ztqMakeCmdXaRollbackTree( ztqCmdParseParam * aParam,
                                  YYSTYPE            aXid,
                                  ztqXaFlag        * aXaFlags )
{
    ztqCmdXaRollbackTree * sXaRollbackTree;
    ztqXaFlag            * sXaFlag;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdXaRollbackTree ),
                           (void**)&sXaRollbackTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sXaRollbackTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_XA_ROLLBACK;
    sXaRollbackTree->mFlags = TMNOFLAGS;
    stlMemcpy( &sXaRollbackTree->mXid, aXid, STL_SIZEOF(XID) );
    
    sXaFlag = aXaFlags;
    
    while( sXaFlag != NULL )
    {
        sXaRollbackTree->mFlags |= sXaFlag->mFlag;
        sXaFlag = sXaFlag->mNext;
    }
    
    return (YYSTYPE)sXaRollbackTree;
}

YYSTYPE ztqMakeCmdXaPrepareTree( ztqCmdParseParam * aParam,
                                 YYSTYPE            aXid,
                                 ztqXaFlag        * aXaFlags )
{
    ztqCmdXaPrepareTree * sXaPrepareTree;
    ztqXaFlag           * sXaFlag;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdXaPrepareTree ),
                           (void**)&sXaPrepareTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sXaPrepareTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_XA_PREPARE;
    sXaPrepareTree->mFlags = TMNOFLAGS;
    stlMemcpy( &sXaPrepareTree->mXid, aXid, STL_SIZEOF(XID) );
    
    sXaFlag = aXaFlags;
    
    while( sXaFlag != NULL )
    {
        sXaPrepareTree->mFlags |= sXaFlag->mFlag;
        sXaFlag = sXaFlag->mNext;
    }
    
    return (YYSTYPE)sXaPrepareTree;
}

YYSTYPE ztqMakeCmdXaForgetTree( ztqCmdParseParam * aParam,
                                YYSTYPE            aXid,
                                ztqXaFlag        * aXaFlags )
{
    ztqCmdXaForgetTree * sXaForgetTree;
    ztqXaFlag          * sXaFlag;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdXaForgetTree ),
                           (void**)&sXaForgetTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sXaForgetTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_XA_FORGET;
    sXaForgetTree->mFlags = TMNOFLAGS;
    stlMemcpy( &sXaForgetTree->mXid, aXid, STL_SIZEOF(XID) );
    
    sXaFlag = aXaFlags;
    
    while( sXaFlag != NULL )
    {
        sXaForgetTree->mFlags |= sXaFlag->mFlag;
        sXaFlag = sXaFlag->mNext;
    }
    
    return (YYSTYPE)sXaForgetTree;
}

YYSTYPE ztqMakeCmdXaRecoverTree( ztqCmdParseParam * aParam,
                                 YYSTYPE            aCountStr,
                                 ztqXaFlag        * aXaFlags )
{
    ztqCmdXaRecoverTree * sXaRecoverTree;
    ztqXaFlag           * sXaFlag;
    stlChar             * sEndPtr;
    stlInt64              sCount;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdXaRecoverTree ),
                           (void**)&sXaRecoverTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    if(  stlStrToInt64( aCountStr,
                        STL_NTS,
                        &sEndPtr,
                        10,
                        &sCount,
                        aParam->mErrorStack )
         != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( XID ) * sCount,
                           (void**)&sXaRecoverTree->mXids,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }
    
    sXaRecoverTree->mCount = sCount;
    sXaRecoverTree->mCommandType = ZTQ_COMMAND_TYPE_CMD_XA_RECOVER;
    sXaRecoverTree->mFlags = TMNOFLAGS;
    
    sXaFlag = aXaFlags;
    
    while( sXaFlag != NULL )
    {
        sXaRecoverTree->mFlags |= sXaFlag->mFlag;
        sXaFlag = sXaFlag->mNext;
    }
    
    return (YYSTYPE)sXaRecoverTree;
}


YYSTYPE ztqMakeCmdDDLTree( ztqCmdParseParam * aParam,
                           ztqCommandType     aCommandType,
                           ztqIdentifier    * aIdentifierList,
                           ztqPrintDDLType    aDDLType )
{
    ztqCmdDDLTree * sDDLTree = NULL;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           STL_SIZEOF( ztqCmdDDLTree ),
                           (void**) & sDDLTree,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sDDLTree->mCommandType    = aCommandType;
    sDDLTree->mIdentifierList = aIdentifierList;
    sDDLTree->mDDLType        = aDDLType;
    
    return (YYSTYPE) sDDLTree;
}



/**
 * @note 이름을 변경할수 없다.
 */
stlInt32 ztqCmderror( ztqCmdParseParam * aParam,
                      void             * aScanner,
                      const stlChar    * aMsg )
{
    stlInt32   i;
    stlInt32   sCurLineNo = 1;
    stlChar  * sStartPtr = aParam->mBuffer;
    stlChar  * sEndPtr;
    stlChar  * sErrMsg;
    stlChar    sErrLineMsgBuffer[128];
    stlInt32   sErrMsgLength;

    /**
     * 에러 메세지의 총길이는 (원본 SQL의 길이 + 에러 위치 메세지 길이 +
     * 에러 라인 메세지 길이)의 총합과 같다.
     */
    sErrMsgLength = ((stlStrlen( sStartPtr ) + 1) * 2 ) + 25;

    STL_TRY( stlAllocRegionMem( aParam->mAllocator,
                                sErrMsgLength,
                                (void**)&sErrMsg,
                                aParam->mErrorStack )
             == STL_SUCCESS );

    sErrMsg[0] = '\0';
    
    while( sCurLineNo < aParam->mLineNo )
    {
        if( *sStartPtr == '\n' )
        {
            sCurLineNo++;
        }
        sStartPtr++;
    }
    sEndPtr = sStartPtr;
    while( (*sEndPtr != '\n') && (*sEndPtr != '\0') )
    {
        sEndPtr++;
    }

    /**
     * 원본 SQL 저장
     */
    stlStrnncat( sErrMsg, "\n", sErrMsgLength, STL_UINT32_MAX );
    stlStrnncat( sErrMsg, sStartPtr, sErrMsgLength, sEndPtr - sStartPtr + 1 );
    stlStrnncat( sErrMsg, "\n", sErrMsgLength, STL_UINT32_MAX );

    /**
     * 에러 위치 메세지 저장
     */
    for( i = 1; i < aParam->mColumn; i++ )
    {
        stlStrnncat( sErrMsg, ".", sErrMsgLength, STL_UINT32_MAX );
    }
    stlStrnncat( sErrMsg, "^", sErrMsgLength, STL_UINT32_MAX );
    for( i = 0; i < aParam->mColumnLen - 2; i++ )
    {
        stlStrnncat( sErrMsg, " ", sErrMsgLength, STL_UINT32_MAX );
    }
    if( aParam->mColumnLen >= 2 )
    {
        stlStrnncat( sErrMsg, "^", sErrMsgLength, STL_UINT32_MAX );
    }
    stlStrnncat( sErrMsg, "\n", sErrMsgLength, STL_UINT32_MAX );

    /**
     * 에러 라인 메세지 저장
     */
    stlSnprintf( sErrLineMsgBuffer,
                 128,
                 "Error at line %d",
                 aParam->mLineNo );
    stlStrnncat( sErrMsg, sErrLineMsgBuffer, sErrMsgLength, STL_UINT32_MAX );
    
    aParam->mErrStatus = STL_FAILURE;
    aParam->mErrMsg = sErrMsg;

    return 0;

    STL_FINISH;

    return 0;
}

stlStatus ztqCmdParseIt( stlChar        * aCmd,
                         stlAllocator   * aAllocator,
                         ztqParseTree  ** aParseTree,
                         stlErrorStack  * aErrorStack )
{
    ztqCmdParseParam  sParam;

    sParam.mBuffer             = aCmd;
    sParam.mAllocator          = aAllocator;
    sParam.mErrorStack         = aErrorStack;
    sParam.mLength             = stlStrlen( aCmd );
    sParam.mPosition           = 0;
    sParam.mLineNo             = 1;
    sParam.mColumn             = 1;
    sParam.mColumnLen          = 0;
    sParam.mCurPosition        = 0;
    sParam.mParseTree          = NULL;
    sParam.mErrMsg             = NULL;
    sParam.mErrStatus          = STL_SUCCESS;
    sParam.mSqlPosition        = 0;
    
    ztqCmdlex_init( &sParam.mYyScanner );
    ztqCmdset_extra( &sParam, sParam.mYyScanner );
    ztqCmdparse( &sParam, sParam.mYyScanner );
    ztqCmdlex_destroy( sParam.mYyScanner );

    *aParseTree = sParam.mParseTree;

    STL_TRY_THROW( sParam.mErrStatus == STL_SUCCESS, RAMP_ERR_SYNTAX );
    STL_TRY_THROW( sParam.mParseTree != NULL, RAMP_ERR_SYNTAX );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SYNTAX )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_SYNTAX,
                      sParam.mErrMsg,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
} 

/** @} */

