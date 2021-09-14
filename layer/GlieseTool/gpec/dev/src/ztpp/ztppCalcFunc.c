/*******************************************************************************
 * ztppCalcFunc.c
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
#include <ztppCommon.h>
#include <ztppCalcFunc.h>
#include <ztppMacro.h>
#include <ztppExpandFile.h>
#include <ztpfPathControl.h>

/**
 * @file ztppCalcFunc.c
 * @brief C Preprocessor Parser Routines
 */

/**
 * @addtogroup ztppCalcFunc
 * @{
 */

ztpCalcResult *ztppMakeCalcResult( ztpCalcParseParam  *aParam,
                                   stlInt64            aValue )
{
    ztpCalcResult  *sCalcResult = NULL;

    STL_TRY(stlAllocRegionMem(aParam->mAllocator,
                              STL_SIZEOF(ztpCalcResult),
                              (void**)&sCalcResult,
                              aParam->mErrorStack)
            == STL_SUCCESS);

    sCalcResult->mValue     = aValue;

    return sCalcResult;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;

    return NULL;
}

ztpPPParameterList *ztppCalcParamList( ztpCalcParseParam   *aParam,
                                       ztpPPParameterList  *aParameterList,
                                       stlInt64             aValue )
{
    ztpPPParameterList   *sParameterList = NULL;

    STL_TRY(stlAllocRegionMem(aParam->mAllocator,
                              STL_SIZEOF(ztpPPParameterList),
                              (void**)&sParameterList,
                              aParam->mErrorStack)
            == STL_SUCCESS);

    sParameterList->mConstExpr = NULL;
    sParameterList->mValue     = aValue;
    sParameterList->mNext      = aParameterList;

    return sParameterList;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;

    return NULL;
}

/**
 * @note 이름을 변경할수 없다.
 */
stlInt32 ztppCalcerror( YYLTYPE            *aLloc,
                        ztpCalcParseParam  *aParam,
                        void               *aScanner,
                        const stlChar      *aMsg )
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

stlStatus ztppCalcParseIt( stlAllocator  *aAllocator,
                           stlErrorStack *aErrorStack,
                           stlChar       *aExprStr,
                           stlInt32       aExprLen,
                           stlInt64      *aResult )
{
    ztpCalcParseParam  sParam;

    sParam.mBuffer             = aExprStr;
    sParam.mAllocator          = aAllocator;
    sParam.mErrorStack         = aErrorStack;
    sParam.mLength             = aExprLen;
    sParam.mExprLength         = aExprLen;
    sParam.mPosition           = 0;
    sParam.mLineNo             = 1;
    sParam.mColumn             = 1;
    sParam.mColumnLen          = 0;
    sParam.mErrorMsg           = NULL;
    sParam.mErrorStatus        = STL_SUCCESS;
    sParam.mCurrLoc            = 0;
    sParam.mNextLoc            = 0;

    ztppCalclex_init(&sParam.mYyScanner);
    ztppCalcset_extra(&sParam, sParam.mYyScanner);
    ztppCalcparse(&sParam, sParam.mYyScanner);
    ztppCalclex_destroy(sParam.mYyScanner);

    STL_TRY(sParam.mErrorStatus == STL_SUCCESS);

    *aResult = sParam.mResult;

    return STL_SUCCESS;

    STL_FINISH;

    *aResult = 0;

    return STL_FAILURE;
}

/** @} */

