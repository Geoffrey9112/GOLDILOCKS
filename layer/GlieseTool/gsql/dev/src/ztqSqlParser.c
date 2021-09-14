/*******************************************************************************
 * ztqSqlParser.c
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
#include <ztqSqlLex.h>
#include <ztqSqlParser.h>
#include <ztqSqlMacro.h>
#include <ztqNamedCursor.h>

/**
 * @file ztqSqlParser.c
 * @brief SQL Parser Routines
 */

/**
 * @addtogroup ztqSqlParser
 * @{
 */

stlChar gErrMsgBuffer[STL_MAX_ERROR_MESSAGE];

YYSTYPE ztqMakeTransactionTree( stlParseParam * aParam,
                                stlBool         aIsCommit )
{
    ztqTransactionTree * sParseTree = NULL;

    if( ztqSqlAllocator( aParam->mContext,
                         STL_SIZEOF( ztqTransactionTree ),
                         (void**)&sParseTree,
                         aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sParseTree->mCommandType = ZTQ_COMMAND_TYPE_SQL_TRANSACTION;
    sParseTree->mIsCommit = aIsCommit;

    return (YYSTYPE)sParseTree;
}

YYSTYPE ztqMakeBypassSqlTree( stlParseParam    * aParam,
                              ztqStatementType   aStatementType,
                              ztqObjectType      aObjectType,
                              stlChar          * aObjectName )
{
    ztqBypassTree * sParseTree = NULL;

    if( ztqSqlAllocator( aParam->mContext,
                         STL_SIZEOF( ztqBypassTree ),
                         (void**)&sParseTree,
                         aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    sParseTree->mCommandType = ZTQ_COMMAND_TYPE_SQL_BYPASS;
    sParseTree->mStatementType = aStatementType;
    sParseTree->mObjectType = aObjectType;
    sParseTree->mObjectName = aObjectName;
    sParseTree->mHostVarList = ((ztqSqlParseParam*)(aParam->mContext))->mHostVarList;

    return (YYSTYPE)sParseTree;
}

YYSTYPE ztqAddInHostVariable( stlParseParam * aParam,
                              YYSTYPE         aHostVariable,
                              YYSTYPE         aHostIndicator )
{
    ztqHostVariable  * sHostVariable = NULL;
    ztqSqlParseParam * sSqlParam;

    if( ztqSqlAllocator( aParam->mContext,
                         STL_SIZEOF( ztqHostVariable ),
                         (void**)&sHostVariable,
                         aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    ZTQ_INIT_HOST_VARIABLE( sHostVariable );

    stlStrncpy( sHostVariable->mName,
                (const stlChar*)aHostVariable,
                STL_MAX_SQL_IDENTIFIER_LENGTH );
    sHostVariable->mBindType = ZTQ_HOST_BIND_TYPE_IN;
    sHostVariable->mNext = NULL;

    sSqlParam = (ztqSqlParseParam*)(aParam->mContext);

    if( sSqlParam->mHostVarList == NULL )
    {
        sSqlParam->mHostVarList = sHostVariable;
    }
    else
    {
        ztqAddHostVariable( sSqlParam->mHostVarList,
                            sHostVariable );
    }

    if( aHostIndicator != NULL )
    {
        ztqAddHostVariable( sSqlParam->mHostVarList,
                            (ztqHostVariable *)aHostIndicator );
        sHostVariable->mIndicatorVariable = aHostIndicator;
    }

    return (YYSTYPE)sHostVariable;
}

YYSTYPE ztqAddOutHostVariable( stlParseParam * aParam,
                               YYSTYPE         aHostVariable,
                               YYSTYPE         aHostIndicator )
{
    ztqHostVariable  * sHostVariable = NULL;
    ztqSqlParseParam * sSqlParam;

    if( ztqSqlAllocator( aParam->mContext,
                         STL_SIZEOF( ztqHostVariable ),
                         (void**)&sHostVariable,
                         aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    ZTQ_INIT_HOST_VARIABLE( sHostVariable );

    stlStrncpy( sHostVariable->mName,
                (const stlChar*)aHostVariable,
                STL_MAX_SQL_IDENTIFIER_LENGTH );
    sHostVariable->mBindType = ZTQ_HOST_BIND_TYPE_OUT;
    sHostVariable->mNext = NULL;

    sSqlParam = (ztqSqlParseParam*)(aParam->mContext);

    if( sSqlParam->mHostVarList == NULL )
    {
        sSqlParam->mHostVarList = sHostVariable;
    }
    else
    {
        ztqAddHostVariable( sSqlParam->mHostVarList,
                            sHostVariable );
    }

    if( aHostIndicator != NULL )
    {
        ztqAddHostVariable( sSqlParam->mHostVarList,
                            (ztqHostVariable *)aHostIndicator );
        sHostVariable->mIndicatorVariable = aHostIndicator;
    }

    return (YYSTYPE)sHostVariable;
}

YYSTYPE ztqMakeHostIndicator( stlParseParam * aParam,
                              YYSTYPE         aHostIndicator )
{
    ztqHostVariable * sHostIndicator = NULL;

    if( ztqSqlAllocator( aParam->mContext,
                         STL_SIZEOF( ztqHostVariable ),
                         (void**)&sHostIndicator,
                         aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
        return (YYSTYPE)NULL;
    }

    ZTQ_INIT_HOST_VARIABLE( sHostIndicator );

    stlStrncpy( sHostIndicator->mName,
                (const stlChar*)aHostIndicator,
                STL_MAX_SQL_IDENTIFIER_LENGTH );
    sHostIndicator->mBindType = ZTQ_HOST_BIND_TYPE_INDICATOR;
    sHostIndicator->mNext = NULL;

    return (YYSTYPE)sHostIndicator;
}

void ztqAddHostVariable( ztqHostVariable * aHostVariableList,
                         ztqHostVariable * aHostVariable )
{
    ztqHostVariable * sHostVariable = NULL;

    sHostVariable = (ztqHostVariable*)aHostVariableList;

    STL_ASSERT( sHostVariable != NULL );

    while( sHostVariable->mNext != NULL )
    {
        sHostVariable = (ztqHostVariable*)sHostVariable->mNext;
    }

    sHostVariable->mNext = aHostVariable;
}


void ztqSetHostVar4OpenCursor( ztqBypassTree * aParseTree, YYSTYPE aCursorName )
{
    ztqNamedCursor * sCursor = NULL;

    ztqFindNamedCursor( (stlChar*)aCursorName, & sCursor );

    if ( sCursor == NULL )
    {
        aParseTree->mHostVarList = NULL;
    }
    else
    {
        aParseTree->mHostVarList = sCursor->mHostVarList;
    }
}

YYSTYPE ztqMakeHostVariable( stlParseParam * aParam )
{
    stlInt32   sToken;
    stlChar  * sHostIdentifier = NULL;

    sToken = ztqSqlGetNextToken( aParam, aParam->mScanner );
    STL_TRY( ztqSqlIsIdentifier( sToken,
                                 aParam->mBuffer + aParam->mCurrLoc,
                                 aParam->mLength )
             == STL_TRUE );

    STL_TRY( ztqSqlAllocator( aParam->mContext,
                              aParam->mLength + 1,
                              (void**)&sHostIdentifier,
                              aParam->mErrorStack )
             == STL_SUCCESS );

    stlStrncpy( sHostIdentifier,
                (const stlChar*)aParam->mBuffer + aParam->mCurrLoc,
                aParam->mLength + 1 );

    return (YYSTYPE)sHostIdentifier;

    STL_FINISH;

    aParam->mErrStatus = STL_FAILURE;

    return (YYSTYPE)NULL;
}


stlStatus ztqSqlAllocator( void          * aContext,
                           stlInt32        aAllocSize,
                           void         ** aAddr,
                           stlErrorStack * aErrorStack )
{
    return stlAllocRegionMem( ((ztqSqlParseParam*)aContext)->mAllocator,
                              aAllocSize,
                              aAddr,
                              aErrorStack );
}

stlStatus ztqSqlParseIt( stlChar        * aSqlString,
                         stlAllocator   * aAllocator,
                         ztqParseTree  ** aParseTree,
                         stlErrorStack  * aErrorStack )
{
    stlInt32          sState;
    stlParseParam     sStlParam;
    ztqSqlParseParam  sSqlParam;

    sState = 0;
    sSqlParam.mAllocator = aAllocator;
    sSqlParam.mHostVarList = NULL;

    STL_INIT_PARSE_PARAM( &sStlParam,
                          aSqlString,
                          ztqSqlAllocator,
                          aErrorStack,
                          &sSqlParam );

    sState = 1;
    if( STL_PARSER_PARSE( &sStlParam, sStlParam.mScanner ) != 0 )
    {
        STL_TRY_THROW( sStlParam.mErrStatus == STL_SUCCESS, RAMP_ERR_SYNTAX );
    }

    *aParseTree = (ztqParseTree*)sStlParam.mParseTree;

    sState = 2;
    STL_PARSER_LEX_DESTROY( sStlParam.mScanner );

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SYNTAX )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTQ_ERRCODE_SYNTAX,
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

void ztqRaiseParseError( stlParseParam  *aParam,
                         stlInt32        aPosition )
{
    aParam->mErrStatus = STL_FAILURE;
    aParam->mErrMsg = ztqMakeErrPosString( aParam->mBuffer,
                                           aPosition );

    return;
}

/**
 * @brief Parsing 중에 인위적으로 parsing error를 발생시키고 싶을 때,
 *        해당 SQL 위치에 대한 에러 메시지를 생성한다.
 * @param[in] aSQLString    SQL String
 * @param[in] aPosition     Error Position
 * @remarks qllMakeErrPosString() 함수를 참조했으나, character set 을 고려하지 않음
 */
stlChar * ztqMakeErrPosString( stlChar * aSQLString,
                               stlInt32  aPosition )
{
    stlInt32 sOffset = 0;
    stlInt32 sLineNo = 0;
    stlInt32 sLineStartOffset = 0;
    stlInt32 sLineEndOffset = 0;

    gErrMsgBuffer[0] = '\0';

    while( aSQLString[sOffset] != '\0' )
    {
        if ( sOffset >= aPosition )
        {
            /**
             * 위치를 찾음
             */

            sLineEndOffset = sOffset;
            while ( (aSQLString[sLineEndOffset] != '\n') &&
                    (aSQLString[sLineEndOffset] != '\0') )
            {
                sLineEndOffset ++;
            }

            break;
        }

        if ( aSQLString[sOffset] == '\n' )
        {
            sLineNo++;
            sLineStartOffset = sOffset + 1;
        }

        sOffset ++;
    }

    /**
     * 한 라인의 길이가 너무 길 경우 시작 위치를 조정한다.
     */

    if ( ( (sLineEndOffset - sLineStartOffset) * 2 + 20 ) > STL_MAX_ERROR_MESSAGE )
    {
        sLineStartOffset = sLineEndOffset - (STL_MAX_ERROR_MESSAGE / 2) + 20;
    }
    else
    {
        /* 충분한 길이임 */
    }

    stlSnprintf( gErrMsgBuffer,
                 STL_MAX_ERROR_MESSAGE,
                 "\n" );

    for ( sOffset = sLineStartOffset; sOffset < sLineEndOffset; sOffset++ )
    {
        stlSnprintf( gErrMsgBuffer,
                     STL_MAX_ERROR_MESSAGE,
                     "%s%c",
                     gErrMsgBuffer,
                     aSQLString[sOffset] );
    }

    stlSnprintf( gErrMsgBuffer,
                 STL_MAX_ERROR_MESSAGE,
                 "%s\n",
                 gErrMsgBuffer );

    for ( sOffset = sLineStartOffset; sOffset < aPosition;  )
    {
        stlSnprintf( gErrMsgBuffer,
                     STL_MAX_ERROR_MESSAGE,
                     "%s.",
                     gErrMsgBuffer );
        sOffset ++;
    }
    stlSnprintf( gErrMsgBuffer,
                 STL_MAX_ERROR_MESSAGE,
                 "%s%c\n",
                 gErrMsgBuffer,
                 '^' );

    stlSnprintf( gErrMsgBuffer,
                 STL_MAX_ERROR_MESSAGE,
                 "%sError at line %d",
                 gErrMsgBuffer,
                 sLineNo + 1);

    return gErrMsgBuffer;
}

/** @} */

