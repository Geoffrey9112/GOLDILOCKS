/*******************************************************************************
 * ztpSqlParser.c
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
#include <ztpDef.h>
#include <ztpMisc.h>
#include <ztpSqlLex.h>
#include <ztpSqlParser.h>
#include <ztpSqlMacro.h>
#include <ztpcHostVar.h>
#include <ztpuString.h>
#include <ztpvCVarFunc.h>

/**
 * @file ztpSqlParser.c
 * @brief SQL Parser Routines
 */

/**
 * @addtogroup ztpSqlParser
 * @{
 */

YYSTYPE ztpMakeIgnoreStmt( stlParseParam     *aParam )
{
    ztpParseTree * sParseTree = NULL;

    STL_TRY( ztpSqlAllocator( aParam->mContext,
                              STL_SIZEOF( ztpParseTree ),
                              (void**)&sParseTree,
                              aParam->mErrorStack ) == STL_SUCCESS );

    sParseTree->mParseTreeType = ZTP_PARSETREE_TYPE_IGNORE;

    return (YYSTYPE)sParseTree;

    STL_FINISH;

    aParam->mErrStatus = STL_FAILURE;
    return (YYSTYPE)NULL;
}

YYSTYPE ztpMakeTransactionTree( stlParseParam     *aParam,
                                zlplStatementType  aStatementType,
                                stlBool            aIsCommit,
                                stlBool            aIsRelease)
{
    ztpTransactionTree * sParseTree = NULL;

    STL_TRY( ztpSqlAllocator( aParam->mContext,
                              STL_SIZEOF( ztpTransactionTree ),
                              (void**)&sParseTree,
                              aParam->mErrorStack ) == STL_SUCCESS );

    sParseTree->mParseTreeType = ZTP_PARSETREE_TYPE_SQL_TRANSACTION;
    sParseTree->mStatementType = aStatementType;
    sParseTree->mIsCommit = aIsCommit;
    sParseTree->mIsRelease = aIsRelease;

    return (YYSTYPE)sParseTree;

    STL_FINISH;

    aParam->mErrStatus = STL_FAILURE;
    return (YYSTYPE)NULL;
}

YYSTYPE ztpMakeBypassSqlTree( stlParseParam     *aParam,
                              zlplStatementType  aStatementType,
                              ztpObjectType      aObjectType )
{
    ztpBypassTree * sParseTree = NULL;

    STL_TRY( ztpSqlAllocator( aParam->mContext,
                              STL_SIZEOF( ztpBypassTree ),
                              (void**)&sParseTree,
                              aParam->mErrorStack ) == STL_SUCCESS );

    sParseTree->mParseTreeType = ZTP_PARSETREE_TYPE_SQL_BYPASS;
    sParseTree->mStatementType = aStatementType;
    sParseTree->mObjectType = aObjectType;
    sParseTree->mHostVarList = ((ztpSqlParseParam*)(aParam->mContext))->mHostVarList;
    sParseTree->mSqlString = gSqlBuffer;

    return (YYSTYPE)sParseTree;

    STL_FINISH;

    aParam->mErrStatus = STL_FAILURE;
    return (YYSTYPE)NULL;
}

/**
 * @brief DECLARE CURSOR (ISO Standard) parse tree 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aCursorName      Cursor Name
 * @param[in]   aSensitivity     Cursor Sensitivity
 * @param[in]   aScrollability   Cursor Scrollability
 * @param[in]   aHoldability     Cursor Holdability
 * @param[in]   aCursorQuery     Cursor Query
 * @return ztpDeclCursor
 * @remarks
 */
ztpDeclCursor *ztpMakeDeclareCursorISO(stlParseParam           * aParam,
                                       ztpIdentifier           * aCursorName,
                                       zlplCursorSensitivity     aSensitivity,
                                       zlplCursorScrollability   aScrollability,
                                       zlplCursorHoldability     aHoldability,
                                       stlChar                 * aCursorQuery )
{
    ztpDeclCursor *sDeclCursor = NULL;

    STL_TRY( ztpSqlAllocator(aParam->mContext,
                             STL_SIZEOF( ztpDeclCursor ),
                             (void**) & sDeclCursor,
                             aParam->mErrorStack )
             == STL_SUCCESS );

    sDeclCursor->mParseTreeType = ZTP_PARSETREE_TYPE_DECLARE_CURSOR;
    sDeclCursor->mType          = ZLPL_STMT_DECLARE_CURSOR;

    sDeclCursor->mCursorName    = aCursorName->mName;
    sDeclCursor->mCursorNamePos = aCursorName->mNamePos;

    sDeclCursor->mOriginType = ZLPL_CURSOR_ORIGIN_STANDING_CURSOR;

    sDeclCursor->mStandardType  = ZLPL_CURSOR_STANDARD_ISO;
    sDeclCursor->mSensitivity   = aSensitivity;
    sDeclCursor->mScrollability = aScrollability;
    sDeclCursor->mHoldability   = aHoldability;
    sDeclCursor->mUpdatability  = ZLPL_CURSOR_UPDATABILITY_NA;
    sDeclCursor->mReturnability = ZLPL_CURSOR_RETURNABILITY_NA;

    sDeclCursor->mCursorOriginOffset = aCursorQuery - aParam->mBuffer;
    sDeclCursor->mCursorOrigin  = aCursorQuery;

    sDeclCursor->mHostVarList   = ((ztpSqlParseParam*)(aParam->mContext))->mHostVarList;
    sDeclCursor->mNext          = NULL;

    return sDeclCursor;

    STL_FINISH;

    aParam->mErrStatus = STL_FAILURE;
    return NULL;
}


/**
 * @brief Dynamic CURSOR (ISO Standard) parse tree 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aCursorName      Cursor Name
 * @param[in]   aSensitivity     Cursor Sensitivity
 * @param[in]   aScrollability   Cursor Scrollability
 * @param[in]   aHoldability     Cursor Holdability
 * @param[in]   aStmtName        Dynamic Statement Name
 * @return ztpDeclCursor
 * @remarks
 */
ztpDeclCursor *ztpMakeDynamicCursorISO(stlParseParam           * aParam,
                                       ztpIdentifier           * aCursorName,
                                       zlplCursorSensitivity     aSensitivity,
                                       zlplCursorScrollability   aScrollability,
                                       zlplCursorHoldability     aHoldability,
                                       ztpIdentifier           * aStmtName )
{
    ztpDeclCursor *sDeclCursor = NULL;

    STL_TRY( ztpSqlAllocator(aParam->mContext,
                             STL_SIZEOF( ztpDeclCursor ),
                             (void**) & sDeclCursor,
                             aParam->mErrorStack )
             == STL_SUCCESS );

    sDeclCursor->mParseTreeType = ZTP_PARSETREE_TYPE_DECLARE_CURSOR;
    sDeclCursor->mType          = ZLPL_STMT_DECLARE_CURSOR;

    sDeclCursor->mCursorName    = aCursorName->mName;
    sDeclCursor->mCursorNamePos = aCursorName->mNamePos;

    sDeclCursor->mOriginType = ZLPL_CURSOR_ORIGIN_DECLARED_DYNAMIC_CURSOR;

    sDeclCursor->mStandardType  = ZLPL_CURSOR_STANDARD_ISO;
    sDeclCursor->mSensitivity   = aSensitivity;
    sDeclCursor->mScrollability = aScrollability;
    sDeclCursor->mHoldability   = aHoldability;
    sDeclCursor->mUpdatability  = ZLPL_CURSOR_UPDATABILITY_NA;
    sDeclCursor->mReturnability = ZLPL_CURSOR_RETURNABILITY_NA;

    sDeclCursor->mCursorOriginOffset = 0;
    sDeclCursor->mCursorOrigin  = aStmtName->mName;

    sDeclCursor->mHostVarList   = ((ztpSqlParseParam*)(aParam->mContext))->mHostVarList;
    sDeclCursor->mNext          = NULL;

    return sDeclCursor;

    STL_FINISH;

    aParam->mErrStatus = STL_FAILURE;
    return NULL;
}

/**
 * @brief DECLARE CURSOR (ODBC Standard) parse tree 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aCursorName      Cursor Name
 * @param[in]   aStandardType    Cursor Standard Type
 * @param[in]   aHoldability     Cursor Holdability
 * @param[in]   aCursorQuery     Cursor Query
 * @return ztpDeclCursor
 * @remarks
 */
ztpDeclCursor *ztpMakeDeclareCursorODBC(stlParseParam          * aParam,
                                        ztpIdentifier          * aCursorName,
                                        zlplCursorStandardType   aStandardType,
                                        zlplCursorHoldability    aHoldability,
                                        stlChar                * aCursorQuery )
{
    ztpDeclCursor * sDeclCursor = NULL;

    STL_TRY( ztpSqlAllocator(aParam->mContext,
                             STL_SIZEOF( ztpDeclCursor ),
                             (void**)&sDeclCursor,
                             aParam->mErrorStack )
             == STL_SUCCESS );

    sDeclCursor->mParseTreeType = ZTP_PARSETREE_TYPE_DECLARE_CURSOR;
    sDeclCursor->mType          = ZLPL_STMT_DECLARE_CURSOR;

    sDeclCursor->mCursorName    = aCursorName->mName;
    sDeclCursor->mCursorNamePos = aCursorName->mNamePos;

    sDeclCursor->mOriginType = ZLPL_CURSOR_ORIGIN_STANDING_CURSOR;

    sDeclCursor->mStandardType  = aStandardType;
    sDeclCursor->mSensitivity   = ZLPL_CURSOR_SENSITIVITY_NA;
    sDeclCursor->mScrollability = ZLPL_CURSOR_SCROLLABILITY_NA;
    sDeclCursor->mHoldability   = aHoldability;
    sDeclCursor->mUpdatability  = ZLPL_CURSOR_UPDATABILITY_NA;
    sDeclCursor->mReturnability = ZLPL_CURSOR_RETURNABILITY_NA;

    sDeclCursor->mCursorOriginOffset = aCursorQuery - aParam->mBuffer;
    sDeclCursor->mCursorOrigin  = aCursorQuery;

    sDeclCursor->mHostVarList   = ((ztpSqlParseParam*)(aParam->mContext))->mHostVarList;
    sDeclCursor->mNext          = NULL;

    return sDeclCursor;

    STL_FINISH;

    aParam->mErrStatus = STL_FAILURE;
    return NULL;
}


/**
 * @brief Dynamic CURSOR (ODBC Standard) parse tree 생성
 * @param[in]   aParam           Parse Parameter
 * @param[in]   aCursorName      Cursor Name
 * @param[in]   aStandardType    Cursor Standard Type
 * @param[in]   aHoldability     Cursor Holdability
 * @param[in]   aStmtName        Dynamic Statement Name
 * @return ztpDeclCursor
 * @remarks
 */
ztpDeclCursor *ztpMakeDynamicCursorODBC(stlParseParam          * aParam,
                                        ztpIdentifier          * aCursorName,
                                        zlplCursorStandardType   aStandardType,
                                        zlplCursorHoldability    aHoldability,
                                        ztpIdentifier          * aStmtName )
{
    ztpDeclCursor * sDeclCursor = NULL;

    STL_TRY( ztpSqlAllocator(aParam->mContext,
                             STL_SIZEOF( ztpDeclCursor ),
                             (void**)&sDeclCursor,
                             aParam->mErrorStack )
             == STL_SUCCESS );

    sDeclCursor->mParseTreeType = ZTP_PARSETREE_TYPE_DECLARE_CURSOR;
    sDeclCursor->mType          = ZLPL_STMT_DECLARE_CURSOR;

    sDeclCursor->mCursorName    = aCursorName->mName;
    sDeclCursor->mCursorNamePos = aCursorName->mNamePos;

    sDeclCursor->mOriginType = ZLPL_CURSOR_ORIGIN_DECLARED_DYNAMIC_CURSOR;

    sDeclCursor->mStandardType  = aStandardType;
    sDeclCursor->mSensitivity   = ZLPL_CURSOR_SENSITIVITY_NA;
    sDeclCursor->mScrollability = ZLPL_CURSOR_SCROLLABILITY_NA;
    sDeclCursor->mHoldability   = aHoldability;
    sDeclCursor->mUpdatability  = ZLPL_CURSOR_UPDATABILITY_NA;
    sDeclCursor->mReturnability = ZLPL_CURSOR_RETURNABILITY_NA;

    sDeclCursor->mCursorOriginOffset = 0;
    sDeclCursor->mCursorOrigin  = aStmtName->mName;

    sDeclCursor->mHostVarList   = ((ztpSqlParseParam*)(aParam->mContext))->mHostVarList;
    sDeclCursor->mNext          = NULL;

    return sDeclCursor;

    STL_FINISH;

    aParam->mErrStatus = STL_FAILURE;
    return NULL;
}

/**
 * @brief OPEN CURSOR를 parsing
 * @param[in]   aParam       Parse Parameter
 * @param[in]   aName        Cursor Name
 * @param[in]   aUsingParam  USING parameter
 * @return ztpOpenCursor *
 * @remarks
 */
ztpOpenCursor * ztpMakeOpenCursor( stlParseParam   * aParam,
                                   ztpIdentifier   * aName,
                                   ztpDynamicParam * aUsingParam )
{
    ztpOpenCursor * sOpenCursor = NULL;
    ztpCVariable  * sHost = NULL;

    STL_TRY(ztpSqlAllocator(aParam->mContext,
                            STL_SIZEOF(ztpOpenCursor),
                            (void**)&sOpenCursor,
                            aParam->mErrorStack)
             == STL_SUCCESS);

    sOpenCursor->mParseTreeType = ZTP_PARSETREE_TYPE_OPEN_CURSOR;
    sOpenCursor->mType = ZLPL_STMT_OPEN_CURSOR;

    sOpenCursor->mCursorName    = aName->mName;
    sOpenCursor->mCursorNamePos = aName->mNamePos;

    sOpenCursor->mDynUsing      = aUsingParam;

    /**
     * USING paramter 정보를 설정
     */

    if ( aUsingParam != NULL )
    {
        for ( sHost = aUsingParam->mHostVar; sHost != NULL; sHost = sHost->mNext )
        {
            /**
             * OPEN .. USING dynamic parameter 는 항상 IN Parameter 이다.
             */
            sHost->mParamIOType = ZLPL_PARAM_IO_TYPE_IN;
        }
    }
    else
    {
        /* nothing to do */
    }

    return sOpenCursor;

    STL_FINISH;

    aParam->mErrStatus = STL_FAILURE;
    return NULL;
}

/**
 * @brief FETCH ORIENTATION를 parsing
 * @param[in]   aParam         Parse Parameter
 * @param[in]   aFetchOrient   Fetch Orientation
 * @param[in]   aOffsetStr     Fetch Offset String
 * @param[in]   aOffsetPos     Fetch Offset Position
 * @param[in]   aOffsetHostVar Fetch Offset Host Variable
 * @return ztpFetchOrient *
 * @remarks
 */
ztpFetchOrient * ztpMakeFetchOrientation( stlParseParam        * aParam,
                                          zlplFetchOrientation   aFetchOrient,
                                          stlChar              * aOffsetStr,
                                          stlInt32               aOffsetPos,
                                          ztpCVariable         * aOffsetHostVar )
{
    ztpFetchOrient *sFetchOrient = NULL;

    STL_TRY_THROW(aFetchOrient != ZLPL_FETCH_NA, EXIT_FUNC);

    STL_TRY(ztpSqlAllocator( aParam->mContext,
                             STL_SIZEOF(ztpFetchOrient),
                             (void**)&sFetchOrient,
                             aParam->mErrorStack )
             == STL_SUCCESS);

    sFetchOrient->mFetchOrient = aFetchOrient;

    if ( aOffsetStr != NULL )
    {
        STL_DASSERT( aOffsetHostVar == NULL );

        sFetchOrient->mFetchPos.mType = ZTP_NAMETYPE_LITERAL;

        stlStrncpy(sFetchOrient->mFetchPos.mName, aOffsetStr, stlStrlen(aOffsetStr) + 1);
        sFetchOrient->mFetchPos.mNamePos = aOffsetPos;
    }
    else
    {
        sFetchOrient->mFetchPos.mType = ZTP_NAMETYPE_LITERAL;

        stlStrncpy(sFetchOrient->mFetchPos.mName, "0", 2);
        sFetchOrient->mFetchPos.mNamePos = 0;
    }

    sFetchOrient->mFetchOffsetHost = aOffsetHostVar;

    STL_RAMP(EXIT_FUNC);

    return sFetchOrient;

    STL_FINISH;

    aParam->mErrStatus = STL_FAILURE;
    return NULL;
}

/**
 * @brief FETCH CURSOR를 parsing
 * @param[in]   aParam         Parse Parameter
 * @param[in]   aName          Cursor Name
 * @param[in]   aFetchOrient   Fetch할 cursor의 Fetch Orientation
 * @return ztpFetchCursor *
 * @remarks
 */
ztpFetchCursor *ztpMakeFetchCursor(stlParseParam  *aParam,
                                   ztpIdentifier  *aName,
                                   ztpFetchOrient *aFetchOrient)
{
    ztpFetchCursor *sFetchCursor = NULL;

    STL_TRY(ztpSqlAllocator(aParam->mContext,
                            STL_SIZEOF(ztpFetchCursor),
                            (void**)&sFetchCursor,
                            aParam->mErrorStack)
             == STL_SUCCESS);

    sFetchCursor->mParseTreeType = ZTP_PARSETREE_TYPE_FETCH_CURSOR;
    sFetchCursor->mType = ZLPL_STMT_FETCH_CURSOR;

    sFetchCursor->mCursorName    = aName->mName;
    sFetchCursor->mCursorNamePos = aName->mNamePos;

    sFetchCursor->mHostVarList = ((ztpSqlParseParam*)(aParam->mContext))->mHostVarList;;
    sFetchCursor->mFetchOrientation = aFetchOrient;

    return sFetchCursor;

    STL_FINISH;

    aParam->mErrStatus = STL_FAILURE;
    return NULL;
}

/**
 * @brief CLOSE CURSOR를 parsing
 * @param[in]   aParam    Parse Parameter
 * @param[in]   aName     Cursor Name
 * @return ztpCloseCursor *
 * @remarks
 */
ztpCloseCursor *ztpMakeCloseCursor(stlParseParam *aParam,
                                   ztpIdentifier *aName)
{
    ztpCloseCursor *sCloseCursor = NULL;

    STL_TRY(ztpSqlAllocator(aParam->mContext,
                            STL_SIZEOF(ztpCloseCursor),
                            (void**)&sCloseCursor,
                            aParam->mErrorStack)
             == STL_SUCCESS);

    sCloseCursor->mParseTreeType = ZTP_PARSETREE_TYPE_CLOSE_CURSOR;
    sCloseCursor->mType = ZLPL_STMT_CLOSE_CURSOR;

    sCloseCursor->mCursorName    = aName->mName;
    sCloseCursor->mCursorNamePos = aName->mNamePos;

    return sCloseCursor;

    STL_FINISH;

    aParam->mErrStatus = STL_FAILURE;
    return NULL;
}


/**
 * @brief Positioned Cursor DML 을 위한 Parse Tree 를 생성한다.
 * @param[in] aParam      Parser Parameter
 * @param[in] aName       Cursor Name Identifier
 * @param[in] aCursorDML  Cursor DML string
 * @remarks
 */
ztpPositionedCursorDML * ztpMakePositionedCursorDML( stlParseParam * aParam,
                                                     ztpIdentifier * aName,
                                                     stlChar       * aCursorDML )
{
    ztpPositionedCursorDML * sPositionedDML = NULL;

    STL_TRY(ztpSqlAllocator(aParam->mContext,
                            STL_SIZEOF(ztpPositionedCursorDML),
                            (void**) & sPositionedDML,
                            aParam->mErrorStack)
             == STL_SUCCESS);

    sPositionedDML->mParseTreeType = ZTP_PARSETREE_TYPE_POSITIONED_CURSOR_DML;
    sPositionedDML->mType = ZLPL_STMT_POSITIONED_CURSOR_DML;

    sPositionedDML->mCursorName    = aName->mName;
    sPositionedDML->mCursorNamePos = aName->mNamePos;

    sPositionedDML->mCursorDML = gSqlBuffer;
    sPositionedDML->mHostVarList = ((ztpSqlParseParam*)(aParam->mContext))->mHostVarList;

    return sPositionedDML;

    STL_FINISH;

    aParam->mErrStatus = STL_FAILURE;
    return NULL;
}

/**
 * @brief Identifier를 parsing
 * @param[in]   aParam    Parse Parameter
 * @param[in]   aName     Identifier Name
 * @param[in]   aNamePos  Identifier Name Position
 * @return Identifier
 * @remarks
 */
ztpIdentifier *ztpMakeIdentifier(stlParseParam *aParam,
                                 stlChar       *aName,
                                 stlInt32       aNamePos )
{
    ztpIdentifier *sIdentifier = NULL;

    STL_TRY(ztpSqlAllocator(aParam->mContext,
                            STL_SIZEOF(ztpIdentifier),
                            (void**)&sIdentifier,
                            aParam->mErrorStack)
             == STL_SUCCESS);

    STL_TRY_THROW( stlStrlen( aName ) < STL_MAX_SQL_IDENTIFIER_LENGTH, RAMP_ERR_INVALID_LENGTH );

    stlStrncpy(sIdentifier->mName, aName, STL_MAX_SQL_IDENTIFIER_LENGTH);
    sIdentifier->mNamePos = aNamePos;


    return sIdentifier;

    STL_CATCH( RAMP_ERR_INVALID_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTP_ERRCODE_IDENTIFIER_LENGTH_EXCEEDED,
                      NULL,
                      aParam->mErrorStack );
        STL_PARSER_ERROR( NULL, aParam, aParam->mScanner, NULL );
    }

    STL_FINISH;

    aParam->mErrStatus = STL_FAILURE;
    return NULL;
}

/***********************************************************************
 * for Dynamic SQL
 ***********************************************************************/

/**
 * @brief EXECUTE IMMEDIATE 를 parsing
 * @param[in]   aParam         Parse Parameter
 * @param[in]   aIsQuoted      is Quoted string or plain string
 * @param[in]   aHostVariable  Host Variable
 * @param[in]   aStringSQL     SQL string
 * @return ztpExecuteImmediate
 * @remarks
 */
ztpExecuteImmediate * ztpMakeExecuteImmediate( stlParseParam  * aParam,
                                               stlBool          aIsQuoted,
                                               ztpCVariable   * aHostVariable,
                                               stlChar        * aStringSQL )
{
    ztpExecuteImmediate * sParseTree = NULL;
    stlInt32              sSqlLen;
    stlChar             * sStringSQL;

    STL_TRY(ztpSqlAllocator( aParam->mContext,
                             STL_SIZEOF(ztpExecuteImmediate),
                             (void**) & sParseTree,
                             aParam->mErrorStack )
             == STL_SUCCESS);

    sParseTree->mParseTreeType = ZTP_PARSETREE_TYPE_DYNAMIC_SQL_EXECUTE_IMMEDIATE;
    sParseTree->mType          = ZTPL_STMT_DYNAMIC_SQL_EXECUTE_IMMEDIATE;

    if ( aHostVariable != NULL )
    {
        sParseTree->mHostVariable = aHostVariable;
        sParseTree->mStringSQL = NULL;
    }
    else
    {
        //STL_DASSERT( aStringSQL != NULL );

        sParseTree->mHostVariable = NULL;
        if( aIsQuoted == STL_TRUE )
        {
            if( *aStringSQL != '\0' )
            {
                sParseTree->mStringSQL = aStringSQL;
            }
            else
            {
                sParseTree->mStringSQL = NULL;
            }
        }
        else
        {
            sSqlLen = aParam->mCurrLoc - (aStringSQL - aParam->mBuffer);

            STL_TRY(ztpSqlAllocator( aParam->mContext,
                                     sSqlLen + 1,
                                     (void**) & sStringSQL,
                                     aParam->mErrorStack )
                    == STL_SUCCESS);

            stlStrncpy( sStringSQL, aStringSQL, sSqlLen + 1);

            sParseTree->mStringSQL    = sStringSQL;
        }
    }

    return sParseTree;

    STL_FINISH;

    aParam->mErrStatus = STL_FAILURE;
    return NULL;
}



/**
 * @brief PREPARE 를 parsing
 * @param[in]   aParam         Parse Parameter
 * @param[in]   aStmtName      statement name
 * @param[in]   aIsQuoted      is Quoted string or plain string
 * @param[in]   aHostVariable  Host Variable
 * @param[in]   aStringSQL     SQL string
 * @return ztpPrepare
 * @remarks
 */
ztpPrepare * ztpMakePrepare( stlParseParam  * aParam,
                             ztpIdentifier  * aStmtName,
                             stlBool          aIsQuoted,
                             ztpCVariable   * aHostVariable,
                             stlChar        * aStringSQL )
{
    ztpPrepare * sParseTree = NULL;
    stlInt32     sSqlLen;
    stlChar    * sStringSQL;

    STL_TRY(ztpSqlAllocator( aParam->mContext,
                             STL_SIZEOF(ztpPrepare),
                             (void**) & sParseTree,
                             aParam->mErrorStack )
             == STL_SUCCESS);

    sParseTree->mParseTreeType = ZTP_PARSETREE_TYPE_DYNAMIC_SQL_PREPARE;
    sParseTree->mType          = ZTPL_STMT_DYNAMIC_SQL_PREPARE;

    sParseTree->mStmtName = aStmtName;

    if ( aHostVariable != NULL )
    {
        sParseTree->mHostVariable = aHostVariable;
        sParseTree->mStringSQL = NULL;
    }
    else
    {
        //STL_DASSERT( aStringSQL != NULL );

        sParseTree->mHostVariable = NULL;
        if( aIsQuoted == STL_TRUE )
        {
            if( *aStringSQL != '\0' )
            {
                sParseTree->mStringSQL = aStringSQL;
            }
            else
            {
                sParseTree->mStringSQL = NULL;
            }
        }
        else
        {
            sSqlLen = aParam->mCurrLoc - (aStringSQL - aParam->mBuffer);

            STL_TRY(ztpSqlAllocator( aParam->mContext,
                                     sSqlLen + 1,
                                     (void**) & sStringSQL,
                                     aParam->mErrorStack )
                    == STL_SUCCESS);

            stlStrncpy( sStringSQL, aStringSQL, sSqlLen + 1);

            sParseTree->mStringSQL    = sStringSQL;
        }
    }

    return sParseTree;

    STL_FINISH;

    aParam->mErrStatus = STL_FAILURE;
    return NULL;
}


/**
 * @brief Dynamic Parameter 를 parsing
 * @param[in]  aParam         Parse Parameter
 * @param[in]  aIsDesc        Is Descriptor or Host Variable
 * @param[in]  aHostVariable  Host Variable
 * @param[in]  aDescName      Descriptor Name
 */
ztpDynamicParam * ztpMakeDynamicParam( stlParseParam  * aParam,
                                       stlBool          aIsDesc,
                                       ztpCVariable   * aHostVariable,
                                       stlChar        * aDescName )
{
    ztpDynamicParam * sDynParam = NULL;

    STL_TRY(ztpSqlAllocator( aParam->mContext,
                             STL_SIZEOF(ztpDynamicParam),
                             (void**) & sDynParam,
                             aParam->mErrorStack )
             == STL_SUCCESS);

    sDynParam->mIsDesc   = aIsDesc;
    sDynParam->mHostVar  = aHostVariable;
    sDynParam->mDescName = aDescName;

    return sDynParam;

    STL_FINISH;

    aParam->mErrStatus = STL_FAILURE;
    return NULL;
}

/**
 * @brief EXECUTE 를 parsing
 * @param[in]   aParam         Parse Parameter
 * @param[in]   aStmtName      statement name
 * @param[in]   aDynUsing      USING dynamic
 * @param[in]   aDynInto       INTO dynamic
 * @return ztpPrepare
 * @remarks
 */
ztpExecute * ztpMakeExecute( stlParseParam    * aParam,
                             ztpIdentifier    * aStmtName,
                             ztpDynamicParam  * aDynUsing,
                             ztpDynamicParam  * aDynInto )
{
    ztpExecute * sParseTree = NULL;
    ztpCVariable * sHost = NULL;

    STL_TRY(ztpSqlAllocator( aParam->mContext,
                             STL_SIZEOF(ztpExecute),
                             (void**) & sParseTree,
                             aParam->mErrorStack )
             == STL_SUCCESS);

    sParseTree->mParseTreeType = ZTP_PARSETREE_TYPE_DYNAMIC_SQL_EXECUTE;
    sParseTree->mType          = ZTPL_STMT_DYNAMIC_SQL_EXECUTE;

    sParseTree->mStmtName  = aStmtName;
    sParseTree->mDynUsing  = aDynUsing;
    sParseTree->mDynInto   = aDynInto;

    /**
     * USING parameter 정보 설정
     */

    if ( aDynUsing != NULL )
    {
        for ( sHost = aDynUsing->mHostVar; sHost != NULL; sHost = sHost->mNext )
        {
            /**
             * dynamic parameter 는 IN/OUT 여부를 판단할 수 없다.
             * - 이 값을 사용해서는 안됨
             */

            sHost->mParamIOType = ZLPL_PARAM_IO_TYPE_MAX;
        }
    }
    else
    {
        /* nothing to do */
    }

    /**
     * INTO parameter 정보 설정
     */

    if ( aDynInto != NULL )
    {
        for ( sHost = aDynInto->mHostVar; sHost != NULL; sHost = sHost->mNext )
        {
            sHost->mParamIOType = ZLPL_PARAM_IO_TYPE_OUT;
        }
    }
    else
    {
        /* nothing to do */
    }

    return sParseTree;

    STL_FINISH;

    aParam->mErrStatus = STL_FAILURE;
    return NULL;
}

/***********************************************************************
 * for Host Variable
 ***********************************************************************/

stlChar * ztpMakeSignedNumericString( stlParseParam * aParam,
                                      stlInt32        aStartStrPos,
                                      stlInt32        aEndStrPos )
{
    stlInt32   sStrLen = aEndStrPos - aStartStrPos;
    stlChar  * sString = NULL;

    STL_DASSERT( sStrLen > 1 );

    STL_TRY(ztpSqlAllocator( aParam->mContext,
                             sStrLen + 1,
                             (void**) & sString,
                             aParam->mErrorStack)
            == STL_SUCCESS);

    stlMemcpy( sString,
               & aParam->mBuffer[aStartStrPos],
               sStrLen );
    sString[sStrLen] = '\0';

    return sString;

    STL_FINISH;

    aParam->mErrStatus = STL_FAILURE;
    return NULL;
}

ztpHostVariable *ztpMakeHostVariableInSql( stlParseParam *aParam,
                                           stlInt32       aStartPos,
                                           stlInt32      *aEndPos )
{
    ztpHostVariable *sHostVariable = NULL;
    stlAllocator    *sAllocator = ((ztpSqlParseParam*)aParam->mContext)->mAllocator;
    stlErrorData    *sErrorData = NULL;
    stlChar          sErrorMsg[STL_MAX_ERROR_MESSAGE + 2];

    sHostVariable = ztpvMakeHostVariable( sAllocator,
                                          aParam->mErrorStack,
                                          aParam->mBuffer,
                                          aStartPos,
                                          aEndPos );

    STL_TRY_THROW( sHostVariable != NULL, RAMP_ERR_SYMBOL );

    return (YYSTYPE)sHostVariable;

    STL_CATCH(RAMP_ERR_SYMBOL)
    {
        STL_PARSER_ERROR( NULL, aParam, aParam->mScanner, NULL );
        sErrorData = stlPopError( aParam->mErrorStack );

        /*
         * 반드시 ErrorStack이 비어있지 않아야 한다.
         */
        STL_DASSERT( sErrorData != NULL );

        stlSnprintf( sErrorMsg,
                     STL_MAX_ERROR_MESSAGE + 2,
                     ", %s",
                     sErrorData->mErrorMessage );
        (void)ztpuAppendErrorMsg( sAllocator,
                                  aParam->mErrorStack,
                                  &aParam->mErrMsg,
                                  sErrorMsg );
    }
    STL_FINISH;

    aParam->mErrStatus = STL_FAILURE;
    return (YYSTYPE)NULL;
}

ztpCVariable *ztpAddHostVariable( stlParseParam   *aParam,
                                  ztpHostVariable *aHostVariableDesc,
                                  ztpCVariable    *aHostIndicator,
                                  stlInt32         aStartPos,
                                  stlBool          aIn )
{
    ztpCVariable      *sHostVariable;
    ztpCVariable      *sHostVariableList = NULL;
    ztpCVariable      *sCurrHostVariable = NULL;
    ztpCVariable      *sCurrHostIndicator = NULL;
    ztpSqlParseParam  *sSqlParam = (ztpSqlParseParam*)(aParam->mContext);
    stlChar           *sCopyStartPos = NULL;
    stlInt32           i;
    stlInt32           sNameLen;
    stlInt32           sMemberCount = 0;
    stlBool            sIsSimpleVar = STL_FALSE;

    STL_TRY(ztpSqlAllocator(aParam->mContext,
                            STL_SIZEOF(ztpCVariable),
                            (void**)&sHostVariable,
                            aParam->mErrorStack)
            == STL_SUCCESS);
    ZTP_INIT_C_VARIABLE( sHostVariable );

    sNameLen = stlStrlen( aHostVariableDesc->mHostVarString );
    STL_TRY(ztpSqlAllocator(aParam->mContext,
                            sNameLen + 1,
                            (void **)&(sHostVariable->mName),
                            aParam->mErrorStack)
            == STL_SUCCESS );

    stlStrncpy( sHostVariable->mName,
                aHostVariableDesc->mHostVarString,
                sNameLen + 1 );
    sHostVariable->mNamePos = aStartPos;

    STL_TRY_THROW( ztpcHostVar2CVariable( sHostVariable,
                                          aHostVariableDesc,
                                          &sIsSimpleVar,
                                          aParam->mErrorStack )
                   == STL_SUCCESS, RAMP_ERR_SYMBOL );

    sHostVariable->mNext = NULL;

    if( sHostVariable->mDataType == ZLPL_C_DATATYPE_STRUCT )
    {
        STL_TRY( ztpcSpreadStructMember( sHostVariable,
                                         &sHostVariableList,
                                         aIn,
                                         &sMemberCount,
                                         sSqlParam->mAllocator,
                                         aParam->mErrorStack )
                 == STL_SUCCESS );

        sCurrHostVariable = sHostVariableList;
        sCurrHostIndicator = aHostIndicator;

        while( ( sCurrHostVariable != NULL )
               && ( sCurrHostIndicator != NULL ) )
        {
            sCurrHostVariable->mIndicator = (ztpCVariable *)sCurrHostIndicator;

            sCurrHostVariable = sCurrHostVariable->mNext;
            sCurrHostIndicator = sCurrHostIndicator->mNext;
        }
    }
    else
    {
        if( aIn == STL_TRUE )
        {
            sHostVariable->mParamIOType = ZLPL_PARAM_IO_TYPE_IN;
        }
        else
        {
            sHostVariable->mParamIOType = ZLPL_PARAM_IO_TYPE_OUT;
        }

        sHostVariable->mIndicator = (ztpCVariable *)aHostIndicator;

        sHostVariableList = sHostVariable;
    }

    if( sSqlParam->mHostVarList == NULL )
    {
        sSqlParam->mHostVarList = sHostVariableList;
    }
    else
    {
        ztpAddHostVariableList( sSqlParam->mHostVarList,
                                sHostVariableList );
    }

    /*
     * 특수 기호(., ->, [])가 사용된 Host variable을 plain text로 치환
     * 특수 기호에 해당하는 문자들을 _ 로 바꿈
     */
    if( sHostVariable->mDataType == ZLPL_C_DATATYPE_STRUCT )
    {
        sCopyStartPos = aParam->mBuffer + gSqlBufferIdx;
        stlStrncat( gSqlBuffer, sCopyStartPos, aStartPos - gSqlBufferIdx + 1);

        for( i = 0; i < sMemberCount - 1; i ++ )
        {
            stlStrcat( gSqlBuffer, "?, " );
        }
        stlStrcat( gSqlBuffer, "?" );

        //gSqlBufferIdx = aStartPos + sNameLen + 1;
        if( aHostIndicator == NULL )
        {
            gSqlBufferIdx = aParam->mCurrLoc;
        }
        else
        {
            gSqlBufferIdx = aParam->mNextLoc;
        }
    }
    else
    {
        sNameLen = stlStrlen(sHostVariable->mName);
        for(i = 0; i < sNameLen; i ++)
        {
            if( stlIsalnum( aParam->mBuffer[aStartPos + i + 1] ) == 0 )
            {
                aParam->mBuffer[aStartPos + i + 1] = '_';
            }
        }

        if( sHostVariable->mIndicator != NULL )
        {
            sCurrHostIndicator = sHostVariable->mIndicator;
            sNameLen = stlStrlen(sCurrHostIndicator->mName);
            for(i = 0; i < sNameLen; i ++)
            {
                if( stlIsalnum( aParam->mBuffer[sCurrHostIndicator->mNamePos + i + 1] ) == 0 )
                {
                    aParam->mBuffer[sCurrHostIndicator->mNamePos + i + 1] = '_';
                }
            }
        }
    }

    return (YYSTYPE)sHostVariable;

    STL_CATCH(RAMP_ERR_SYMBOL)
    {
        STL_PARSER_ERROR( NULL, aParam, aParam->mScanner, NULL );
    }
    STL_FINISH;

    aParam->mErrStatus = STL_FAILURE;
    return (YYSTYPE)NULL;
}

ztpCVariable *ztpMakeHostIndicator( stlParseParam   *aParam,
                                    ztpHostVariable *aHostIndicatorDesc,
                                    stlInt32         aIndStartPos,
                                    stlInt32         aHostVarStartPos )
{
    ztpCVariable      *sHostIndicator = NULL;
    ztpCVariable      *sHostIndicatorList = NULL;
    ztpSqlParseParam  *sSqlParam = (ztpSqlParseParam*)(aParam->mContext);
    stlInt32           sNameLen;
    stlBool            sIsSimpleVar = STL_FALSE;
    stlInt32           sMemberCount = 0;

    STL_TRY( ztpSqlAllocator( aParam->mContext,
                              STL_SIZEOF( ztpCVariable ),
                              (void**)&sHostIndicator,
                              aParam->mErrorStack )
             == STL_SUCCESS );

    ZTP_INIT_C_VARIABLE( sHostIndicator );

    sNameLen = stlStrlen( aHostIndicatorDesc->mHostVarString );
    STL_TRY(ztpSqlAllocator(aParam->mContext,
                            sNameLen + 1,
                            (void **)&(sHostIndicator->mName),
                            aParam->mErrorStack)
            == STL_SUCCESS );

    stlStrncpy( sHostIndicator->mName,
                aHostIndicatorDesc->mHostVarString,
                sNameLen + 1 );
    sHostIndicator->mNamePos = aHostVarStartPos;
    sHostIndicator->mNext = NULL;

    STL_TRY_THROW( ztpcHostVar2CVariable( sHostIndicator,
                                          aHostIndicatorDesc,
                                          &sIsSimpleVar,
                                          aParam->mErrorStack )
                   == STL_SUCCESS, RAMP_ERR_SYMBOL );
    sHostIndicator->mNext = NULL;

    if( sHostIndicator->mDataType == ZLPL_C_DATATYPE_STRUCT )
    {
        STL_TRY( ztpcSpreadStructMember( sHostIndicator,
                                         &sHostIndicatorList,
                                         ZLPL_PARAM_IO_TYPE_IN,
                                         &sMemberCount,
                                         sSqlParam->mAllocator,
                                         aParam->mErrorStack )
                 == STL_SUCCESS );
    }
    else
    {
        sHostIndicatorList = sHostIndicator;
    }

    return (YYSTYPE)sHostIndicatorList;

    STL_CATCH(RAMP_ERR_SYMBOL)
    {
        STL_PARSER_ERROR( NULL, aParam, aParam->mScanner, NULL );
    }
    STL_FINISH;

    aParam->mErrStatus = STL_FAILURE;
    return (YYSTYPE)NULL;
}

void ztpAddHostVariableList( ztpCVariable * aHostVariableList,
                             ztpCVariable * aHostVariable )
{
    ztpCVariable * sHostVariable = NULL;

    sHostVariable = (ztpCVariable*)aHostVariableList;

    STL_ASSERT( sHostVariable != NULL );

    while( sHostVariable->mNext != NULL )
    {
        sHostVariable = (ztpCVariable*)sHostVariable->mNext;
    }

    sHostVariable->mNext = aHostVariable;
}

ztpNameTag *ztpMakeNameTag( stlParseParam    *aParam,
                            ztpNameType       aType,
                            void             *aHostVarDesc,
                            stlInt32          aHostVarDescPos )
{
    ztpNameTag       *sNameTag;
    ztpCVariable      sCVariable;
    stlBool           sIsSimpleVar = STL_FALSE;
    ztpHostVariable  *sHostVariable = (ztpHostVariable *)aHostVarDesc;
    stlChar          *sLiteral = (stlChar *)aHostVarDesc;

    ZTP_INIT_C_VARIABLE( &sCVariable );
    STL_TRY( ztpSqlAllocator(aParam->mContext,
                             STL_SIZEOF(ztpNameTag),
                             (void**)&sNameTag,
                             aParam->mErrorStack)
             == STL_SUCCESS );

    switch(aType)
    {
        case ZTP_NAMETYPE_DQ_LITERAL:
            sNameTag->mType = ZTP_NAMETYPE_LITERAL;
            stlStrncpy(sNameTag->mName, &sLiteral[1], stlStrlen(sLiteral) - 1);
            break;
        case ZTP_NAMETYPE_HOSTVAR:
            sNameTag->mType = ZTP_NAMETYPE_HOSTVAR;
            STL_TRY( ztpcHostVar2CVariable( &sCVariable,
                                            sHostVariable,
                                            &sIsSimpleVar,
                                            aParam->mErrorStack )
                     == STL_SUCCESS );

            stlStrncpy(sNameTag->mName, sHostVariable->mHostVarString, stlStrlen(sHostVariable->mHostVarString) + 1);

            if(sCVariable.mDataType == ZLPL_C_DATATYPE_VARCHAR)
            {
                stlStrncat(sNameTag->mName, ".arr", 5);
            }
            break;
        default:
            STL_TRY(STL_FALSE);
            break;
    }

    sNameTag->mNamePos = aHostVarDescPos;

    return sNameTag;

    STL_FINISH;

    aParam->mErrStatus = STL_FAILURE;
    return NULL;
}

stlChar *ztpMakeStaticConnStringInSql( stlParseParam   *aParam,
                                       stlChar         *aConnName )
{
    stlChar           *sConnName = NULL;
    ztpSqlParseParam  *sSqlParam = ((ztpSqlParseParam*)aParam->mContext);

    sConnName = ztpuMakeStaticConnString( sSqlParam->mAllocator,
                                          aParam->mErrorStack,
                                          aConnName );

    STL_TRY( sConnName != NULL );

    return sConnName;

    STL_FINISH;

    aParam->mErrStatus = STL_FAILURE;
    return NULL;
}

ztpConnectParam *ztpMakeConnectParam(stlParseParam   *aParam,
                                     ztpNameTag      *aUserName,
                                     ztpNameTag      *aPassword)
{
    ztpConnectParam   *sConnectParam;

    STL_TRY(ztpSqlAllocator(aParam->mContext,
                            STL_SIZEOF(ztpConnectParam),
                            (void**)&sConnectParam,
                            aParam->mErrorStack)
            == STL_SUCCESS);

    sConnectParam->mParseTreeType = ZTP_PARSETREE_TYPE_SQL_CONNECT;
    sConnectParam->mUserName = aUserName;
    sConnectParam->mPassword = aPassword;

    aParam->mParseTree = (ztpParseTree *)sConnectParam;

    return (YYSTYPE)sConnectParam;

    STL_FINISH;

    aParam->mErrStatus = STL_FAILURE;
    return (YYSTYPE)NULL;
}

ztpConnectParam *ztpSetConnectParam(stlParseParam   *aParam,
                                    ztpConnectParam *aConnectParam,
                                    ztpNameTag      *aConnName,
                                    ztpNameTag      *aConnString)
{
    aConnectParam->mParseTreeType = ZTP_PARSETREE_TYPE_SQL_CONNECT;
    aConnectParam->mConnName = aConnName;
    aConnectParam->mConnString = aConnString;

    aParam->mParseTree = (ztpParseTree *)aConnectParam;

    return (YYSTYPE)aConnectParam;
}

ztpDisconnectParam *ztpMakeDisconnectParam(stlParseParam   *aParam,
                                           ztpNameTag      *aConnectionObject,
                                           stlBool          aIsAll)
{
    ztpDisconnectParam   *sDisconnectParam;

    STL_TRY(ztpSqlAllocator(aParam->mContext,
                            STL_SIZEOF(ztpDisconnectParam),
                            (void**)&sDisconnectParam,
                            aParam->mErrorStack)
            == STL_SUCCESS);

    sDisconnectParam->mParseTreeType = ZTP_PARSETREE_TYPE_SQL_DISCONNECT;
    sDisconnectParam->mConnectionObject = aConnectionObject;
    sDisconnectParam->mIsAll = aIsAll;

    aParam->mParseTree = (ztpParseTree *)sDisconnectParam;

    return (YYSTYPE)sDisconnectParam;

    STL_FINISH;

    aParam->mErrStatus = STL_FAILURE;
    return (YYSTYPE)NULL;
}

stlStatus ztpSqlAllocator( void          * aContext,
                           stlInt32        aAllocSize,
                           void         ** aAddr,
                           stlErrorStack * aErrorStack )
{
    return stlAllocRegionMem( ((ztpSqlParseParam*)aContext)->mAllocator,
                              aAllocSize,
                              aAddr,
                              aErrorStack );
}

stlStatus ztpSqlParseIt( stlChar        * aSqlString,
                         stlInt32       * aEndPos,
                         stlAllocator   * aAllocator,
                         ztpParseTree  ** aParseTree,
                         stlErrorStack  * aErrorStack )
{
    stlInt32          sState;
    stlParseParam     sStlParam;
    ztpSqlParseParam  sSqlParam;

    sState = 0;
    sSqlParam.mAllocator = aAllocator;
    sSqlParam.mHostVarList = NULL;

    STL_INIT_PARSE_PARAM( &sStlParam,
                          aSqlString,
                          ztpSqlAllocator,
                          aErrorStack,
                          &sSqlParam );

    sState = 1;
    if( STL_PARSER_PARSE( &sStlParam, sStlParam.mScanner ) != 0 )
    {
        STL_TRY_THROW( sStlParam.mErrStatus == STL_SUCCESS, RAMP_ERR_SYNTAX );
    }

    *aParseTree = sStlParam.mParseTree;
    *aEndPos    = sStlParam.mCurrLoc;

    sState = 2;
    STL_PARSER_LEX_DESTROY( sStlParam.mScanner );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SYNTAX )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTP_ERRCODE_SYNTAX,
                      sStlParam.mErrMsg,
                      aErrorStack );
    }

    STL_FINISH;

    switch( sState )
    {
        case 0 :
            switch( errno )
            {
                case ENOMEM :
                    stlPushError( STL_ERROR_LEVEL_ABORT,
                                  STL_ERRCODE_OUT_OF_MEMORY,
                                  "error occurred on flex/bison",
                                  aErrorStack );
                    break;
                case EINVAL:
                    stlPushError( STL_ERROR_LEVEL_ABORT,
                                  STL_ERRCODE_INVALID_ARGUMENT,
                                  "error occurred on flex/bison",
                                  aErrorStack );
                    break;
                default :
                    STL_DASSERT( 0 );
                    break;
            }
            break;
            
        case 1 :
            STL_PARSER_LEX_DESTROY( sStlParam.mScanner );
            break;
            
        default :
            STL_DASSERT( 0 );
            break;
    }

    return STL_FAILURE;
}

/** @} */

