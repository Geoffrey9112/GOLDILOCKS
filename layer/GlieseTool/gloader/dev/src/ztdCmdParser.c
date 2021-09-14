/*******************************************************************************
 * ztdCmdParser.c
 *
 * Copyright (c) 2012, SUNJESOFT Inc.
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
#include <ztdDef.h>
#include <ztdCmdParser.h>

/**
 * @brief ztdCmdAllocBuffer()는 Size 만큼 Buffer 할당.
 * @param [in]  aParam  Parse parameter
 * @param [in]  aSize   buffer size
 */
stlChar * ztdCmdAllocBuffer( ztdCmdParseParam * aParam,
                             stlSize            aSize )
{
    stlChar * sBuffer = NULL;
    
    if( stlAllocRegionMem( aParam->mAllocator,
                           aSize,
                           (void**)&sBuffer,
                           aParam->mErrorStack ) != STL_SUCCESS )
    {
        aParam->mErrStatus = STL_FAILURE;
    }

    return sBuffer;
}

/**
 * @brief ztdCmdCheckQualifier()는 Qualifier 조건을 검사
 * @param [in]  aParam  Parse parameter
 * @param [in]  aStr    string
 */
void ztdCmdCheckQualifier( ztdCmdParseParam * aParam,
                           stlChar          * aStr )
{
    if( stlStrlen( aStr ) > 1 )
    {
        aParam->mErrStatus = STL_FAILURE;
    }
}

/**
 * @brief ztdCmderror()는 parsing error 출력
 * @param [in]   aLloc      location
 * @param [in]   aParam     parsing 정보를 담는 struct
 * @param [in]   aScanner   source: yylval
 * @param [in]   aMsg
 */
stlInt32 ztdCmderror( YYLTYPE          * aLloc ,
                      ztdCmdParseParam * aParam,
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

    sErrMsgLength = (stlStrlen( sStartPtr ) + 1 ) * 4;

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
    
    stlStrnncat( sErrMsg, "\n", sErrMsgLength, STL_UINT32_MAX );
    stlStrnncat( sErrMsg, sStartPtr, sErrMsgLength, sEndPtr - sStartPtr + 1 );
    stlStrnncat( sErrMsg, "\n", sErrMsgLength, STL_UINT32_MAX );

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

    stlSnprintf( sErrLineMsgBuffer,
                 128,
                 "Error at line %d",
                 aParam->mLineNo );
    stlStrnncat( sErrMsg, sErrLineMsgBuffer, sErrMsgLength, STL_UINT32_MAX );

    aParam->mErrStatus = STL_FAILURE;
    aParam->mErrMsg = sErrMsg;
    
    return 0;

    STL_FINISH;
    
    return -1;
}

/**
 * @brief ztdCmdParseIt()는 Desc에 yylval을 카피.
 * @param [in]     aBuffer      parsing 되는 string
 * @param [in]     aAllocator   Allocatro
 * @param [out]    aControlInfo Control File을 내용이 저장되는 struct
 * @param [in/out] aErrorStack  error stack
 */
stlStatus ztdCmdParseIt( stlChar        * aBuffer,
                         stlAllocator   * aAllocator,
                         ztdControlInfo * aControlInfo,
                         stlErrorStack  * aErrorStack )
{
    ztdCmdParseParam sCmdParseParam;
    
    sCmdParseParam.mControlInfo = aControlInfo;
    sCmdParseParam.mAllocator   = aAllocator;
    sCmdParseParam.mErrorStack  = aErrorStack;
    sCmdParseParam.mErrStatus   = STL_FALSE;
    sCmdParseParam.mBuffer      = aBuffer;
    sCmdParseParam.mErrMsg      = NULL;

    sCmdParseParam.mLength      = stlStrlen( aBuffer );
    sCmdParseParam.mPosition    = 0;
    sCmdParseParam.mLineNo      = 1;
    sCmdParseParam.mColumn      = 1;
    sCmdParseParam.mColumnLen   = 0;
    sCmdParseParam.mCurPosition = 0;
    sCmdParseParam.mNextPosition = 0;

    ztdCmdlex_init( &sCmdParseParam.mYyScanner );
    ztdCmdset_extra( &sCmdParseParam, sCmdParseParam.mYyScanner );
    ztdCmdparse( &sCmdParseParam, sCmdParseParam.mYyScanner );
    ztdCmdlex_destroy( sCmdParseParam.mYyScanner );
    
    STL_TRY_THROW( sCmdParseParam.mErrStatus == STL_SUCCESS, RAMP_ERR_SYNTAX );

    STL_TRY_THROW( aControlInfo->mTable[0] != '\0', RAMP_ERR_TABLE_DELIMITER );
    STL_TRY_THROW( aControlInfo->mDelimiter != NULL, RAMP_ERR_TABLE_DELIMITER );
    
    if( aControlInfo->mOpQualifier != '\0' )
    {
        STL_TRY_THROW( aControlInfo->mDelimiter[0] != aControlInfo->mOpQualifier,
                       RAMP_ERR_SAME_VALUE );
        STL_TRY_THROW( aControlInfo->mDelimiter[0] != aControlInfo->mClQualifier,
                       RAMP_ERR_SAME_VALUE );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_TABLE_DELIMITER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTD_ERRCODE_INVALID_CONTROL_FILE,
                      "\nomitted table name or delimiter in control file.",
                      aErrorStack );        
    }
    
    STL_CATCH( RAMP_ERR_SYNTAX )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTD_ERRCODE_INVALID_CONTROL_FILE,
                      sCmdParseParam.mErrMsg,
                      aErrorStack );        
    }

    STL_CATCH( RAMP_ERR_SAME_VALUE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTD_ERRCODE_DELIMITER_QUALIFIER,
                      NULL,
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}
