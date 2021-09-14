/*******************************************************************************
 * dtfSubstrb.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: $
 *
 * NOTES
 *
 ******************************************************************************/

/**
 * @file dtfSubstrb.c
 * @brief String Function DataType Library Layer
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtfString.h>
#include <dtsCharacterSet.h>

/**
 * @ingroup dtf
 * @internal
 * @{
 */

stlStatus dtfStringSubstrbForAscii( stlUInt16        aInputArgumentCnt,
                                    dtlValueEntry  * aInputArgument,
                                    void           * aResultValue,
                                    dtlFuncVector  * aVectorFunc,
                                    void           * aVectorArg,
                                    void           * aEnv );

stlStatus dtfStringSubstrbForUtf8( stlUInt16        aInputArgumentCnt,
                                   dtlValueEntry  * aInputArgument,
                                   void           * aResultValue,
                                   dtlFuncVector  * aVectorFunc,
                                   void           * aVectorArg,
                                   void           * aEnv );

stlStatus dtfStringSubstrbForUhc( stlUInt16        aInputArgumentCnt,
                                  dtlValueEntry  * aInputArgument,
                                  void           * aResultValue,
                                  dtlFuncVector  * aVectorFunc,
                                  void           * aVectorArg,
                                  void           * aEnv );

stlStatus dtfStringSubstrbForGb18030( stlUInt16        aInputArgumentCnt,
                                      dtlValueEntry  * aInputArgument,
                                      void           * aResultValue,
                                      dtlFuncVector  * aVectorFunc,
                                      void           * aVectorArg,
                                      void           * aEnv );

dtfStringSubstrbFunc dtfStringSubstrbFuncList[ DTL_CHARACTERSET_MAX + 1 ] =
{
    dtfStringSubstrbForAscii,
    dtfStringSubstrbForUtf8,
    dtfStringSubstrbForUhc,
    dtfStringSubstrbForGb18030,
    
    NULL
};

/*******************************************************************************
 * substrb( str, position [, substring_length] )
 *   substrb( str, position [, substring_length] )  => [ O ]
 * ex) substrb( 'abcdefg', 5, 4 ) => efg
 *******************************************************************************/

/**
 * @brief substrb( str, position [, substring_length] )
 *        <BR> string의 substring 추출
 *        <BR> substrb( str, position [, substring_length] ) => [ O ]
 *        <BR> substrb( 'abcdefg', 5, 4 ) => efg
 *   
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> substrb(VARCHAR, BIGINT [, BIGINT] )
 *                               <BR> substrb(LONG VARCHAR, BIGINT [, BIGINT] )
 *                               <BR> substrb(str, position [, substring_length] )
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR | LONG VARCHAR)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfStringSubstrb( stlUInt16        aInputArgumentCnt,
                            dtlValueEntry  * aInputArgument,
                            void           * aResultValue,
                            dtlFuncVector  * aVectorFunc,
                            void           * aVectorArg,
                            void           * aEnv )
{
    /* character set이 추가될때 그에 따른 function list가 누락되는 일이 없도록 하기 위한 검사 코드 */
    DTF_CHECK_CHAR_SET_FUNC_LIST( dtfStringSubstrbFuncList );
    
    return dtfStringSubstrbFuncList[ aVectorFunc->mGetCharSetIDFunc(aVectorArg) ](
        aInputArgumentCnt,
        aInputArgument,
        aResultValue,
        aVectorFunc,
        aVectorArg,
        aEnv );
}

/**
 * @brief ascii 문자셋의 substrb( str, position [, substring_length] )
 *        <BR> string의 substring 추출
 *        <BR> substrb( str, position [, substring_length] ) => [ O ]
 *        <BR> substrb( 'abcdefg', 5, 4 ) => efg
 *   
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> substrb(VARCHAR, BIGINT [, BIGINT] )
 *                               <BR> substrb(LONG VARCHAR, BIGINT [, BIGINT] )
 *                               <BR> substrb(str, position [, substring_length] )
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR | LONG VARCHAR)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfStringSubstrbForAscii( stlUInt16        aInputArgumentCnt,
                                    dtlValueEntry  * aInputArgument,
                                    void           * aResultValue,
                                    dtlFuncVector  * aVectorFunc,
                                    void           * aVectorArg,
                                    void           * aEnv )
{
    dtlDataValue    * sValue1      = NULL;
    dtlDataValue    * sValue2      = NULL;
    dtlDataValue    * sValue3      = NULL;
    dtlDataValue    * sResultValue = NULL;
    
    stlInt64          sPosition         = 0;
    stlInt64          sSubstringLength  = 0;    
    stlInt64          sStrStartPosition = 0;
    stlInt64          sResultLen        = 0;
    stlInt64          sLen              = 0;
    stlInt64          sCopyStrLen       = 0;
    stlInt8           sTempLen          = 0;
    stlChar         * sTempStr          = NULL;
    stlChar         * sResult           = NULL;
    
    DTL_PARAM_VALIDATE( (aInputArgumentCnt >= 2) && (aInputArgumentCnt <= 3),
                        (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    
    sResult = (stlChar*)(sResultValue->mValue);
    
    DTF_CHECK_DTL_GROUP2( sValue1,
                          DTL_GROUP_CHAR_STRING,
                          DTL_GROUP_LONGVARCHAR_STRING,
                          (stlErrorStack *)aEnv );
    
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_NATIVE_BIGINT, (stlErrorStack *)aEnv );
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue,
                           DTL_TYPE_VARCHAR,
                           DTL_TYPE_LONGVARCHAR,
                           (stlErrorStack *)aEnv );

    STL_DASSERT( sResultValue->mBufferSize >= sValue1->mLength );    
    
    /*
     * position 값
     */
    
    sPosition = DTF_BIGINT( sValue2 );

    /* position 값이 0 이면 1로 재조정. */
    if( sPosition == 0 )
    {
        sPosition = 1;
    }
    else
    {
        /* Do Nothing */
    }

    /*
     * ( position > str의 길이 ) 인 경우, 결과값은 NULL이다.
     * 예) substrb( 'abc', 4 ) => NULL
     */
    if( sPosition > sValue1->mLength )
    {
        DTL_SET_NULL( sResultValue );
        STL_THROW( SUCCESS_END );
    }
    else
    {
        /* Do Nothing */
    }

    /*
     * ( position + str의 길이 ) < 0 ) 인 경우, 결과값은 NULL이다.
     * 예) substrb( 'abc', -4 ) => NULL
     */
    if( ( sPosition + sValue1->mLength ) < 0 )
    {
        DTL_SET_NULL( sResultValue );
        STL_THROW( SUCCESS_END );
    }
    else
    {
        /* Do Nothing */
    }

    /*
     * substring_length 값
     */
    
    if( aInputArgumentCnt == 3 )
    {
        sValue3 = aInputArgument[2].mValue.mDataValue;
        DTF_CHECK_DTL_VALUE( sValue3, DTL_TYPE_NATIVE_BIGINT, (stlErrorStack *)aEnv );
        
        sSubstringLength = DTF_BIGINT( sValue3 );
        
        /* ( substring_length < 1 ) 인 경우, 결과값은 NULL이다. */
        if( sSubstringLength < 1 )
        {
            DTL_SET_NULL( sResultValue );
            STL_THROW( SUCCESS_END );
        }
        else
        {
            /* Do Nothing */
        }
    }
    else
    {
        sSubstringLength = STL_INT64_MAX;
    }
    
    /*
     * str의 position 위치 찾기.
     * position의 시작위치가 안 맞는 경우 공백으로 처리.
     * 예) substrb( '가나다', 2, 5 ) => [][]나
     */
    
    STL_DASSERT( sPosition != 0 );
    
    sStrStartPosition = 0;
    
    if( sPosition < 0 )
    {
        sPosition = sValue1->mLength + sPosition + 1;
    }
    else
    {
        /* Do Nothing */
    }
    
    STL_DASSERT( sPosition > 0 );
    
    if( sPosition == 1 )
    {
        /* Do Nothing */
    }
    else
    {
        sTempStr = DTF_VARCHAR( sValue1 );
        
        sPosition--;
        
        while( sPosition > 0 )
        {
            DTS_GET_ASCII_MBLENGTH( sTempStr + sStrStartPosition,
                                    &sTempLen );
            
            sStrStartPosition = sStrStartPosition + sTempLen;
            
            if( sPosition >= sTempLen )
            {
                sPosition = sPosition - sTempLen;
            }
            else
            {
                sPosition = sTempLen - sPosition;
                sPosition = ( sSubstringLength < sPosition ) ? sSubstringLength : sPosition;
                
                sResultLen = sPosition;
                sSubstringLength = sSubstringLength - sPosition;
                STL_DASSERT( sSubstringLength >= 0 );

                for( ; sPosition > 0; sPosition-- )
                {
                    *sResult = ' ';
                    sResult++;
                }
                
                break;
            }
        }

        STL_DASSERT( sPosition == 0 );
    }

    /*
     * str의 position 위치부터 substring_length 만큼의 substring 구하기.
     * substring_length의 길이가 str의 binary length와 안 맞는 경우 공백으로 처리.
     * 예) substrb( '가나다', 2, 6 ) => [][]나[]
     */

    sTempStr = DTF_VARCHAR( sValue1 ) + sStrStartPosition;
    sLen = sValue1->mLength - sStrStartPosition;

    /*
     * substring_length >= (남은 str의 길이) 인 경우,
     * 예) substrb( '가나다', 1, 20 ) => 가나다
     *     lengthb( substrb( '가나다', 1, 20 ) ) => 9
     */
    if( sSubstringLength >= sLen )
    {
        sResultLen = sResultLen + sLen;
        
        STL_DASSERT( sResultLen <= sResultValue->mBufferSize );        
        
        stlMemcpy( sResult,
                   sTempStr,
                   sLen );
        
        STL_THROW( SUCCESS_END );
    }
    else
    {
        /* Do Nothing */
    }

    /*
     * substring_length < (남은 str의 길이) 인 경우,
     * 예) substrb( '가나다', 1, 5 ) => 가[][]
     * 예) substrb( '가나다', 2, 5 ) => [][]나
     */
    
    sLen = sSubstringLength;
    sResultLen = sResultLen + sLen;
    
    STL_DASSERT( sResultLen <= sResultValue->mBufferSize );
    
    while( sLen > 0 )
    {
        DTS_GET_ASCII_MBLENGTH( sTempStr,
                                &sTempLen );
        
        if( sLen >= sTempLen )
        {
            sLen = sLen - sTempLen;
            sTempStr = sTempStr + sTempLen;
        }
        else
        {
            break;
        }
    }
    
    if( sLen == 0 )
    {
        stlMemcpy( sResult,
                   DTF_VARCHAR( sValue1 ) + sStrStartPosition,
                   sSubstringLength );
    }
    else
    {
        STL_DASSERT( sLen > 0 );

        sCopyStrLen = sSubstringLength - sLen;
        
        stlMemcpy( sResult,
                   DTF_VARCHAR( sValue1 ) + sStrStartPosition,
                   sCopyStrLen );

        sResult = sResult + sCopyStrLen;
        
        for( ; sLen > 0; sLen-- )
        {
            *sResult = ' ';
            sResult++;
        }
    }

    STL_RAMP( SUCCESS_END );    
    
    STL_DASSERT( (sResultLen == sSubstringLength) ||
                 (sResultLen <= sValue1->mLength) );
    
    sResultValue->mLength = sResultLen;        

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief utf8 문자셋의 substrb( str, position [, substring_length] )
 *        <BR> string의 substring 추출
 *        <BR> substrb( str, position [, substring_length] ) => [ O ]
 *        <BR> substrb( 'abcdefg', 5, 4 ) => efg
 *   
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> substrb(VARCHAR, BIGINT [, BIGINT] )
 *                               <BR> substrb(LONG VARCHAR, BIGINT [, BIGINT] )
 *                               <BR> substrb(str, position [, substring_length] )
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR | LONG VARCHAR)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfStringSubstrbForUtf8( stlUInt16        aInputArgumentCnt,
                                   dtlValueEntry  * aInputArgument,
                                   void           * aResultValue,
                                   dtlFuncVector  * aVectorFunc,
                                   void           * aVectorArg,
                                   void           * aEnv )
{
    dtlDataValue    * sValue1      = NULL;
    dtlDataValue    * sValue2      = NULL;
    dtlDataValue    * sValue3      = NULL;
    dtlDataValue    * sResultValue = NULL;
    
    stlInt64          sPosition         = 0;
    stlInt64          sSubstringLength  = 0;    
    stlInt64          sStrStartPosition = 0;
    stlInt64          sResultLen        = 0;
    stlInt64          sLen              = 0;
    stlInt64          sCopyStrLen       = 0;
    stlInt8           sTempLen          = 0;
    stlChar         * sTempStr          = NULL;
    stlChar         * sResult           = NULL;
    
    DTL_PARAM_VALIDATE( (aInputArgumentCnt >= 2) && (aInputArgumentCnt <= 3),
                        (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    
    sResult = (stlChar*)(sResultValue->mValue);
    
    DTF_CHECK_DTL_GROUP2( sValue1,
                          DTL_GROUP_CHAR_STRING,
                          DTL_GROUP_LONGVARCHAR_STRING,
                          (stlErrorStack *)aEnv );
    
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_NATIVE_BIGINT, (stlErrorStack *)aEnv );
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue,
                           DTL_TYPE_VARCHAR,
                           DTL_TYPE_LONGVARCHAR,
                           (stlErrorStack *)aEnv );

    STL_DASSERT( sResultValue->mBufferSize >= sValue1->mLength );    
    
    /*
     * position 값
     */
    
    sPosition = DTF_BIGINT( sValue2 );

    /* position 값이 0 이면 1로 재조정. */
    if( sPosition == 0 )
    {
        sPosition = 1;
    }
    else
    {
        /* Do Nothing */
    }

    /*
     * ( position > str의 길이 ) 인 경우, 결과값은 NULL이다.
     * 예) substrb( 'abc', 4 ) => NULL
     */
    if( sPosition > sValue1->mLength )
    {
        DTL_SET_NULL( sResultValue );
        STL_THROW( SUCCESS_END );
    }
    else
    {
        /* Do Nothing */
    }

    /*
     * ( position + str의 길이 ) < 0 ) 인 경우, 결과값은 NULL이다.
     * 예) substrb( 'abc', -4 ) => NULL
     */
    if( ( sPosition + sValue1->mLength ) < 0 )
    {
        DTL_SET_NULL( sResultValue );
        STL_THROW( SUCCESS_END );
    }
    else
    {
        /* Do Nothing */
    }

    /*
     * substring_length 값
     */
    
    if( aInputArgumentCnt == 3 )
    {
        sValue3 = aInputArgument[2].mValue.mDataValue;
        DTF_CHECK_DTL_VALUE( sValue3, DTL_TYPE_NATIVE_BIGINT, (stlErrorStack *)aEnv );
        
        sSubstringLength = DTF_BIGINT( sValue3 );
        
        /* ( substring_length < 1 ) 인 경우, 결과값은 NULL이다. */
        if( sSubstringLength < 1 )
        {
            DTL_SET_NULL( sResultValue );
            STL_THROW( SUCCESS_END );
        }
        else
        {
            /* Do Nothing */
        }
    }
    else
    {
        sSubstringLength = STL_INT64_MAX;
    }
    
    /*
     * str의 position 위치 찾기.
     * position의 시작위치가 안 맞는 경우 공백으로 처리.
     * 예) substrb( '가나다', 2, 5 ) => [][]나
     */
    
    STL_DASSERT( sPosition != 0 );
    
    sStrStartPosition = 0;
    
    if( sPosition < 0 )
    {
        sPosition = sValue1->mLength + sPosition + 1;
    }
    else
    {
        /* Do Nothing */
    }
    
    STL_DASSERT( sPosition > 0 );
    
    if( sPosition == 1 )
    {
        /* Do Nothing */
    }
    else
    {
        sTempStr = DTF_VARCHAR( sValue1 );
        
        sPosition--;
        
        while( sPosition > 0 )
        {
            DTS_GET_UTF8_MBLENGTH( sTempStr + sStrStartPosition,
                                   &sTempLen );
            
            sStrStartPosition = sStrStartPosition + sTempLen;
            
            if( sPosition >= sTempLen )
            {
                sPosition = sPosition - sTempLen;
            }
            else
            {
                sPosition = sTempLen - sPosition;
                sPosition = ( sSubstringLength < sPosition ) ? sSubstringLength : sPosition;
                
                sResultLen = sPosition;
                sSubstringLength = sSubstringLength - sPosition;
                STL_DASSERT( sSubstringLength >= 0 );

                for( ; sPosition > 0; sPosition-- )
                {
                    *sResult = ' ';
                    sResult++;
                }
                
                break;
            }
        }

        STL_DASSERT( sPosition == 0 );
    }

    /*
     * str의 position 위치부터 substring_length 만큼의 substring 구하기.
     * substring_length의 길이가 str의 binary length와 안 맞는 경우 공백으로 처리.
     * 예) substrb( '가나다', 2, 6 ) => [][]나[]
     */

    sTempStr = DTF_VARCHAR( sValue1 ) + sStrStartPosition;
    sLen = sValue1->mLength - sStrStartPosition;

    /*
     * substring_length >= (남은 str의 길이) 인 경우,
     * 예) substrb( '가나다', 1, 20 ) => 가나다
     *     lengthb( substrb( '가나다', 1, 20 ) ) => 9
     */
    if( sSubstringLength >= sLen )
    {
        sResultLen = sResultLen + sLen;
        
        STL_DASSERT( sResultLen <= sResultValue->mBufferSize );        
        
        stlMemcpy( sResult,
                   sTempStr,
                   sLen );
        
        STL_THROW( SUCCESS_END );
    }
    else
    {
        /* Do Nothing */
    }

    /*
     * substring_length < (남은 str의 길이) 인 경우,
     * 예) substrb( '가나다', 1, 5 ) => 가[][]
     * 예) substrb( '가나다', 2, 5 ) => [][]나
     */
    
    sLen = sSubstringLength;
    sResultLen = sResultLen + sLen;
    
    STL_DASSERT( sResultLen <= sResultValue->mBufferSize );
    
    while( sLen > 0 )
    {
        DTS_GET_UTF8_MBLENGTH( sTempStr,
                               &sTempLen );
        
        if( sLen >= sTempLen )
        {
            sLen = sLen - sTempLen;
            sTempStr = sTempStr + sTempLen;
        }
        else
        {
            break;
        }
    }
    
    if( sLen == 0 )
    {
        stlMemcpy( sResult,
                   DTF_VARCHAR( sValue1 ) + sStrStartPosition,
                   sSubstringLength );
    }
    else
    {
        STL_DASSERT( sLen > 0 );

        sCopyStrLen = sSubstringLength - sLen;
        
        stlMemcpy( sResult,
                   DTF_VARCHAR( sValue1 ) + sStrStartPosition,
                   sCopyStrLen );

        sResult = sResult + sCopyStrLen;
        
        for( ; sLen > 0; sLen-- )
        {
            *sResult = ' ';
            sResult++;
        }
    }

    STL_RAMP( SUCCESS_END );    
    
    STL_DASSERT( (sResultLen == sSubstringLength) ||
                 (sResultLen <= sValue1->mLength) );
    
    sResultValue->mLength = sResultLen;        

    return STL_SUCCESS;

    STL_CATCH( ERROR_INVALID_CHARACTER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_CHARACTER_IN_CHARSET_REPERTOIRE,
                      NULL,
                      (stlErrorStack *)aEnv );  
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief uhc 문자셋의 substrb( str, position [, substring_length] )
 *        <BR> string의 substring 추출
 *        <BR> substrb( str, position [, substring_length] ) => [ O ]
 *        <BR> substrb( 'abcdefg', 5, 4 ) => efg
 *   
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> substrb(VARCHAR, BIGINT [, BIGINT] )
 *                               <BR> substrb(LONG VARCHAR, BIGINT [, BIGINT] )
 *                               <BR> substrb(str, position [, substring_length] )
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR | LONG VARCHAR)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfStringSubstrbForUhc( stlUInt16        aInputArgumentCnt,
                                  dtlValueEntry  * aInputArgument,
                                  void           * aResultValue,
                                  dtlFuncVector  * aVectorFunc,
                                  void           * aVectorArg,
                                  void           * aEnv )
{
    dtlDataValue    * sValue1      = NULL;
    dtlDataValue    * sValue2      = NULL;
    dtlDataValue    * sValue3      = NULL;
    dtlDataValue    * sResultValue = NULL;
    
    stlInt64          sPosition         = 0;
    stlInt64          sSubstringLength  = 0;    
    stlInt64          sStrStartPosition = 0;
    stlInt64          sResultLen        = 0;
    stlInt64          sLen              = 0;
    stlInt64          sCopyStrLen       = 0;
    stlInt8           sTempLen          = 0;
    stlChar         * sTempStr          = NULL;
    stlChar         * sResult           = NULL;
    
    DTL_PARAM_VALIDATE( (aInputArgumentCnt >= 2) && (aInputArgumentCnt <= 3),
                        (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    
    sResult = (stlChar*)(sResultValue->mValue);
    
    DTF_CHECK_DTL_GROUP2( sValue1,
                          DTL_GROUP_CHAR_STRING,
                          DTL_GROUP_LONGVARCHAR_STRING,
                          (stlErrorStack *)aEnv );
    
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_NATIVE_BIGINT, (stlErrorStack *)aEnv );
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue,
                           DTL_TYPE_VARCHAR,
                           DTL_TYPE_LONGVARCHAR,
                           (stlErrorStack *)aEnv );

    STL_DASSERT( sResultValue->mBufferSize >= sValue1->mLength );    
    
    /*
     * position 값
     */
    
    sPosition = DTF_BIGINT( sValue2 );

    /* position 값이 0 이면 1로 재조정. */
    if( sPosition == 0 )
    {
        sPosition = 1;
    }
    else
    {
        /* Do Nothing */
    }

    /*
     * ( position > str의 길이 ) 인 경우, 결과값은 NULL이다.
     * 예) substrb( 'abc', 4 ) => NULL
     */
    if( sPosition > sValue1->mLength )
    {
        DTL_SET_NULL( sResultValue );
        STL_THROW( SUCCESS_END );
    }
    else
    {
        /* Do Nothing */
    }

    /*
     * ( position + str의 길이 ) < 0 ) 인 경우, 결과값은 NULL이다.
     * 예) substrb( 'abc', -4 ) => NULL
     */
    if( ( sPosition + sValue1->mLength ) < 0 )
    {
        DTL_SET_NULL( sResultValue );
        STL_THROW( SUCCESS_END );
    }
    else
    {
        /* Do Nothing */
    }

    /*
     * substring_length 값
     */
    
    if( aInputArgumentCnt == 3 )
    {
        sValue3 = aInputArgument[2].mValue.mDataValue;
        DTF_CHECK_DTL_VALUE( sValue3, DTL_TYPE_NATIVE_BIGINT, (stlErrorStack *)aEnv );
        
        sSubstringLength = DTF_BIGINT( sValue3 );
        
        /* ( substring_length < 1 ) 인 경우, 결과값은 NULL이다. */
        if( sSubstringLength < 1 )
        {
            DTL_SET_NULL( sResultValue );
            STL_THROW( SUCCESS_END );
        }
        else
        {
            /* Do Nothing */
        }
    }
    else
    {
        sSubstringLength = STL_INT64_MAX;
    }
    
    /*
     * str의 position 위치 찾기.
     * position의 시작위치가 안 맞는 경우 공백으로 처리.
     * 예) substrb( '가나다', 2, 5 ) => [][]나
     */
    
    STL_DASSERT( sPosition != 0 );
    
    sStrStartPosition = 0;
    
    if( sPosition < 0 )
    {
        sPosition = sValue1->mLength + sPosition + 1;
    }
    else
    {
        /* Do Nothing */
    }
    
    STL_DASSERT( sPosition > 0 );
    
    if( sPosition == 1 )
    {
        /* Do Nothing */
    }
    else
    {
        sTempStr = DTF_VARCHAR( sValue1 );
        
        sPosition--;
        
        while( sPosition > 0 )
        {
            DTS_GET_UHC_MBLENGTH( sTempStr + sStrStartPosition,
                                  &sTempLen );
            
            sStrStartPosition = sStrStartPosition + sTempLen;
            
            if( sPosition >= sTempLen )
            {
                sPosition = sPosition - sTempLen;
            }
            else
            {
                sPosition = sTempLen - sPosition;
                sPosition = ( sSubstringLength < sPosition ) ? sSubstringLength : sPosition;
                
                sResultLen = sPosition;
                sSubstringLength = sSubstringLength - sPosition;
                STL_DASSERT( sSubstringLength >= 0 );

                for( ; sPosition > 0; sPosition-- )
                {
                    *sResult = ' ';
                    sResult++;
                }
                
                break;
            }
        }

        STL_DASSERT( sPosition == 0 );
    }

    /*
     * str의 position 위치부터 substring_length 만큼의 substring 구하기.
     * substring_length의 길이가 str의 binary length와 안 맞는 경우 공백으로 처리.
     * 예) substrb( '가나다', 2, 6 ) => [][]나[]
     */

    sTempStr = DTF_VARCHAR( sValue1 ) + sStrStartPosition;
    sLen = sValue1->mLength - sStrStartPosition;

    /*
     * substring_length >= (남은 str의 길이) 인 경우,
     * 예) substrb( '가나다', 1, 20 ) => 가나다
     *     lengthb( substrb( '가나다', 1, 20 ) ) => 9
     */
    if( sSubstringLength >= sLen )
    {
        sResultLen = sResultLen + sLen;
        
        STL_DASSERT( sResultLen <= sResultValue->mBufferSize );        
        
        stlMemcpy( sResult,
                   sTempStr,
                   sLen );
        
        STL_THROW( SUCCESS_END );
    }
    else
    {
        /* Do Nothing */
    }

    /*
     * substring_length < (남은 str의 길이) 인 경우,
     * 예) substrb( '가나다', 1, 5 ) => 가[][]
     * 예) substrb( '가나다', 2, 5 ) => [][]나
     */
    
    sLen = sSubstringLength;
    sResultLen = sResultLen + sLen;
    
    STL_DASSERT( sResultLen <= sResultValue->mBufferSize );
    
    while( sLen > 0 )
    {
        DTS_GET_UHC_MBLENGTH( sTempStr,
                              &sTempLen );
        
        if( sLen >= sTempLen )
        {
            sLen = sLen - sTempLen;
            sTempStr = sTempStr + sTempLen;
        }
        else
        {
            break;
        }
    }
    
    if( sLen == 0 )
    {
        stlMemcpy( sResult,
                   DTF_VARCHAR( sValue1 ) + sStrStartPosition,
                   sSubstringLength );
    }
    else
    {
        STL_DASSERT( sLen > 0 );

        sCopyStrLen = sSubstringLength - sLen;
        
        stlMemcpy( sResult,
                   DTF_VARCHAR( sValue1 ) + sStrStartPosition,
                   sCopyStrLen );

        sResult = sResult + sCopyStrLen;
        
        for( ; sLen > 0; sLen-- )
        {
            *sResult = ' ';
            sResult++;
        }
    }

    STL_RAMP( SUCCESS_END );    
    
    STL_DASSERT( (sResultLen == sSubstringLength) ||
                 (sResultLen <= sValue1->mLength) );
    
    sResultValue->mLength = sResultLen;        

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief gb18030 문자셋의 substrb( str, position [, substring_length] )
 *        <BR> string의 substring 추출
 *        <BR> substrb( str, position [, substring_length] ) => [ O ]
 *        <BR> substrb( 'abcdefg', 5, 4 ) => efg
 *   
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> substrb(VARCHAR, BIGINT [, BIGINT] )
 *                               <BR> substrb(LONG VARCHAR, BIGINT [, BIGINT] )
 *                               <BR> substrb(str, position [, substring_length] )
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR | LONG VARCHAR)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfStringSubstrbForGb18030( stlUInt16        aInputArgumentCnt,
                                      dtlValueEntry  * aInputArgument,
                                      void           * aResultValue,
                                      dtlFuncVector  * aVectorFunc,
                                      void           * aVectorArg,
                                      void           * aEnv )
{
    dtlDataValue    * sValue1      = NULL;
    dtlDataValue    * sValue2      = NULL;
    dtlDataValue    * sValue3      = NULL;
    dtlDataValue    * sResultValue = NULL;
    
    stlInt64          sPosition         = 0;
    stlInt64          sSubstringLength  = 0;    
    stlInt64          sStrStartPosition = 0;
    stlInt64          sResultLen        = 0;
    stlInt64          sLen              = 0;
    stlInt64          sCopyStrLen       = 0;
    stlInt8           sTempLen          = 0;
    stlChar         * sTempStr          = NULL;
    stlChar         * sResult           = NULL;
    
    DTL_PARAM_VALIDATE( (aInputArgumentCnt >= 2) && (aInputArgumentCnt <= 3),
                        (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    
    sResult = (stlChar*)(sResultValue->mValue);
    
    DTF_CHECK_DTL_GROUP2( sValue1,
                          DTL_GROUP_CHAR_STRING,
                          DTL_GROUP_LONGVARCHAR_STRING,
                          (stlErrorStack *)aEnv );
    
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_NATIVE_BIGINT, (stlErrorStack *)aEnv );
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue,
                           DTL_TYPE_VARCHAR,
                           DTL_TYPE_LONGVARCHAR,
                           (stlErrorStack *)aEnv );

    STL_DASSERT( sResultValue->mBufferSize >= sValue1->mLength );    
    
    /*
     * position 값
     */
    
    sPosition = DTF_BIGINT( sValue2 );

    /* position 값이 0 이면 1로 재조정. */
    if( sPosition == 0 )
    {
        sPosition = 1;
    }
    else
    {
        /* Do Nothing */
    }

    /*
     * ( position > str의 길이 ) 인 경우, 결과값은 NULL이다.
     * 예) substrb( 'abc', 4 ) => NULL
     */
    if( sPosition > sValue1->mLength )
    {
        DTL_SET_NULL( sResultValue );
        STL_THROW( SUCCESS_END );
    }
    else
    {
        /* Do Nothing */
    }

    /*
     * ( position + str의 길이 ) < 0 ) 인 경우, 결과값은 NULL이다.
     * 예) substrb( 'abc', -4 ) => NULL
     */
    if( ( sPosition + sValue1->mLength ) < 0 )
    {
        DTL_SET_NULL( sResultValue );
        STL_THROW( SUCCESS_END );
    }
    else
    {
        /* Do Nothing */
    }

    /*
     * substring_length 값
     */
    
    if( aInputArgumentCnt == 3 )
    {
        sValue3 = aInputArgument[2].mValue.mDataValue;
        DTF_CHECK_DTL_VALUE( sValue3, DTL_TYPE_NATIVE_BIGINT, (stlErrorStack *)aEnv );
        
        sSubstringLength = DTF_BIGINT( sValue3 );
        
        /* ( substring_length < 1 ) 인 경우, 결과값은 NULL이다. */
        if( sSubstringLength < 1 )
        {
            DTL_SET_NULL( sResultValue );
            STL_THROW( SUCCESS_END );
        }
        else
        {
            /* Do Nothing */
        }
    }
    else
    {
        sSubstringLength = STL_INT64_MAX;
    }
    
    /*
     * str의 position 위치 찾기.
     * position의 시작위치가 안 맞는 경우 공백으로 처리.
     * 예) substrb( '가나다', 2, 5 ) => [][]나
     */
    
    STL_DASSERT( sPosition != 0 );
    
    sStrStartPosition = 0;
    
    if( sPosition < 0 )
    {
        sPosition = sValue1->mLength + sPosition + 1;
    }
    else
    {
        /* Do Nothing */
    }
    
    STL_DASSERT( sPosition > 0 );
    
    if( sPosition == 1 )
    {
        /* Do Nothing */
    }
    else
    {
        sTempStr = DTF_VARCHAR( sValue1 );
        
        sPosition--;
        
        while( sPosition > 0 )
        {
            DTS_GET_GB18030_MBLENGTH( sTempStr + sStrStartPosition,
                                      &sTempLen );
            
            sStrStartPosition = sStrStartPosition + sTempLen;
            
            if( sPosition >= sTempLen )
            {
                sPosition = sPosition - sTempLen;
            }
            else
            {
                sPosition = sTempLen - sPosition;
                sPosition = ( sSubstringLength < sPosition ) ? sSubstringLength : sPosition;
                
                sResultLen = sPosition;
                sSubstringLength = sSubstringLength - sPosition;
                STL_DASSERT( sSubstringLength >= 0 );

                for( ; sPosition > 0; sPosition-- )
                {
                    *sResult = ' ';
                    sResult++;
                }
                
                break;
            }
        }

        STL_DASSERT( sPosition == 0 );
    }

    /*
     * str의 position 위치부터 substring_length 만큼의 substring 구하기.
     * substring_length의 길이가 str의 binary length와 안 맞는 경우 공백으로 처리.
     * 예) substrb( '가나다', 2, 6 ) => [][]나[]
     */

    sTempStr = DTF_VARCHAR( sValue1 ) + sStrStartPosition;
    sLen = sValue1->mLength - sStrStartPosition;

    /*
     * substring_length >= (남은 str의 길이) 인 경우,
     * 예) substrb( '가나다', 1, 20 ) => 가나다
     *     lengthb( substrb( '가나다', 1, 20 ) ) => 9
     */
    if( sSubstringLength >= sLen )
    {
        sResultLen = sResultLen + sLen;
        
        STL_DASSERT( sResultLen <= sResultValue->mBufferSize );        
        
        stlMemcpy( sResult,
                   sTempStr,
                   sLen );
        
        STL_THROW( SUCCESS_END );
    }
    else
    {
        /* Do Nothing */
    }

    /*
     * substring_length < (남은 str의 길이) 인 경우,
     * 예) substrb( '가나다', 1, 5 ) => 가[][]
     * 예) substrb( '가나다', 2, 5 ) => [][]나
     */
    
    sLen = sSubstringLength;
    sResultLen = sResultLen + sLen;
    
    STL_DASSERT( sResultLen <= sResultValue->mBufferSize );
    
    while( sLen > 0 )
    {
        DTS_GET_GB18030_MBLENGTH( sTempStr,
                                  &sTempLen );
        
        if( sLen >= sTempLen )
        {
            sLen = sLen - sTempLen;
            sTempStr = sTempStr + sTempLen;
        }
        else
        {
            break;
        }
    }
    
    if( sLen == 0 )
    {
        stlMemcpy( sResult,
                   DTF_VARCHAR( sValue1 ) + sStrStartPosition,
                   sSubstringLength );
    }
    else
    {
        STL_DASSERT( sLen > 0 );

        sCopyStrLen = sSubstringLength - sLen;
        
        stlMemcpy( sResult,
                   DTF_VARCHAR( sValue1 ) + sStrStartPosition,
                   sCopyStrLen );

        sResult = sResult + sCopyStrLen;
        
        for( ; sLen > 0; sLen-- )
        {
            *sResult = ' ';
            sResult++;
        }
    }

    STL_RAMP( SUCCESS_END );    
    
    STL_DASSERT( (sResultLen == sSubstringLength) ||
                 (sResultLen <= sValue1->mLength) );
    
    sResultValue->mLength = sResultLen;        

    return STL_SUCCESS;

    STL_CATCH( ERROR_CHARACTER_NOT_IN_REPERTOIRE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_CHARACTER_IN_CHARSET_REPERTOIRE,
                      NULL,
                      (stlErrorStack *)aEnv );  
    }

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief substrb( str, position [, substring_length] )
 *        <BR> string의 substring 추출
 *        <BR> substrb( str, position [, substring_length] ) => [ O ]
 *        <BR> substrb( X'AABBCC', 2, 1 ) => BB
 *   
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> substrb(VARBINARY, BIGINT [, BIGINT] )
 *                               <BR> substrb(LONG VARBINARY, BIGINT [, BIGINT] )
 *                               <BR> substrb(str, position [, substring_length] )
 * @param[out] aResultValue      연산 결과 (결과타입 VARBINARY | LONG VARBINARY)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfBinaryStringSubstrb( stlUInt16        aInputArgumentCnt,
                                  dtlValueEntry  * aInputArgument,
                                  void           * aResultValue,
                                  dtlFuncVector  * aVectorFunc,
                                  void           * aVectorArg,
                                  void           * aEnv )
{
    dtlDataValue    * sValue1      = NULL;
    dtlDataValue    * sValue2      = NULL;
    dtlDataValue    * sValue3      = NULL;
    dtlDataValue    * sResultValue = NULL;
    
    stlInt64          sPosition         = 0;
    stlInt64          sSubstringLength  = 0;    
    stlInt64          sResultLen        = 0;
    stlInt64          sLen              = 0;
    stlChar         * sTempStr          = NULL;
    stlChar         * sResult           = NULL;
    
    DTL_PARAM_VALIDATE( (aInputArgumentCnt >= 2) && (aInputArgumentCnt <= 3),
                        (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    
    sResult = (stlChar*)(sResultValue->mValue);

    DTF_CHECK_DTL_GROUP2( sValue1,
                          DTL_GROUP_BINARY_STRING,
                          DTL_GROUP_LONGVARBINARY_STRING,
                          (stlErrorStack *)aEnv );
    
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_NATIVE_BIGINT, (stlErrorStack *)aEnv );
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue,
                           DTL_TYPE_VARBINARY,
                           DTL_TYPE_LONGVARBINARY,
                           (stlErrorStack *)aEnv );
    
    STL_DASSERT( sResultValue->mBufferSize >= sValue1->mLength );    
    
    /*
     * position 값
     */
    
    sPosition = DTF_BIGINT( sValue2 );

    /* position 값이 0 이면 1로 재조정. */
    if( sPosition == 0 )
    {
        sPosition = 1;
    }
    else
    {
        /* Do Nothing */
    }

    /*
     * ( position > str의 길이 ) 인 경우, 결과값은 NULL이다.
     * 예) substrb( 'abc', 4 ) => NULL
     */
    if( sPosition > sValue1->mLength )
    {
        DTL_SET_NULL( sResultValue );
        STL_THROW( SUCCESS_END );
    }
    else
    {
        /* Do Nothing */
    }

    /*
     * ( position + str의 길이 ) < 0 ) 인 경우, 결과값은 NULL이다.
     * 예) substrb( 'abc', -4 ) => NULL
     */
    if( ( sPosition + sValue1->mLength ) < 0 )
    {
        DTL_SET_NULL( sResultValue );
        STL_THROW( SUCCESS_END );
    }
    else
    {
        /* Do Nothing */
    }

    /*
     * substring_length 값
     */
    
    if( aInputArgumentCnt == 3 )
    {
        sValue3 = aInputArgument[2].mValue.mDataValue;
        DTF_CHECK_DTL_VALUE( sValue3, DTL_TYPE_NATIVE_BIGINT, (stlErrorStack *)aEnv );
        
        sSubstringLength = DTF_BIGINT( sValue3 );
        
        /* ( substring_length < 1 ) 인 경우, 결과값은 NULL이다. */
        if( sSubstringLength < 1 )
        {
            DTL_SET_NULL( sResultValue );
            STL_THROW( SUCCESS_END );
        }
        else
        {
            /* Do Nothing */
        }
    }
    else
    {
        sSubstringLength = STL_INT64_MAX;
    }
    
    /*
     * str의 position 위치 찾기.
     */
    
    STL_DASSERT( sPosition != 0 );
    
    if( sPosition > 0 )
    {
        /*
         * sPosition 이 양수인 경우          
         * str의 왼쪽 부터 찾는다.
         */

        sLen = sPosition - 1;
        sTempStr = DTF_VARBINARY( sValue1 ) + sLen;
        sLen = sValue1->mLength - sLen;
    }
    else
    {
        /*
         * sPosition 이 음수인 경우          
         * str의 오른쪽 부터 찾는다.
         */ 
        
        sLen = sValue1->mLength + sPosition;
        sTempStr = DTF_VARBINARY( sValue1 ) + sLen;
        sLen = sValue1->mLength - sLen;
    }

    if( sSubstringLength > sLen )
    {
        sResultLen = sLen;
        STL_DASSERT( sResultLen <= sResultValue->mBufferSize );
        
        stlMemcpy( sResult,
                   sTempStr,
                   sLen );
    }
    else
    {
        sResultLen = sSubstringLength;
        STL_DASSERT( sResultLen <= sResultValue->mBufferSize );        
        
        stlMemcpy( sResult,
                   sTempStr,
                   sSubstringLength );
    }
    
    STL_RAMP( SUCCESS_END );    
    
    STL_DASSERT( (sResultLen == sSubstringLength) ||
                 (sResultLen <= sValue1->mLength) );
    
    sResultValue->mLength = sResultLen;        

    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/** @} */
