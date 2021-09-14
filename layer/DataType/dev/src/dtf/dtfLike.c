/*******************************************************************************
 * dtfLike.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *        
 * NOTES
 * 
 ******************************************************************************/

/**
 * @file dtfLike.c
 * @brief Like Function DataType Library Layer
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtsCharacterSet.h>
#include <dtfString.h>

/*******************************************************************************
 *   Like
 *   Like ==> [ P, O, M, S ]
 * ex) 'string' Like 'strin_'  = true
 *******************************************************************************/

static stlStatus dtfLikeMakePattern( dtlAllocInfo        *aAllocInfo,
                                     stlChar             *aString,
                                     stlInt32             aStringLen,
                                     stlBool             *aEscapeList,
                                     dtlCharacterSet      aCharSetID,
                                     void                *aEnv,
                                     dtlPattern         **aResultPattern );

static stlStatus dtfLikeMatchAll( dtlPatternInfo *aInfo,
                                  stlChar        *aString,
                                  stlInt32        aStringLen,
                                  dtlCharacterSet aCharSetId,
                                  stlErrorStack  *aErrorStack,
                                  stlBool        *aResult );

static stlStatus dtfLikeMatchConst( dtlPatternInfo *aInfo,
                                    stlChar        *aString,
                                    stlInt32        aStringLen,
                                    dtlCharacterSet aCharSetId,
                                    stlErrorStack  *aErrorStack,
                                    stlBool        *aResult );

static stlStatus dtfLikeGeneralBinaryMatch( dtlPatternInfo *aInfo,
                                            stlChar        *aString,
                                            stlInt32        aStringLen,
                                            dtlCharacterSet aCharSetId,
                                            stlErrorStack  *aErrorStack,
                                            stlBool        *aResult );

static stlStatus dtfLikeGeneralCharMatch( dtlPatternInfo *aInfo,
                                          stlChar        *aString,
                                          stlInt32        aStringLen,
                                          dtlCharacterSet aCharSetId,
                                          stlErrorStack  *aErrorStack,
                                          stlBool        *aResult );

/**
 * @brief Like Pattern
 *        <BR> expr Like expr
 *        <BR>   Like => [ P, O, M, S ]
 *        <BR> ex) 'gliese' Like 'glies_'  =>  true
 *        
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> STRING LIKE STRING [ ESCAPE 'STRING']
 * @param[out] aResultValue      연산 결과 (결과타입 BOOLEAN)
 * @param[in]  aInfo             pattern에 대한 자료구조
 * @param[in]  aVectorFunc       Function Vectorc
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfLikePattern( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aInfo,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv )
{
    stlInt32           i;
    dtlDataValue     * sValue1;
    dtlDataValue     * sValue2;
    dtlVarcharType     sPattern;
    dtlVarcharType     sPatternBegin;
    dtlVarcharType     sPatternSave;
    dtlVarcharType     sPatternLast;
    dtlVarcharType     sPatternPos;
    stlInt32           sPatternLen;
    dtlCharacterSet    sCharSetID;
    stlBool            sWildStart;
    dtlAllocInfo     * sAllocInfo = (dtlAllocInfo *)aInfo;
    dtlPatternInfo   * sPatternInfo;
    stlBool          * sEscapeList;
    stlBool          * sEscapePos;
    stlChar          * sEscapeStr;
    stlInt32           sEscapeLen;
    stlInt64           sEscapeCharLen;
    stlInt32           sPatternCount;
    stlInt8            sMbLen;
    stlBool            sHasUnderscore;
    stlBool            sHasFollowingPattern;
    dtlDataValue     * sResultValue = (dtlDataValue *)aResultValue;
    stlErrorStack    * sErrorStack = (stlErrorStack *)aEnv;

    STL_RAMP( RAMP_RETRY );

    DTL_ALLOC_INFO_INIT( sAllocInfo, (stlChar *)DTF_BINARY( sResultValue ), sResultValue->mBufferSize );
    stlMemset( sResultValue->mValue, 0x00, sResultValue->mBufferSize );

    sHasUnderscore = STL_FALSE;
    sValue1        = aInputArgument[0].mValue.mDataValue;
    sCharSetID     = aVectorFunc->mGetCharSetIDFunc( aVectorArg );
    sPattern       = DTF_VARCHAR( sValue1 );
    sPatternLen    = sValue1->mLength;

    STL_TRY_THROW( dtlAllocMem( sAllocInfo,
                                STL_SIZEOF( dtlPatternInfo ) ,
                                (void **) &sPatternInfo ) == STL_SUCCESS, MORE_MEMORY_NEED );

    STL_TRY_THROW( dtlAllocMem( sAllocInfo,
                                sPatternLen,
                                (void **) &sPatternInfo->mPatternString ) == STL_SUCCESS, MORE_MEMORY_NEED );

    STL_TRY_THROW( dtlAllocMem( sAllocInfo,
                                sPatternLen,
                                (void **) &sEscapeList ) == STL_SUCCESS, MORE_MEMORY_NEED );

    stlMemcpy( sPatternInfo->mPatternString,
               sPattern,
               sPatternLen );

    stlMemset( sEscapeList, STL_FALSE, sPatternLen );

    sPattern = sPatternInfo->mPatternString;
    sPatternBegin = sPattern;
    sPatternLast = sPattern + sPatternLen - 1;

    if( aInputArgumentCnt > 1 )
    {
        sEscapePos = sEscapeList;
        sValue2 = aInputArgument[1].mValue.mDataValue;
        
        STL_TRY( dtsGetMbstrlenWithLenFuncList[ sCharSetID ]( DTF_VARCHAR( sValue2 ),
                                                              sValue2->mLength,
                                                              &sEscapeCharLen,
                                                              aVectorFunc,
                                                              aVectorArg,
                                                              sErrorStack )
                 == STL_SUCCESS );
        
        STL_TRY_THROW( sEscapeCharLen == 1, ERROR_INVALID_ESCAPE_CHARACTER );
        
        sEscapeStr = DTF_VARCHAR( sValue2 );
        sEscapeLen = sValue2->mLength;

        while( sPattern <= sPatternLast )
        {
            if( ( *sPattern == *sEscapeStr ) && ( sPatternLast - sPattern + 1 >= sEscapeLen ) )
            {
                if( stlMemcmp( sPattern,
                               sEscapeStr,
                               sEscapeLen ) == 0 )
                {
                    if( sPattern + sEscapeLen > sPatternLast )
                    {
                        STL_THROW( ERROR_INVALID_LIKE_PATTERN );
                    }

                    if( ( *(sPattern+sEscapeLen) != '%' ) && ( *(sPattern+sEscapeLen) != '_' ) )
                    {
                        if( sPattern + sEscapeLen + sEscapeLen - 1 > sPatternLast )
                        { 
                            STL_THROW( ERROR_INVALID_LIKE_PATTERN );
                        }
                        else
                        {
                            if( stlMemcmp( sPattern + sEscapeLen,
                                           sEscapeStr,
                                           sEscapeLen ) == 0 )
                            {
                                /* nothing to do */
                            }
                            else
                            {
                                STL_THROW( ERROR_INVALID_LIKE_PATTERN );
                            }
                        }
                    }

                    sPatternSave = sPattern;
                    while( sPattern <= sPatternLast - sEscapeLen )
                    {
                        *sPattern = *(sPattern+sEscapeLen);
                        sPattern++;
                    }

                    sPattern = sPatternSave;
                    *sEscapePos = STL_TRUE;

                    sPatternLast -= sEscapeLen;
                }
            }

            STL_TRY( dtsGetMbLengthFuncList[sCharSetID]( sPattern,
                                                         &sMbLen,
                                                         sErrorStack )
                     == STL_SUCCESS );

            sPattern += sMbLen;
            sEscapePos += sMbLen;
        }

        sPattern = sPatternBegin;
    }

    /**
     * STEP-1: Leading pattern을 분석한다.
     */
    if( DTF_LIKE_PATTERN_IS_WILDCARD( sPatternBegin, sPattern, sEscapeList ) == STL_FALSE )
    {
        /* leading pattern이 존재 */
        sPatternSave = sPattern;

        STL_TRY( dtsGetMbLengthFuncList[sCharSetID]( sPattern,
                                                     &sMbLen,
                                                     sErrorStack )
                 == STL_SUCCESS );

        sPattern += sMbLen;

        while( ( sPattern <= sPatternLast ) &&
               ( DTF_LIKE_PATTERN_IS_WILDCARD( sPatternBegin, sPattern, sEscapeList ) == STL_FALSE ) )
        {
            STL_TRY( dtsGetMbLengthFuncList[sCharSetID]( sPattern,
                                                         &sMbLen,
                                                         sErrorStack )
                     == STL_SUCCESS );

            sPattern += sMbLen;
        }

        if( sPattern > sPatternLast )
        {
            /* 이 코드는 pattern string이 잘못되어 멀티바이트 코드로 시작했으나
               다음 추가 바이트가 없는 경우 서버가 잘못된 메모리 영역을 읽는 것을 
               막기 위한 것이다. 멀티바이트 캐릭터 이동할 때 항상 이런식으로 방어를 해야 한다. */
            sPattern = sPatternLast + 1;
        }

        STL_TRY( dtfLikeMakePattern( sAllocInfo,
                                     sPatternSave,
                                     sPattern - sPatternSave,
                                     sEscapeList + ( sPatternSave - sPatternBegin ),
                                     sCharSetID,
                                     aEnv,
                                     &sPatternInfo->mLeadingPattern ) == STL_SUCCESS );

        STL_TRY_THROW( sPatternInfo->mLeadingPattern != NULL, MORE_MEMORY_NEED );

        if( DTF_PATTERN_HAS_UNDERSCORE( sPatternInfo->mLeadingPattern ) == STL_TRUE )
        {
            sHasUnderscore = STL_TRUE;
        }
    }
    else
    {
        sPatternInfo->mLeadingPattern = NULL;
    }

    /**
     * STEP-2: Leading pattern을 제외한 나머지 부분에서 %로 구분된 패턴의 개수를 샌다.
     */
    sPatternCount = 0;
    sPatternSave = sPattern;

    sWildStart = STL_FALSE;
    while( sPattern <= sPatternLast )
    {
        if( DTF_LIKE_PATTERN_IS_WILDCARD( sPatternBegin, sPattern, sEscapeList ) == STL_TRUE )
        {
            sWildStart = STL_TRUE;
        }
        else
        {
            if( sWildStart == STL_TRUE )
            {
                sPatternCount++;
                sWildStart = STL_FALSE;
            }
        }

        STL_TRY( dtsGetMbLengthFuncList[sCharSetID]( sPattern,
                                                     &sMbLen,
                                                     sErrorStack )
                 == STL_SUCCESS );

        sPattern += sMbLen;
    }

    sPattern = sPatternSave;

    /**
     * STEP-3: 패턴 개수중에 맨 마지막 패턴은 Following pattern이 되고 나머지는
     *         mPatterns를 이룬다. 여기서 mPatterns를 구성한다.
     */
    sPatternInfo->mPatterns = NULL;
    sPatternInfo->mPatternCount = 0;
    sPatternInfo->mIsConstPattern = STL_FALSE;
    sPatternInfo->mIsBinComparable = dtsCharsetBinaryComparable[ sCharSetID ];
    if( sWildStart == STL_TRUE )
    {
        /* %로 끝나는 경우이다. 이 경우엔 Following pattern이 없다. */
        sPatternInfo->mPatternCount = sPatternCount;
        sHasFollowingPattern = STL_FALSE;
    }
    else
    {
        /* pattern의 맨 끝이 %가 아니다. */
        if( sPatternCount == 0 )
        {
            /* const pattern이다. leading pattern만 있고 아무것도 없다. */
            sPatternInfo->mIsConstPattern = STL_TRUE;
        }
        else
        {
            sPatternInfo->mPatternCount = sPatternCount - 1;
        }

        sHasFollowingPattern = STL_TRUE;
    }

    if( sPatternInfo->mPatternCount > 0 )
    {
        STL_TRY_THROW( dtlAllocMem( sAllocInfo,
                                    STL_SIZEOF( dtlPattern* ) * sPatternInfo->mPatternCount,
                                    (void **) &sPatternInfo->mPatterns ) == STL_SUCCESS, MORE_MEMORY_NEED );
    }

    for( i = 0; i < sPatternInfo->mPatternCount; i++ )
    {
        while( DTF_LIKE_PATTERN_IS_WILDCARD( sPatternBegin, sPattern, sEscapeList ) == STL_TRUE )
        {
            /* 현재 % 문자이므로 멀티바이트 캐릭터 이동할 필요없이 1씩 이동해도 된다. */
            sPattern++;
        }

        sPatternPos = sPattern;

        while( DTF_LIKE_PATTERN_IS_WILDCARD( sPatternBegin, sPattern, sEscapeList ) == STL_FALSE )
        {
            STL_TRY( dtsGetMbLengthFuncList[sCharSetID]( sPattern,
                                                         &sMbLen,
                                                         sErrorStack )
                     == STL_SUCCESS );

            sPattern += sMbLen;

            /* 반드시 %를 만나게 되어 있으므로 sPattern의 종료 조건을 따로 둘 필요가 없다. */
        }

        STL_TRY( dtfLikeMakePattern( sAllocInfo,
                                     sPatternPos,
                                     sPattern - sPatternPos,
                                     sEscapeList + ( sPatternPos - sPatternBegin ),
                                     sCharSetID,
                                     aEnv,
                                     &sPatternInfo->mPatterns[i] ) == STL_SUCCESS );

        STL_TRY_THROW( sPatternInfo->mPatterns[i] != NULL, MORE_MEMORY_NEED );

        if( DTF_PATTERN_HAS_UNDERSCORE( sPatternInfo->mPatterns[i] ) == STL_TRUE )
        {
            sHasUnderscore = STL_TRUE;
        }

        STL_DASSERT( sPattern <= sPatternLast );
    }

    /**
     * STEP-4: Following pattern을 구성한다.
     */
    if( ( sHasFollowingPattern == STL_TRUE ) && ( sPattern < sPatternLast ) )
    {
        /* following pattern이 존재 */

        STL_DASSERT( *sPattern == '%' );

        /* %를 모두 건너뛴다. */
        while( DTF_LIKE_PATTERN_IS_WILDCARD( sPatternBegin, sPattern, sEscapeList ) == STL_TRUE )
        {
            sPattern++;
        }

        STL_TRY( dtfLikeMakePattern( sAllocInfo,
                                     sPattern,
                                     sPatternLast - sPattern + 1,
                                     sEscapeList + ( sPattern - sPatternBegin ),
                                     sCharSetID,
                                     aEnv,
                                     &sPatternInfo->mFollowingPattern ) == STL_SUCCESS );

        STL_TRY_THROW( sPatternInfo->mFollowingPattern != NULL, MORE_MEMORY_NEED );

        if( DTF_PATTERN_HAS_UNDERSCORE( sPatternInfo->mFollowingPattern ) == STL_TRUE )
        {
            sHasUnderscore = STL_TRUE;
        }
    }
    else
    {
        sPatternInfo->mFollowingPattern = NULL;
    }

    /**
     * STEP-5:  pattern의 유형 분석 
     *  - 1. %                                 : %만 하나 있는 경우
     *  - 2. abc                               : const 상수
     *  - 3. 1, 2가 아니면서 _가 없고 binary_comparable_charset인 경우 : %abc%acd, ...
     *  - 4. 1, 2, 3이 아닌 경우, 즉 _가 포함되었거나 character_comparable_charset(UHC, MS-949, Shift-JIS, ...)
     */
    if( ( sPatternInfo->mLeadingPattern == NULL ) &&
        ( sPatternInfo->mFollowingPattern == NULL ) &&
        ( sPatternInfo->mPatterns == NULL ) )
    {
        /* 유형 1 */
        sPatternInfo->mLikeFunc = dtfLikeMatchAll;
    }
    else if( ( sPatternInfo->mLeadingPattern != NULL ) &&
             ( sPatternInfo->mFollowingPattern == NULL ) &&
             ( sPatternInfo->mPatterns == NULL ) )
    {
        if( sPatternInfo->mIsConstPattern == STL_TRUE )
        {
            if( sHasUnderscore == STL_FALSE )
            {
                /* 유형 2 */
                sPatternInfo->mLikeFunc = dtfLikeMatchConst;
            }
            else
            {
                /* 유형 4: _가 하나라도 포함된 경우 */
                sPatternInfo->mLikeFunc = dtfLikeGeneralCharMatch;
            }
        }
        else
        {
            if( sHasUnderscore == STL_FALSE )
            {
                /* 유형 3 */
                sPatternInfo->mLikeFunc = dtfLikeGeneralBinaryMatch;
            }
            else
            {
                /* 유형 4: _가 하나라도 포함된 경우 */
                sPatternInfo->mLikeFunc = dtfLikeGeneralCharMatch;
            }
        }
    }
    else
    {
        if( ( sPatternInfo->mIsBinComparable == STL_TRUE ) &&
            ( sHasUnderscore == STL_FALSE ) )
        {
            /* 유형 3 */
            sPatternInfo->mLikeFunc = dtfLikeGeneralBinaryMatch;
        }
        else
        {
            /* 유형 4 */
            sPatternInfo->mLikeFunc = dtfLikeGeneralCharMatch;
        }
    }

    ((dtlDataValue *)aResultValue)->mLength = DTL_ALLOC_INFO_USED_SIZE( sAllocInfo );

    return STL_SUCCESS;

    STL_CATCH( MORE_MEMORY_NEED )
    {
        sResultValue->mBufferSize *= 2;

        STL_TRY_THROW( sResultValue->mBufferSize <= DTL_LONGVARBINARY_MAX_PRECISION,
                       ERROR_OUT_OF_RANGE );

        STL_TRY( aVectorFunc->mReallocLongVaryingMemFunc( aVectorArg,
                                                          sResultValue->mBufferSize,
                                                          &sResultValue->mValue,
                                                          sErrorStack )
                 == STL_SUCCESS );

        DTL_ALLOC_INFO_SET_BUF_SIZE( sAllocInfo, sResultValue->mBufferSize );

        STL_THROW( RAMP_RETRY );
    }

    STL_CATCH( ERROR_INVALID_LIKE_PATTERN )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_LIKE_PATTERN,
                      NULL,
                      sErrorStack );
    }

    STL_CATCH( ERROR_INVALID_ESCAPE_CHARACTER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_ESCAPE_CHARACTER,
                      NULL,
                      sErrorStack );
    }
   
    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_BINARY_BYTE_LENGTH_GREATER_THAN_COLUMN_LENGTH,
                      NULL,
                      sErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Like 
 *        <BR> expr Like expr
 *        <BR>   Like => [ P, O, M, S ]
 *        <BR> ex) 'gliese' Like 'glies_'  =>  true
 *        
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> STRING LIKE STRING [ ESCAPE 'STRING']
 * @param[out] aResultValue      연산 결과 (결과타입 BOOLEAN)
 * @param[in]  aInfo             pattern에 대한 자료구조
 * @param[in]  aVectorFunc       Function Vectorc
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfLike( stlUInt16        aInputArgumentCnt,
                   dtlValueEntry  * aInputArgument,
                   void           * aResultValue,
                   void           * aInfo,
                   dtlFuncVector  * aVectorFunc,
                   void           * aVectorArg,
                   void           * aEnv )
{
    dtlDataValue     * sValue1;
    dtlDataValue     * sValue2;
    dtlVarcharType     sString;
    stlInt32           sStringLen;
    dtlCharacterSet    sCharSetID;
    stlErrorStack    * sErrorStack = (stlErrorStack *)aEnv;
    dtlPatternInfo   * sPatternInfo;
    dtlBooleanType     sResultValue = STL_FALSE;

    sValue1        = aInputArgument[0].mValue.mDataValue;
    sValue2        = aInputArgument[1].mValue.mDataValue;

    sCharSetID     = aVectorFunc->mGetCharSetIDFunc( aVectorArg );
    sString        = DTF_VARCHAR( sValue1 );
    sStringLen     = sValue1->mLength;
    sPatternInfo   = (dtlPatternInfo *)(DTF_BINARY( sValue2 ));

    STL_TRY( sPatternInfo->mLikeFunc( sPatternInfo,
                                      sString,
                                      sStringLen,
                                      sCharSetID,
                                      sErrorStack,
                                      &sResultValue ) == STL_SUCCESS );

    DTL_BOOLEAN( aResultValue ) = sResultValue;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

static stlStatus dtfLikeMakePattern( dtlAllocInfo        *aAllocInfo,
                                     stlChar             *aString,
                                     stlInt32             aStringLen,
                                     stlBool             *aEscapeList,
                                     dtlCharacterSet      aCharSetID,
                                     void                *aEnv,
                                     dtlPattern         **aResultPattern )
{
    stlInt32           i;
    stlInt32           sAtomPatternIndex;
    stlInt32           sUnderscoreCount;
    stlChar          * sString = aString;
    dtlPattern       * sPattern;
    stlErrorStack    * sErrorStack = (stlErrorStack *)aEnv;
    stlBool            sPatternStart = STL_FALSE;
    stlInt8            sMbLen;

    STL_TRY_THROW( dtlAllocMem( aAllocInfo,
                                STL_SIZEOF( dtlPattern ),
                                (void **) &sPattern ) == STL_SUCCESS, MORE_MEMORY_NEED );

    sPattern->mAtomPatternCount = 0;
    sPattern->mPatternLen = aStringLen;

    for( i = 0; i < aStringLen; )
    {
        if( DTF_LIKE_PATTERN_IS_UNDERSCORE( aString, sString, aEscapeList ) == STL_TRUE )
        {
            sPatternStart = STL_FALSE;
        }
        else
        {
            if( sPatternStart == STL_FALSE )
            {
                sPattern->mAtomPatternCount++;
                sPatternStart = STL_TRUE;
            }
        }

        STL_TRY( dtsGetMbLengthFuncList[aCharSetID]( sString,
                                                     &sMbLen,
                                                     sErrorStack )
                 == STL_SUCCESS );

        sString += sMbLen;
        i += sMbLen;
    }
    if( sPatternStart == STL_FALSE )
    {
        /* pattern이 '_'로 끝날 경우 atom pattern을 하나 추가한다.
           이 경우 atom pattern의 len은 0이 되어 memcmp시 항상 true가 리턴된다. */
        sPattern->mAtomPatternCount++;
    }

    STL_TRY_THROW( dtlAllocMem( aAllocInfo,
                                STL_SIZEOF( stlChar * ) * sPattern->mAtomPatternCount,
                                (void **) &sPattern->mAtomPatterns ) == STL_SUCCESS, MORE_MEMORY_NEED );

    STL_TRY_THROW( dtlAllocMem( aAllocInfo,
                                STL_SIZEOF( stlInt32 ) * sPattern->mAtomPatternCount,
                                (void **) &sPattern->mUnderscoreCount ) == STL_SUCCESS, MORE_MEMORY_NEED );

    STL_TRY_THROW( dtlAllocMem( aAllocInfo,
                                STL_SIZEOF( stlInt32 ) * sPattern->mAtomPatternCount,
                                (void **) &sPattern->mAtomPatternLen ) == STL_SUCCESS, MORE_MEMORY_NEED );

    sUnderscoreCount = 0;
    sPatternStart = STL_FALSE;
    sString = aString;
    sAtomPatternIndex = -1;

    for( i = 0; i < aStringLen; )
    {
        STL_TRY( dtsGetMbLengthFuncList[aCharSetID]( sString,
                                                     &sMbLen,
                                                     sErrorStack )
                 == STL_SUCCESS );

        if( DTF_LIKE_PATTERN_IS_UNDERSCORE( aString, sString, aEscapeList ) == STL_TRUE )
        {
            if( sPatternStart == STL_TRUE )
            {
                sUnderscoreCount = 1;
                sPatternStart = STL_FALSE;
            }
            else
            {
                sUnderscoreCount++;
            }
        }
        else
        {
            if( sPatternStart == STL_FALSE )
            {
                sAtomPatternIndex++;
                sPattern->mAtomPatterns[sAtomPatternIndex] = sString;
                sPattern->mUnderscoreCount[sAtomPatternIndex] = sUnderscoreCount;
                sPattern->mAtomPatternLen[sAtomPatternIndex] = sMbLen;
                sPatternStart = STL_TRUE;
            }
            else
            {
                sPattern->mAtomPatternLen[sAtomPatternIndex] += sMbLen;
            }
        }

        sString += sMbLen;
        i += sMbLen;
    }
    if( sPatternStart == STL_FALSE )
    {
        sAtomPatternIndex++;
        sPattern->mAtomPatterns[sAtomPatternIndex] = sString;
        sPattern->mUnderscoreCount[sAtomPatternIndex] = sUnderscoreCount;
        sPattern->mAtomPatternLen[sAtomPatternIndex] = 0;
    }

    *aResultPattern = sPattern;

    return STL_SUCCESS;

    STL_CATCH( MORE_MEMORY_NEED )
    {
        *aResultPattern = NULL;
        return STL_SUCCESS;
    }

    STL_FINISH;

    return STL_FAILURE;
}

static stlStatus dtfLikeMatchAll( dtlPatternInfo *aInfo,
                                  stlChar        *aString,
                                  stlInt32        aStringLen,
                                  dtlCharacterSet aCharSetId,
                                  stlErrorStack  *aErrorStack,
                                  stlBool        *aResult )
{
    /* pattern이 '%'인 경우 */
    *aResult = STL_TRUE;

    return STL_SUCCESS;
}

static stlStatus dtfLikeMatchConst( dtlPatternInfo *aInfo,
                                    stlChar        *aString,
                                    stlInt32        aStringLen,
                                    dtlCharacterSet aCharSetId,
                                    stlErrorStack  *aErrorStack,
                                    stlBool        *aResult )
{
    /* pattern이 'abc' 같이 %나 _가 없는 상수인 경우 */
    if( aStringLen != aInfo->mLeadingPattern->mPatternLen )
    {
        *aResult = STL_FALSE;
    }
    else if( stlMemcmp( aString,
                        aInfo->mLeadingPattern->mAtomPatterns[0],
                        aStringLen ) == 0 )
    {
        *aResult = STL_TRUE;
    }
    else
    {
        *aResult = STL_FALSE;
    }

    return STL_SUCCESS;
}

static stlStatus dtfLikeGeneralBinaryMatch( dtlPatternInfo *aInfo,
                                            stlChar        *aString,
                                            stlInt32        aStringLen,
                                            dtlCharacterSet aCharSetId,
                                            stlErrorStack  *aErrorStack,
                                            stlBool        *aResult )
{
    /* 이 함수에서는 _를 고려할 필요가 없다. */
    dtlPattern  *sPattern;
    stlInt32     sPatternLen;
    stlInt32     i;
    stlChar     *sString;
    stlBool      sMatched = STL_FALSE;
    stlChar      sStartChar;

    *aResult = STL_FALSE;

    if( aInfo->mLeadingPattern != NULL )
    {
        sPattern = aInfo->mLeadingPattern;
        sPatternLen = sPattern->mPatternLen;

        if( aStringLen < sPatternLen )
        {
            STL_THROW( MATCH_FINISHED );
        }

        if( *aString != *sPattern->mAtomPatterns[0] )
        {
            STL_THROW( MATCH_FINISHED );
        }

        if( stlMemcmp( aString,
                       sPattern->mAtomPatterns[0],
                       sPatternLen ) != 0 )
        {
            STL_THROW( MATCH_FINISHED );
        }

        aString += sPatternLen;
        aStringLen -= sPatternLen;
    }

    if( aInfo->mFollowingPattern != NULL )
    {
        sPattern = aInfo->mFollowingPattern;
        sPatternLen = sPattern->mPatternLen;

        if( aStringLen < sPatternLen )
        {
            STL_THROW( MATCH_FINISHED );
        }

        sString = aString + aStringLen - sPatternLen;
        aStringLen -= sPatternLen;

        if( *sString != *sPattern->mAtomPatterns[0] )
        {
            STL_THROW( MATCH_FINISHED );
        }

        if( stlMemcmp( sString,
                       sPattern->mAtomPatterns[0],
                       sPatternLen ) != 0 )
        {
            STL_THROW( MATCH_FINISHED );
        }
    }

    for( i = 0; i < aInfo->mPatternCount; i++ )
    {
        sPattern = aInfo->mPatterns[i];
        sMatched = STL_FALSE;
        sStartChar = *sPattern->mAtomPatterns[0];
        while( aStringLen >= sPattern->mPatternLen )
        {
            /* 문자 비교 성능을 위해 첫 문자가 같을 때 stlMemcmp를 사용한다. */
            if( *aString == sStartChar )
            {
                if( stlMemcmp( aString,
                               sPattern->mAtomPatterns[0],
                               sPattern->mPatternLen ) == 0 )
                {
                    aString += sPattern->mPatternLen;
                    aStringLen -= sPattern->mPatternLen;
                    sMatched = STL_TRUE;
                    break;
                }
            }

            aString++;
            aStringLen--;
        }

        if( sMatched == STL_FALSE )
        {
            STL_THROW( MATCH_FINISHED );
        }
    }

    *aResult = STL_TRUE;

    STL_RAMP( MATCH_FINISHED );

    return STL_SUCCESS;
}

static stlStatus dtfLikeGeneralCharMatch( dtlPatternInfo *aInfo,
                                          stlChar        *aString,
                                          stlInt32        aStringLen,
                                          dtlCharacterSet aCharSetId,
                                          stlErrorStack  *aErrorStack,
                                          stlBool        *aResult )
{
    dtlPattern  *sPattern;
    stlInt8      sStrMbLen;
    stlInt32     i;
    stlInt32     j;
    stlInt32     k;
    stlInt32     sPatternLen;
    stlChar     *sString;
    stlInt32     sStringLen;
    stlBool      sMatched;
    stlChar     *sPatternStr;
    stlInt32     sUnderscoreCount;

    *aResult = STL_FALSE;

    if( aInfo->mLeadingPattern != NULL )
    {
        sPattern = aInfo->mLeadingPattern;
 
        for( i = 0; i < sPattern->mAtomPatternCount; i++ )
        {
            for( j = 0; j < sPattern->mUnderscoreCount[i]; j++ )
            {
                if( aStringLen <= 0 )
                {
                    STL_THROW( MATCH_FINISHED );
                }

                STL_TRY( dtsGetMbLengthFuncList[aCharSetId]( aString,
                                                             &sStrMbLen,
                                                             aErrorStack )
                         == STL_SUCCESS );

                if( aStringLen < sStrMbLen )
                {
                    STL_THROW( MATCH_FINISHED );
                }

                aString += sStrMbLen;
                aStringLen -= sStrMbLen;
            }

            sPatternLen = sPattern->mAtomPatternLen[i];
            if( aStringLen < sPatternLen )
            {
                STL_THROW( MATCH_FINISHED );
            }

            if( stlMemcmp( aString,
                           sPattern->mAtomPatterns[i],
                           sPatternLen ) != 0 )
            {
                STL_THROW( MATCH_FINISHED );
            }

            aString += sPatternLen;
            aStringLen -= sPatternLen;
        }

        if( aInfo->mIsConstPattern == STL_TRUE )
        {
            if( aStringLen == 0 )
            {
                *aResult = STL_TRUE;
            }

            STL_THROW( MATCH_FINISHED );
        }
    }

    for( k = 0; k < aInfo->mPatternCount; k++ )
    {
        sPattern = aInfo->mPatterns[k];
        sMatched = STL_FALSE;

        i = 0;
        sPatternStr = sPattern->mAtomPatterns[i];
        sPatternLen = sPattern->mAtomPatternLen[i];
        sUnderscoreCount = sPattern->mUnderscoreCount[i];
        sString = aString;
        sStringLen = aStringLen;

        while( sStringLen >= sPatternLen )
        {
            if( sUnderscoreCount > 0 )
            {
                for( j = 0; j < sPattern->mUnderscoreCount[i]; j++ )
                {
                    if( sStringLen <= 0 )
                    {
                        STL_THROW( MATCH_FINISHED );
                    }

                    STL_TRY( dtsGetMbLengthFuncList[aCharSetId]( sString,
                                                                 &sStrMbLen,
                                                                 aErrorStack )
                             == STL_SUCCESS );

                    if( sStringLen < sStrMbLen )
                    {
                        STL_THROW( MATCH_FINISHED );
                    }

                    sString += sStrMbLen;
                    sStringLen -= sStrMbLen;
                }

                if( sPatternLen == 0 )
                {
                    STL_THROW( RAMP_NEXT_ATOM_PATTERN );
                }

                if( sStringLen < sPatternLen )
                {
                    STL_THROW( MATCH_FINISHED );
                }
            }

            if( *sString != *sPatternStr )
            {
                // 성능을 위해 stlMemcmp()를 호출하기 전에 byte 검사를 먼저한다. */
                STL_THROW( RAMP_NEXT_CHARACTER );
            }
            if( stlMemcmp( sString + 1,
                           sPatternStr + 1,
                           sPatternLen - 1 ) != 0 )
            {
                STL_THROW( RAMP_NEXT_CHARACTER );
            }

            STL_RAMP( RAMP_NEXT_ATOM_PATTERN )
            {
                sString += sPatternLen;
                sStringLen -= sPatternLen;

                if( i == sPattern->mAtomPatternCount - 1 )
                {
                    aString = sString;
                    aStringLen = sStringLen;
                    sMatched = STL_TRUE;
                    break;
                }
                i++;
                sPatternStr = sPattern->mAtomPatterns[i];
                sPatternLen = sPattern->mAtomPatternLen[i];
                sUnderscoreCount = sPattern->mUnderscoreCount[i];
                continue;
            }
            STL_RAMP( RAMP_NEXT_CHARACTER )
            {
                if( i > 0 )
                {
                    i = 0;
                    sPatternStr = sPattern->mAtomPatterns[i];
                    sPatternLen = sPattern->mAtomPatternLen[i];
                    sUnderscoreCount = sPattern->mUnderscoreCount[i];
                }

                STL_TRY( dtsGetMbLengthFuncList[aCharSetId]( aString,
                                                             &sStrMbLen,
                                                             aErrorStack )
                         == STL_SUCCESS );

                aString += sStrMbLen;
                aStringLen -= sStrMbLen;
                sString = aString;
                sStringLen = aStringLen;
            }
        }

        if( sMatched == STL_FALSE )
        {
            STL_THROW( MATCH_FINISHED );
        }
    }

    if( aInfo->mFollowingPattern != NULL )
    {
        sPattern = aInfo->mFollowingPattern;

        while( aStringLen >= sPattern->mPatternLen )
        {
            sString = aString;
            sStringLen = aStringLen;

            for( i = 0; i < sPattern->mAtomPatternCount; i++ )
            {
                for( j = 0; j < sPattern->mUnderscoreCount[i]; j++ )
                {
                    if( sStringLen <= 0 ) 
                    {
                        STL_THROW( MATCH_FINISHED );
                    }

                    STL_TRY( dtsGetMbLengthFuncList[aCharSetId]( sString,
                                                                 &sStrMbLen,
                                                                 aErrorStack )
                             == STL_SUCCESS );

                    if( sStringLen < sStrMbLen )
                    {
                        STL_THROW( MATCH_FINISHED );
                    }

                    sString += sStrMbLen;
                    sStringLen -= sStrMbLen;
                }

                sPatternLen = sPattern->mAtomPatternLen[i];

                if( sPatternLen == 0 )
                {
                    continue;
                }

                if( sStringLen < sPatternLen )
                {
                    STL_THROW( MATCH_FINISHED );
                }

                if( *sString != *sPattern->mAtomPatterns[i] )
                {
                    // 성능을 위해 stlMemcmp()를 호출하기 전에 byte 검사를 먼저한다. */
                    break;
                }
                if( stlMemcmp( sString,
                               sPattern->mAtomPatterns[i],
                               sPatternLen ) != 0 )
                {
                    break;
                }

                sString += sPatternLen;
                sStringLen -= sPatternLen;
            }

            if( ( i == sPattern->mAtomPatternCount ) && ( sStringLen == 0 ) )
            {
                *aResult = STL_TRUE;
                STL_THROW( MATCH_FINISHED );
            }

            STL_TRY( dtsGetMbLengthFuncList[aCharSetId]( aString,
                                                         &sStrMbLen,
                                                         aErrorStack )
                     == STL_SUCCESS );

            aString += sStrMbLen;
            aStringLen -= sStrMbLen;
        }

        if( aStringLen < sPattern->mPatternLen )
        {
            STL_THROW( MATCH_FINISHED );
        }
    }

    *aResult = STL_TRUE;

    STL_RAMP( MATCH_FINISHED );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/*******************************************************************************
 *   Not Like
 *   Not Like ==> [ P, O, M, S ]
 * ex) 'string' Not Like 's%n'  = true
 *******************************************************************************/

/**
 * @brief Not Like 
 *        <BR> expr Not Like expr
 *        <BR>   Not Like => [ P, O, M, S ]
 *        <BR> ex) 'gliese' Not Like 'g_se'  =>  true
 *        
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> STRING NOT LIKE STRING [ ESCAPE 'STRING']
 * @param[out] aResultValue      연산 결과 (결과타입 BOOLEAN)
 * @param[in]  aInfo             Pattern에 대한 자료구조
 * @param[in]  aVectorFunc       Function Vectorc
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfNotLike( stlUInt16        aInputArgumentCnt,
                      dtlValueEntry  * aInputArgument,
                      void           * aResultValue,
                      void           * aInfo,
                      dtlFuncVector  * aVectorFunc,
                      void           * aVectorArg,
                      void           * aEnv )
{
    STL_TRY( dtfLike( aInputArgumentCnt,
                      aInputArgument,
                      aResultValue,
                      aInfo,
                      aVectorFunc,
                      aVectorArg,
                      aEnv ) == STL_SUCCESS );

    DTL_BOOLEAN( aResultValue ) = ( ( DTL_BOOLEAN( aResultValue ) == STL_TRUE ) ?
                                    STL_FALSE : STL_TRUE );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
