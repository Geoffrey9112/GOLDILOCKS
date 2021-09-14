/*******************************************************************************
 * ztpvCVarFunc.c
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
#include <ztpvCVarFunc.h>
#include <ztppExpandFile.h>
#include <ztpfPathControl.h>

/**
 * @file ztpvCVarFunc.c
 * @brief C Preprocessor Parser Routines
 */

/**
 * @addtogroup ztpvCVarFunc
 * @{
 */

ztpHostVariable *ztpvMakeHostVarNode( ztpCHostVarParseParam  *aParam,
                                      ztpHostVarDesc          aHostVarDesc,
                                      stlChar                *aValue,
                                      ztpHostVariable        *aHostVariable )
{
    ztpHostVariable  *sHostVariable;
    ztpHostVariable  *sHostVariableNode;

    STL_TRY( stlAllocRegionMem(aParam->mAllocator,
                               STL_SIZEOF(ztpHostVariable),
                               (void**)&sHostVariable,
                               aParam->mErrorStack )
             == STL_SUCCESS );

    stlMemset( sHostVariable, 0x00, STL_SIZEOF(ztpHostVariable) );

    sHostVariable->mHostVarDesc = aHostVarDesc;
    sHostVariable->mValue       = aValue;

    sHostVariableNode = aHostVariable;
    if( sHostVariableNode != NULL )
    {
        while( sHostVariableNode->mNext != NULL )
        {
            sHostVariableNode = sHostVariableNode->mNext;
        }
        sHostVariableNode->mNext    = sHostVariable;
        sHostVariable               = aHostVariable;
    }

    return sHostVariable;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return NULL;
}

stlChar *ztpvMakeString( ztpCHostVarParseParam *aParam,
                         stlInt32               aStartPos,
                         stlInt32               aEndPos )
{
    stlChar    *sResultStr;
    stlInt32    sLength;

    sLength = aEndPos - aStartPos;

    STL_TRY(stlAllocRegionMem(aParam->mAllocator,
                              sLength + 1,
                              (void**)&sResultStr,
                              aParam->mErrorStack)
            == STL_SUCCESS);

    stlStrncpy( sResultStr, aParam->mBuffer + aStartPos, sLength + 1);

    return sResultStr;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;
    return NULL;
}

ztpHostVariable *ztpvMakeHostVariable( stlAllocator  *aAllocator,
                                       stlErrorStack *aErrorStack,
                                       stlChar       *aBuffer,
                                       stlInt32       aStartPos,
                                       stlInt32      *aEndPos )
{
    stlChar         *sHostVarString = NULL;
    stlInt32         sHostVarLen = 0;
    stlChar         *sStartString = NULL;
    ztpHostVariable *sHostVariable = NULL;

    /*
     * Host Variable 은 ':' 로 시작하기 때문에,
     * 시작하는 ':' 를 제거하기 위해서 +1 을 한다.
     */
    sStartString = aBuffer + aStartPos + 1;

    STL_TRY( ztpvCVarParseIt( aAllocator,
                              aErrorStack,
                              sStartString,
                              &sHostVarLen,
                              &sHostVariable )
             == STL_SUCCESS );

    while( stlIsspace( *sStartString ) != 0 )
    {
        sHostVarLen --;
        sStartString ++;
    }

    while( stlIsspace( sStartString[sHostVarLen - 1] ) != 0 )
    {
        sHostVarLen --;
    }

    STL_TRY( stlAllocRegionMem( aAllocator,
                                sHostVarLen + 1,
                                (void**)&sHostVarString,
                                aErrorStack )
             == STL_SUCCESS );


    stlStrncpy( sHostVarString,
                sStartString,
                sHostVarLen + 1);

    /*
     * Host variable은 ':'로 시작하기 때문에,
     * 원본 Buffer의 시작 위치에서 +1 을 해 주어야 한다.
     */
    *aEndPos = aStartPos + sHostVarLen + 1;

    sHostVariable->mHostVarString = sHostVarString;

    return sHostVariable;

    STL_FINISH;

    return NULL;
}

/**
 * @note 이름을 변경할수 없다.
 */
stlInt32 ztpvCVarerror( YYLTYPE                 * aLloc,
                        ztpCHostVarParseParam   * aParam,
                        void                    * aScanner,
                        const stlChar           * aMsg )
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

stlStatus ztpvCVarParseIt( stlAllocator     *aAllocator,
                           stlErrorStack    *aErrorStack,
                           stlChar          *aHostVarStr,
                           stlInt32         *aHostVarLen,
                           ztpHostVariable **aHostVariable )
{
    ztpCHostVarParseParam  sParam;
    stlInt32               sMaxLen;

    sMaxLen = stlStrlen( aHostVarStr );

    sParam.mBuffer             = aHostVarStr;
    sParam.mAllocator          = aAllocator;
    sParam.mErrorStack         = aErrorStack;
    sParam.mLength             = sMaxLen;
    sParam.mCHostVarLength     = sMaxLen;
    sParam.mPosition           = 0;
    sParam.mLineNo             = 1;
    sParam.mColumn             = 1;
    sParam.mColumnLen          = 0;
    sParam.mErrorMsg           = NULL;
    sParam.mErrorStatus        = STL_SUCCESS;
    sParam.mCurrLoc            = 0;
    sParam.mNextLoc            = 0;

    sParam.mHostVar            = NULL;

    ztpvCVarlex_init(&sParam.mYyScanner);
    ztpvCVarset_extra(&sParam, sParam.mYyScanner);
    ztpvCVarparse(&sParam, sParam.mYyScanner);
    ztpvCVarlex_destroy(sParam.mYyScanner);

    STL_TRY(sParam.mErrorStatus == STL_SUCCESS);

    *aHostVarLen   = sParam.mCurrLoc;
    *aHostVariable = sParam.mHostVar;

    return STL_SUCCESS;

    STL_FINISH;

    *aHostVarLen = sParam.mNextLoc;

    return STL_FAILURE;
}

/** @} */

