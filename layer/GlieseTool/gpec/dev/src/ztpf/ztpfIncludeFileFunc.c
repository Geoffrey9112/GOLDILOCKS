/*******************************************************************************
 * ztpfIncludeFileFunc.c
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

#include <dtl.h>
#include <ztpDef.h>
#include <ztpfIncludeFileFunc.h>

/**
 * @file ztpfIncludeFileFunc.c
 * @brief C Preprocessor Parser Routines
 */

/**
 * @addtogroup ztpfIncludeFileFunc
 * @{
 */

/**
 * @note 이름을 변경할수 없다.
 */
stlInt32 ztpfIncludeFileerror( YYLTYPE                     * aLloc,
                               ztpIncludeFileParseParam    * aParam,
                               void                        * aScanner,
                               const stlChar               * aMsg )
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
     * (에러 위치 메시지 길이 + 에러 라인 메세지 길이)를 정확히 예상할수도 있지만
     * 대략 원본 메세지 길이의 4배보다는 작음을 보장할수 있다.
     */
    sErrMsgLength = (stlStrlen( sStartPtr ) + 1) * 4;

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

    aParam->mErrorStatus = STL_FAILURE;
    aParam->mErrorMsg = sErrMsg;

    return 0;

    STL_FINISH;

    return 0;
}


stlStatus ztpfIncludeFileParseIt( stlAllocator     *aAllocator,
                                  stlErrorStack    *aErrorStack,
                                  stlChar          *aPathStr,
                                  stlInt32         *aPathLen,
                                  stlChar         **aPathDesc )
{
    ztpIncludeFileParseParam  sParam;
    stlInt32                  sMaxLen;

    sMaxLen = stlStrlen( aPathStr );

    sParam.mBuffer             = aPathStr;
    sParam.mAllocator          = aAllocator;
    sParam.mErrorStack         = aErrorStack;
    sParam.mLength             = sMaxLen;
    sParam.mPathLength         = sMaxLen;
    sParam.mPosition           = 0;
    sParam.mLineNo             = 1;
    sParam.mColumn             = 1;
    sParam.mColumnLen          = 0;
    sParam.mErrorMsg           = NULL;
    sParam.mErrorStatus        = STL_SUCCESS;
    sParam.mCurrLoc            = 0;
    sParam.mNextLoc            = 0;

    sParam.mPathDesc            = NULL;

    ztpfIncludeFilelex_init(&sParam.mYyScanner);
    ztpfIncludeFileset_extra(&sParam, sParam.mYyScanner);
    ztpfIncludeFileparse(&sParam, sParam.mYyScanner);
    ztpfIncludeFilelex_destroy(sParam.mYyScanner);

    STL_TRY(sParam.mErrorStatus == STL_SUCCESS);

    *aPathLen   = sParam.mPathLength;
    *aPathDesc  = sParam.mPathDesc;

    return STL_SUCCESS;

    STL_FINISH;

    *aPathLen = sParam.mNextLoc;

    return STL_FAILURE;
}

/** @} */

