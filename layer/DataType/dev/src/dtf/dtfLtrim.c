/*******************************************************************************
 * dtfLtrim.c
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
 * @file dtfLtrim.c
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

#define DTF_LTRIM_INFO_ARR_MAX ( 500 )

typedef struct dtfLtrim
{
    stlInt8    mTrimByte;
    stlChar    mAlign1[3];   /**< 8byte align을 위한 padding */
#if (STL_SIZEOF_VOIDP == 8)
    stlUInt32  mAlign2;      /**< 8byte align을 위한 추가 padding */
#endif 
    stlChar  * mTrimStr;        
} dtfLtrim;


stlStatus dtfStringLtrimForAscii( stlUInt16        aInputArgumentCnt,
                                  dtlValueEntry  * aInputArgument,
                                  void           * aResultValue,
                                  dtlFuncVector  * aVectorFunc,
                                  void           * aVectorArg,
                                  void           * aEnv );

stlStatus dtfStringLtrimForUtf8( stlUInt16        aInputArgumentCnt,
                                 dtlValueEntry  * aInputArgument,
                                 void           * aResultValue,
                                 dtlFuncVector  * aVectorFunc,
                                 void           * aVectorArg,
                                 void           * aEnv );

stlStatus dtfStringLtrimForUhc( stlUInt16        aInputArgumentCnt,
                                dtlValueEntry  * aInputArgument,
                                void           * aResultValue,
                                dtlFuncVector  * aVectorFunc,
                                void           * aVectorArg,
                                void           * aEnv );

stlStatus dtfStringLtrimForGb18030( stlUInt16        aInputArgumentCnt,
                                    dtlValueEntry  * aInputArgument,
                                    void           * aResultValue,
                                    dtlFuncVector  * aVectorFunc,
                                    void           * aVectorArg,
                                    void           * aEnv );

dtfStringLtrimFunc dtfStringLtrimFuncList[ DTL_CHARACTERSET_MAX + 1 ] =
{
    dtfStringLtrimForAscii,
    dtfStringLtrimForUtf8,
    dtfStringLtrimForUhc,
    dtfStringLtrimForGb18030,
    
    NULL
};

/*******************************************************************************
 * LTRIM( trim_source, trim_character )
 *   LTRIM( trim_source, trim_character ) => [ P, O ]
 * ex) LTRIM( 'xTom', 'x' ) => Tom
 *******************************************************************************/

/**
 * @brief LTRIM( trim_source, trim_character )
 *        <BR> LTRIM( trim_source, trim_character )
 *        <BR>   LTRIM( trim_source, trim_character ) => [ P, O ]
 *        <BR> ex) LTRIM( 'xTom', 'x' ) => Tom
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> ltrim(VARCHAR, VARCHAR)
 *                               <BR> ltrim(LONGVARCHAR, LONGVARCHAR)
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR || LONGVARCHAR)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfStringLtrim( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv )
{
    /* character set이 추가될때 그에 따른 function list가 누락되는 일이 없도록 하기 위한 검사 코드 */
    DTF_CHECK_CHAR_SET_FUNC_LIST( dtfStringLtrimFuncList );
    
    return dtfStringLtrimFuncList[ aVectorFunc->mGetCharSetIDFunc(aVectorArg) ](
        aInputArgumentCnt,
        aInputArgument,
        aResultValue,
        aVectorFunc,
        aVectorArg,
        aEnv );
}


/**
 * @brief ascii 에 대한 ltrim 처리
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> ltrim(VARCHAR, VARCHAR)
 *                               <BR> ltrim(LONGVARCHAR, LONGVARCHAR)
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR || LONGVARCHAR)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfStringLtrimForAscii( stlUInt16        aInputArgumentCnt,
                                  dtlValueEntry  * aInputArgument,
                                  void           * aResultValue,
                                  dtlFuncVector  * aVectorFunc,
                                  void           * aVectorArg,
                                  void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;
    
    stlChar         * sString;
    stlChar         * sTrimStr;
    stlChar         * sTrimStrEnd;
    stlInt64          sStringByteLen = 0;
    stlInt8           sStrByte = 0;
    stlInt8           sTrimByte = 0;
    stlInt64          sTrimCharLen = 0;
    stlInt64          sTrimCharLenOrg = 0;    
    stlChar           sDefaultTrimChar[1];
    
    dtlCharacterSet   sCharSetID = DTL_SQL_ASCII;
    stlInt64          i = 0;
    stlBool           sIsRemainTrimStr = STL_FALSE;
    stlBool           sIsEqual = STL_FALSE;

    stlChar         * sTempTrimStr;
    dtfLtrim          sTrimInfo[ DTF_LTRIM_INFO_ARR_MAX ];
    
    
    /**
     * Parameter Validation
     */
    
    DTL_PARAM_VALIDATE( ( aInputArgumentCnt >= 1 ) &&
                        ( aInputArgumentCnt <= 2 ),
                        (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_GROUP2( sValue1,
                          DTL_GROUP_CHAR_STRING,
                          DTL_GROUP_LONGVARCHAR_STRING,
                          sErrorStack );
    /* NULL value는 들어오지 않는다. */
    STL_DASSERT( DTL_IS_NULL( sValue1 ) != STL_TRUE );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue,
                           DTL_TYPE_VARCHAR,
                           DTL_TYPE_LONGVARCHAR,
                           sErrorStack );

    /*
     * sString : target data
     * sStringLen : target data의 길이
     */
    sString = DTF_VARCHAR( sValue1 );
    sStringByteLen = sValue1->mLength;

    if( aInputArgumentCnt == 1 )
    {
        /*
         * trim_character가 생략되었을 경우, ' '으로 처리됨.
         * 예) LTRIM( 'abcde' ) => LTRIM( 'abcde', ' ' )
         */

        /*
         * @todo
         * ' ' 는 character set에 맞는 code로 가져와야 한다.
         * character set에 맞는 CharLen, ByteLen을 구해야 함.
         */
        sDefaultTrimChar[0] = ' ';
        sTrimByte = 1;
        
        sTrimStr = sDefaultTrimChar;        
        
        while( sStringByteLen > 0 )
        {
            /**
             * 체크할 sString의 첫번째 하나의 char의 length를 구한다.
             */
            
            DTS_GET_ASCII_MBLENGTH( sString,
                                    & sStrByte );
            
            if( sStrByte == sTrimByte )
            {
                if( sString[0] == sTrimStr[0] )
                {
                    /* Match */
                    sString += sTrimByte;
                    sStringByteLen -= sTrimByte;
                    
                    continue;
                }
                else
                {
                    /* No Match */
                    break;
                }
            }
            else
            {
                /* No Match */
                break;
            }
        }
        
        STL_THROW( SET_RESULT_VALUE );
    }
    else
    {
        sValue2 = aInputArgument[1].mValue.mDataValue;
        
        DTF_CHECK_DTL_GROUP2( sValue2,
                              DTL_GROUP_CHAR_STRING,
                              DTL_GROUP_LONGVARCHAR_STRING,
                              sErrorStack );
        /* NULL value는 들어오지 않는다. */
        STL_DASSERT( DTL_IS_NULL( sValue2 ) != STL_TRUE );
        
        sTrimStr = DTF_VARCHAR( sValue2 );
        sTrimStrEnd = sTrimStr + sValue2->mLength;
    }

    /*
     * trim_character 에 대한 정보구축
     */
    
    DTS_GET_ASCII_MBLENGTH( sTrimStr,
                            & sTrimByte );
    
    i = 0;
    sTrimInfo[i].mTrimByte = sTrimByte;
    sTrimInfo[i].mTrimStr = sTrimStr;
    
    sTrimStr += sTrimInfo[i].mTrimByte;
    i++;
    
    while( ( sTrimStr < sTrimStrEnd ) && ( i < DTF_LTRIM_INFO_ARR_MAX )  )
    {
        DTS_GET_ASCII_MBLENGTH( sTrimStr,
                                & sTrimByte );
        
        sTrimInfo[i].mTrimByte = sTrimByte;
        sTrimInfo[i].mTrimStr = sTrimStr;
        
        sTrimStr += sTrimInfo[i].mTrimByte;
        i++;
    }
    
    if( sTrimStr < sTrimStrEnd )
    {
        sIsRemainTrimStr = STL_TRUE;
        
        /* trim_character의 문자길이를 구한다. */
        STL_TRY( dtsGetMbstrlenWithLenFuncList[ sCharSetID ]( sTrimStr,
                                                              ( sTrimStrEnd - sTrimStr ),
                                                              & sTrimCharLen,
                                                              aVectorFunc,
                                                              aVectorArg,
                                                              sErrorStack )
                 == STL_SUCCESS );
        
        sTrimCharLenOrg = i + sTrimCharLen;
    }
    else
    {
        STL_DASSERT( sTrimStr == sTrimStrEnd );
        
        sIsRemainTrimStr = STL_FALSE;
        /* trim_character의 문자길이를 구한다. */        
        sTrimCharLenOrg = i;
    }
    
    /*
     * ltrim
     */
    
    sTrimCharLen = ( sTrimCharLenOrg <= DTF_LTRIM_INFO_ARR_MAX ) ? sTrimCharLenOrg : DTF_LTRIM_INFO_ARR_MAX;
    sTrimStr = DTF_VARCHAR( sValue2 );
    while( sStringByteLen > 0 )
    {
        /**
         * 체크할 sString의 첫번째 하나의 char의 length를 구한다.
         */

        DTS_GET_ASCII_MBLENGTH( sString,
                                &sStrByte );
        
        for( i = 0; i < sTrimCharLen; i++ )
        {
            if( sStrByte == sTrimInfo[i].mTrimByte )
            {
                DTS_IS_EQUAL_SAME_BYTELEN_ONE_CHAR( sString,
                                                    sTrimInfo[i].mTrimStr,
                                                    sTrimInfo[i].mTrimByte,
                                                    & sIsEqual );
                if( sIsEqual == STL_TRUE )
                {
                    /* Match */
                    sString += sTrimInfo[i].mTrimByte;
                    sStringByteLen -= sTrimInfo[i].mTrimByte;
                    
                    break;
                }
                else
                {
                    /* No Match */
                    /* Do Nothing */                
                }
            }
            else
            {
                /* No Match */
                /* Do Nothing */                
            }
        }
        
        if( i < sTrimCharLen )
        {
            /* Match */
            continue;
        }
        else
        {
            /* Do Nothing */
        }
        
        STL_DASSERT( i == sTrimCharLen );
        
        if( sIsRemainTrimStr == STL_FALSE )
        {
            /* No Match */
            break;
        }
        else
        {
            /*
             * trim_character가 DTF_LTRIM_INFO_ARR_MAX characters를 넘는 경우로, 
             * sTrimInfo 를 구성하지 못한 나머지 trim string 처리.
             */
            
            STL_DASSERT( sTrimCharLen == DTF_LTRIM_INFO_ARR_MAX );
            
            sTempTrimStr = ( sTrimInfo[ sTrimCharLen - 1 ].mTrimStr + sTrimInfo[ sTrimCharLen - 1 ].mTrimByte );
            
            for( i = sTrimCharLen; i < sTrimCharLenOrg; i++ )
            {
                /**
                 * 체크할 sTrimStr의 첫번째 하나의 char의 length를 구한다.
                 */

                DTS_GET_ASCII_MBLENGTH( sTempTrimStr,
                                        & sTrimByte );
                
                if( sStrByte == sTrimByte )
                {
                    DTS_IS_EQUAL_SAME_BYTELEN_ONE_CHAR( sString,
                                                        sTempTrimStr,
                                                        sTrimByte,
                                                        & sIsEqual );
                    if( sIsEqual == STL_TRUE )
                    {
                        /* Match */
                        sString += sTrimByte;
                        sStringByteLen -= sTrimByte;
                        
                        break;
                    }
                    else
                    {
                        /* No Match */
                        /* Do Nothing */
                    }
                }
                else
                {
                    /* No Match */
                    /* Do Nothing */
                }
                
                /* trim_character의 다음 문자로 이동 */
                sTempTrimStr += sTrimByte;
            }
            
            if( i < sTrimCharLenOrg )
            {
                /* Match */
                continue;
            }
            else
            {
                /* No Match */
                break;
            }
        }
    }
    
    STL_RAMP( SET_RESULT_VALUE );
    
    stlMemcpy( DTF_VARCHAR( sResultValue ), sString, sStringByteLen );
    sResultValue->mLength = sStringByteLen;
    
    STL_DASSERT( sResultValue->mLength <= sResultValue->mBufferSize );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief utf8 에 대한 ltrim 처리
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> ltrim(VARCHAR, VARCHAR)
 *                               <BR> ltrim(LONGVARCHAR, LONGVARCHAR)
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR || LONGVARCHAR)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfStringLtrimForUtf8( stlUInt16        aInputArgumentCnt,
                                 dtlValueEntry  * aInputArgument,
                                 void           * aResultValue,
                                 dtlFuncVector  * aVectorFunc,
                                 void           * aVectorArg,
                                 void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;
    
    stlChar         * sString;
    stlChar         * sTrimStr;
    stlChar         * sTrimStrEnd;
    stlInt64          sStringByteLen = 0;
    stlInt8           sStrByte = 0;
    stlInt8           sTrimByte = 0;
    stlInt64          sTrimCharLen = 0;
    stlInt64          sTrimCharLenOrg = 0;    
    stlChar           sDefaultTrimChar[1];
    
    dtlCharacterSet   sCharSetID = DTL_UTF8;
    stlInt64          i = 0;
    stlBool           sIsRemainTrimStr = STL_FALSE;
    stlBool           sIsEqual = STL_FALSE;

    stlChar         * sTempTrimStr;
    dtfLtrim          sTrimInfo[ DTF_LTRIM_INFO_ARR_MAX ];
    
    
    /**
     * Parameter Validation
     */
    
    DTL_PARAM_VALIDATE( ( aInputArgumentCnt >= 1 ) &&
                        ( aInputArgumentCnt <= 2 ),
                        (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_GROUP2( sValue1,
                          DTL_GROUP_CHAR_STRING,
                          DTL_GROUP_LONGVARCHAR_STRING,
                          sErrorStack );
    /* NULL value는 들어오지 않는다. */
    STL_DASSERT( DTL_IS_NULL( sValue1 ) != STL_TRUE );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue,
                           DTL_TYPE_VARCHAR,
                           DTL_TYPE_LONGVARCHAR,
                           sErrorStack );

    /*
     * sString : target data
     * sStringLen : target data의 길이
     */
    sString = DTF_VARCHAR( sValue1 );
    sStringByteLen = sValue1->mLength;

    if( aInputArgumentCnt == 1 )
    {
        /*
         * trim_character가 생략되었을 경우, ' '으로 처리됨.
         * 예) LTRIM( 'abcde' ) => LTRIM( 'abcde', ' ' )
         */

        /*
         * @todo
         * ' ' 는 character set에 맞는 code로 가져와야 한다.
         * character set에 맞는 CharLen, ByteLen을 구해야 함.
         */
        sDefaultTrimChar[0] = ' ';
        sTrimByte = 1;
        
        sTrimStr = sDefaultTrimChar;        
        
        while( sStringByteLen > 0 )
        {
            /**
             * 체크할 sString의 첫번째 하나의 char의 length를 구한다.
             */
            
            DTS_GET_UTF8_MBLENGTH( sString,
                                   & sStrByte );
            
            if( sStrByte == sTrimByte )
            {
                if( sString[0] == sTrimStr[0] )
                {
                    /* Match */
                    sString += sTrimByte;
                    sStringByteLen -= sTrimByte;
                    
                    continue;
                }
                else
                {
                    /* No Match */
                    break;
                }
            }
            else
            {
                /* No Match */
                break;
            }
        }
        
        STL_THROW( SET_RESULT_VALUE );
    }
    else
    {
        sValue2 = aInputArgument[1].mValue.mDataValue;
        
        DTF_CHECK_DTL_GROUP2( sValue2,
                              DTL_GROUP_CHAR_STRING,
                              DTL_GROUP_LONGVARCHAR_STRING,
                              sErrorStack );
        /* NULL value는 들어오지 않는다. */
        STL_DASSERT( DTL_IS_NULL( sValue2 ) != STL_TRUE );
        
        sTrimStr = DTF_VARCHAR( sValue2 );
        sTrimStrEnd = sTrimStr + sValue2->mLength;
    }

    /*
     * trim_character 에 대한 정보구축
     */
    
    DTS_GET_UTF8_MBLENGTH( sTrimStr,
                           & sTrimByte );
    
    i = 0;
    sTrimInfo[i].mTrimByte = sTrimByte;
    sTrimInfo[i].mTrimStr = sTrimStr;
    
    sTrimStr += sTrimInfo[i].mTrimByte;
    i++;
    
    while( ( sTrimStr < sTrimStrEnd ) && ( i < DTF_LTRIM_INFO_ARR_MAX )  )
    {
        DTS_GET_UTF8_MBLENGTH( sTrimStr,
                               & sTrimByte );
        
        sTrimInfo[i].mTrimByte = sTrimByte;
        sTrimInfo[i].mTrimStr = sTrimStr;
        
        sTrimStr += sTrimInfo[i].mTrimByte;
        i++;
    }
    
    if( sTrimStr < sTrimStrEnd )
    {
        sIsRemainTrimStr = STL_TRUE;
        
        /* trim_character의 문자길이를 구한다. */
        STL_TRY( dtsGetMbstrlenWithLenFuncList[ sCharSetID ]( sTrimStr,
                                                              ( sTrimStrEnd - sTrimStr ),
                                                              & sTrimCharLen,
                                                              aVectorFunc,
                                                              aVectorArg,
                                                              sErrorStack )
                 == STL_SUCCESS );
        
        sTrimCharLenOrg = i + sTrimCharLen;
    }
    else
    {
        STL_DASSERT( sTrimStr == sTrimStrEnd );
        
        sIsRemainTrimStr = STL_FALSE;
        /* trim_character의 문자길이를 구한다. */        
        sTrimCharLenOrg = i;
    }
    
    /*
     * ltrim
     */
    
    sTrimCharLen = ( sTrimCharLenOrg <= DTF_LTRIM_INFO_ARR_MAX ) ? sTrimCharLenOrg : DTF_LTRIM_INFO_ARR_MAX;
    sTrimStr = DTF_VARCHAR( sValue2 );
    while( sStringByteLen > 0 )
    {
        /**
         * 체크할 sString의 첫번째 하나의 char의 length를 구한다.
         */

        DTS_GET_UTF8_MBLENGTH( sString,
                               &sStrByte );
        
        for( i = 0; i < sTrimCharLen; i++ )
        {
            if( sStrByte == sTrimInfo[i].mTrimByte )
            {
                DTS_IS_EQUAL_SAME_BYTELEN_ONE_CHAR( sString,
                                                    sTrimInfo[i].mTrimStr,
                                                    sTrimInfo[i].mTrimByte,
                                                    & sIsEqual );
                if( sIsEqual == STL_TRUE )
                {
                    /* Match */
                    sString += sTrimInfo[i].mTrimByte;
                    sStringByteLen -= sTrimInfo[i].mTrimByte;
                    
                    break;
                }
                else
                {
                    /* No Match */
                    /* Do Nothing */                
                }
            }
            else
            {
                /* No Match */
                /* Do Nothing */                
            }
        }
        
        if( i < sTrimCharLen )
        {
            /* Match */
            continue;
        }
        else
        {
            /* Do Nothing */
        }
        
        STL_DASSERT( i == sTrimCharLen );
        
        if( sIsRemainTrimStr == STL_FALSE )
        {
            /* No Match */
            break;
        }
        else
        {
            /*
             * trim_character가 DTF_LTRIM_INFO_ARR_MAX characters를 넘는 경우로, 
             * sTrimInfo 를 구성하지 못한 나머지 trim string 처리.
             */
            
            STL_DASSERT( sTrimCharLen == DTF_LTRIM_INFO_ARR_MAX );
            
            sTempTrimStr = ( sTrimInfo[ sTrimCharLen - 1 ].mTrimStr + sTrimInfo[ sTrimCharLen - 1 ].mTrimByte );
            
            for( i = sTrimCharLen; i < sTrimCharLenOrg; i++ )
            {
                /**
                 * 체크할 sTrimStr의 첫번째 하나의 char의 length를 구한다.
                 */

                DTS_GET_UTF8_MBLENGTH( sTempTrimStr,
                                       & sTrimByte );
                
                if( sStrByte == sTrimByte )
                {
                    DTS_IS_EQUAL_SAME_BYTELEN_ONE_CHAR( sString,
                                                        sTempTrimStr,
                                                        sTrimByte,
                                                        & sIsEqual );
                    if( sIsEqual == STL_TRUE )
                    {
                        /* Match */
                        sString += sTrimByte;
                        sStringByteLen -= sTrimByte;
                        
                        break;
                    }
                    else
                    {
                        /* No Match */
                        /* Do Nothing */
                    }
                }
                else
                {
                    /* No Match */
                    /* Do Nothing */
                }
                
                /* trim_character의 다음 문자로 이동 */
                sTempTrimStr += sTrimByte;
            }
            
            if( i < sTrimCharLenOrg )
            {
                /* Match */
                continue;
            }
            else
            {
                /* No Match */
                break;
            }
        }
    }
    
    STL_RAMP( SET_RESULT_VALUE );
    
    stlMemcpy( DTF_VARCHAR( sResultValue ), sString, sStringByteLen );
    sResultValue->mLength = sStringByteLen;
    
    STL_DASSERT( sResultValue->mLength <= sResultValue->mBufferSize );
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_INVALID_CHARACTER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_CHARACTER_IN_CHARSET_REPERTOIRE,
                      NULL,
                      sErrorStack );  
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief uhc 에 대한 ltrim 처리
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> ltrim(VARCHAR, VARCHAR)
 *                               <BR> ltrim(LONGVARCHAR, LONGVARCHAR)
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR || LONGVARCHAR)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfStringLtrimForUhc( stlUInt16        aInputArgumentCnt,
                                dtlValueEntry  * aInputArgument,
                                void           * aResultValue,
                                dtlFuncVector  * aVectorFunc,
                                void           * aVectorArg,
                                void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;
    
    stlChar         * sString;
    stlChar         * sTrimStr;
    stlChar         * sTrimStrEnd;
    stlInt64          sStringByteLen = 0;
    stlInt8           sStrByte = 0;
    stlInt8           sTrimByte = 0;
    stlInt64          sTrimCharLen = 0;
    stlInt64          sTrimCharLenOrg = 0;    
    stlChar           sDefaultTrimChar[1];
    
    dtlCharacterSet   sCharSetID = DTL_UHC;
    stlInt64          i = 0;
    stlBool           sIsRemainTrimStr = STL_FALSE;
    stlBool           sIsEqual = STL_FALSE;

    stlChar         * sTempTrimStr;
    dtfLtrim          sTrimInfo[ DTF_LTRIM_INFO_ARR_MAX ];
    
    
    /**
     * Parameter Validation
     */
    
    DTL_PARAM_VALIDATE( ( aInputArgumentCnt >= 1 ) &&
                        ( aInputArgumentCnt <= 2 ),
                        (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_GROUP2( sValue1,
                          DTL_GROUP_CHAR_STRING,
                          DTL_GROUP_LONGVARCHAR_STRING,
                          sErrorStack );
    /* NULL value는 들어오지 않는다. */
    STL_DASSERT( DTL_IS_NULL( sValue1 ) != STL_TRUE );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue,
                           DTL_TYPE_VARCHAR,
                           DTL_TYPE_LONGVARCHAR,
                           sErrorStack );

    /*
     * sString : target data
     * sStringLen : target data의 길이
     */
    sString = DTF_VARCHAR( sValue1 );
    sStringByteLen = sValue1->mLength;

    if( aInputArgumentCnt == 1 )
    {
        /*
         * trim_character가 생략되었을 경우, ' '으로 처리됨.
         * 예) LTRIM( 'abcde' ) => LTRIM( 'abcde', ' ' )
         */

        /*
         * @todo
         * ' ' 는 character set에 맞는 code로 가져와야 한다.
         * character set에 맞는 CharLen, ByteLen을 구해야 함.
         */
        sDefaultTrimChar[0] = ' ';
        sTrimByte = 1;
        
        sTrimStr = sDefaultTrimChar;        
        
        while( sStringByteLen > 0 )
        {
            /**
             * 체크할 sString의 첫번째 하나의 char의 length를 구한다.
             */
            
            DTS_GET_UHC_MBLENGTH( sString,
                                  & sStrByte );
            
            if( sStrByte == sTrimByte )
            {
                if( sString[0] == sTrimStr[0] )
                {
                    /* Match */
                    sString += sTrimByte;
                    sStringByteLen -= sTrimByte;
                    
                    continue;
                }
                else
                {
                    /* No Match */
                    break;
                }
            }
            else
            {
                /* No Match */
                break;
            }
        }
        
        STL_THROW( SET_RESULT_VALUE );
    }
    else
    {
        sValue2 = aInputArgument[1].mValue.mDataValue;
        
        DTF_CHECK_DTL_GROUP2( sValue2,
                              DTL_GROUP_CHAR_STRING,
                              DTL_GROUP_LONGVARCHAR_STRING,
                              sErrorStack );
        /* NULL value는 들어오지 않는다. */
        STL_DASSERT( DTL_IS_NULL( sValue2 ) != STL_TRUE );
        
        sTrimStr = DTF_VARCHAR( sValue2 );
        sTrimStrEnd = sTrimStr + sValue2->mLength;
    }

    /*
     * trim_character 에 대한 정보구축
     */
    
    DTS_GET_UHC_MBLENGTH( sTrimStr,
                          & sTrimByte );
    
    i = 0;
    sTrimInfo[i].mTrimByte = sTrimByte;
    sTrimInfo[i].mTrimStr = sTrimStr;
    
    sTrimStr += sTrimInfo[i].mTrimByte;
    i++;
    
    while( ( sTrimStr < sTrimStrEnd ) && ( i < DTF_LTRIM_INFO_ARR_MAX )  )
    {
        DTS_GET_UHC_MBLENGTH( sTrimStr,
                              & sTrimByte );
        
        sTrimInfo[i].mTrimByte = sTrimByte;
        sTrimInfo[i].mTrimStr = sTrimStr;
        
        sTrimStr += sTrimInfo[i].mTrimByte;
        i++;
    }
    
    if( sTrimStr < sTrimStrEnd )
    {
        sIsRemainTrimStr = STL_TRUE;
        
        /* trim_character의 문자길이를 구한다. */
        STL_TRY( dtsGetMbstrlenWithLenFuncList[ sCharSetID ]( sTrimStr,
                                                              ( sTrimStrEnd - sTrimStr ),
                                                              & sTrimCharLen,
                                                              aVectorFunc,
                                                              aVectorArg,
                                                              sErrorStack )
                 == STL_SUCCESS );
        
        sTrimCharLenOrg = i + sTrimCharLen;
    }
    else
    {
        STL_DASSERT( sTrimStr == sTrimStrEnd );
        
        sIsRemainTrimStr = STL_FALSE;
        /* trim_character의 문자길이를 구한다. */        
        sTrimCharLenOrg = i;
    }
    
    /*
     * ltrim
     */
    
    sTrimCharLen = ( sTrimCharLenOrg <= DTF_LTRIM_INFO_ARR_MAX ) ? sTrimCharLenOrg : DTF_LTRIM_INFO_ARR_MAX;
    sTrimStr = DTF_VARCHAR( sValue2 );
    while( sStringByteLen > 0 )
    {
        /**
         * 체크할 sString의 첫번째 하나의 char의 length를 구한다.
         */

        DTS_GET_UHC_MBLENGTH( sString,
                              &sStrByte );
        
        for( i = 0; i < sTrimCharLen; i++ )
        {
            if( sStrByte == sTrimInfo[i].mTrimByte )
            {
                DTS_IS_EQUAL_SAME_BYTELEN_ONE_CHAR( sString,
                                                    sTrimInfo[i].mTrimStr,
                                                    sTrimInfo[i].mTrimByte,
                                                    & sIsEqual );
                if( sIsEqual == STL_TRUE )
                {
                    /* Match */
                    sString += sTrimInfo[i].mTrimByte;
                    sStringByteLen -= sTrimInfo[i].mTrimByte;
                    
                    break;
                }
                else
                {
                    /* No Match */
                    /* Do Nothing */                
                }
            }
            else
            {
                /* No Match */
                /* Do Nothing */                
            }
        }
        
        if( i < sTrimCharLen )
        {
            /* Match */
            continue;
        }
        else
        {
            /* Do Nothing */
        }
        
        STL_DASSERT( i == sTrimCharLen );
        
        if( sIsRemainTrimStr == STL_FALSE )
        {
            /* No Match */
            break;
        }
        else
        {
            /*
             * trim_character가 DTF_LTRIM_INFO_ARR_MAX characters를 넘는 경우로, 
             * sTrimInfo 를 구성하지 못한 나머지 trim string 처리.
             */
            
            STL_DASSERT( sTrimCharLen == DTF_LTRIM_INFO_ARR_MAX );
            
            sTempTrimStr = ( sTrimInfo[ sTrimCharLen - 1 ].mTrimStr + sTrimInfo[ sTrimCharLen - 1 ].mTrimByte );
            
            for( i = sTrimCharLen; i < sTrimCharLenOrg; i++ )
            {
                /**
                 * 체크할 sTrimStr의 첫번째 하나의 char의 length를 구한다.
                 */

                DTS_GET_UHC_MBLENGTH( sTempTrimStr,
                                      & sTrimByte );
                
                if( sStrByte == sTrimByte )
                {
                    DTS_IS_EQUAL_SAME_BYTELEN_ONE_CHAR( sString,
                                                        sTempTrimStr,
                                                        sTrimByte,
                                                        & sIsEqual );
                    if( sIsEqual == STL_TRUE )
                    {
                        /* Match */
                        sString += sTrimByte;
                        sStringByteLen -= sTrimByte;
                        
                        break;
                    }
                    else
                    {
                        /* No Match */
                        /* Do Nothing */
                    }
                }
                else
                {
                    /* No Match */
                    /* Do Nothing */
                }
                
                /* trim_character의 다음 문자로 이동 */
                sTempTrimStr += sTrimByte;
            }
            
            if( i < sTrimCharLenOrg )
            {
                /* Match */
                continue;
            }
            else
            {
                /* No Match */
                break;
            }
        }
    }
    
    STL_RAMP( SET_RESULT_VALUE );
    
    stlMemcpy( DTF_VARCHAR( sResultValue ), sString, sStringByteLen );
    sResultValue->mLength = sStringByteLen;
    
    STL_DASSERT( sResultValue->mLength <= sResultValue->mBufferSize );
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief gb18030 에 대한 ltrim 처리
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> ltrim(VARCHAR, VARCHAR)
 *                               <BR> ltrim(LONGVARCHAR, LONGVARCHAR)
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR || LONGVARCHAR)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfStringLtrimForGb18030( stlUInt16        aInputArgumentCnt,
                                    dtlValueEntry  * aInputArgument,
                                    void           * aResultValue,
                                    dtlFuncVector  * aVectorFunc,
                                    void           * aVectorArg,
                                    void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;
    
    stlChar         * sString;
    stlChar         * sTrimStr;
    stlChar         * sTrimStrEnd;
    stlInt64          sStringByteLen = 0;
    stlInt8           sStrByte = 0;
    stlInt8           sTrimByte = 0;
    stlInt64          sTrimCharLen = 0;
    stlInt64          sTrimCharLenOrg = 0;    
    stlChar           sDefaultTrimChar[1];
    
    dtlCharacterSet   sCharSetID = DTL_GB18030;
    stlInt64          i = 0;
    stlBool           sIsRemainTrimStr = STL_FALSE;
    stlBool           sIsEqual = STL_FALSE;

    stlChar         * sTempTrimStr;
    dtfLtrim          sTrimInfo[ DTF_LTRIM_INFO_ARR_MAX ];
    
    
    /**
     * Parameter Validation
     */
    
    DTL_PARAM_VALIDATE( ( aInputArgumentCnt >= 1 ) &&
                        ( aInputArgumentCnt <= 2 ),
                        (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_GROUP2( sValue1,
                          DTL_GROUP_CHAR_STRING,
                          DTL_GROUP_LONGVARCHAR_STRING,
                          sErrorStack );
    /* NULL value는 들어오지 않는다. */
    STL_DASSERT( DTL_IS_NULL( sValue1 ) != STL_TRUE );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue,
                           DTL_TYPE_VARCHAR,
                           DTL_TYPE_LONGVARCHAR,
                           sErrorStack );

    /*
     * sString : target data
     * sStringLen : target data의 길이
     */
    sString = DTF_VARCHAR( sValue1 );
    sStringByteLen = sValue1->mLength;

    if( aInputArgumentCnt == 1 )
    {
        /*
         * trim_character가 생략되었을 경우, ' '으로 처리됨.
         * 예) LTRIM( 'abcde' ) => LTRIM( 'abcde', ' ' )
         */

        /*
         * @todo
         * ' ' 는 character set에 맞는 code로 가져와야 한다.
         * character set에 맞는 CharLen, ByteLen을 구해야 함.
         */
        sDefaultTrimChar[0] = ' ';
        sTrimByte = 1;
        
        sTrimStr = sDefaultTrimChar;        
        
        while( sStringByteLen > 0 )
        {
            /**
             * 체크할 sString의 첫번째 하나의 char의 length를 구한다.
             */
            
            DTS_GET_GB18030_MBLENGTH( sString,
                                      & sStrByte );
            
            if( sStrByte == sTrimByte )
            {
                if( sString[0] == sTrimStr[0] )
                {
                    /* Match */
                    sString += sTrimByte;
                    sStringByteLen -= sTrimByte;
                    
                    continue;
                }
                else
                {
                    /* No Match */
                    break;
                }
            }
            else
            {
                /* No Match */
                break;
            }
        }
        
        STL_THROW( SET_RESULT_VALUE );
    }
    else
    {
        sValue2 = aInputArgument[1].mValue.mDataValue;
        
        DTF_CHECK_DTL_GROUP2( sValue2,
                              DTL_GROUP_CHAR_STRING,
                              DTL_GROUP_LONGVARCHAR_STRING,
                              sErrorStack );
        /* NULL value는 들어오지 않는다. */
        STL_DASSERT( DTL_IS_NULL( sValue2 ) != STL_TRUE );
        
        sTrimStr = DTF_VARCHAR( sValue2 );
        sTrimStrEnd = sTrimStr + sValue2->mLength;
    }

    /*
     * trim_character 에 대한 정보구축
     */
    
    DTS_GET_GB18030_MBLENGTH( sTrimStr,
                              & sTrimByte );
    
    i = 0;
    sTrimInfo[i].mTrimByte = sTrimByte;
    sTrimInfo[i].mTrimStr = sTrimStr;
    
    sTrimStr += sTrimInfo[i].mTrimByte;
    i++;
    
    while( ( sTrimStr < sTrimStrEnd ) && ( i < DTF_LTRIM_INFO_ARR_MAX )  )
    {
        DTS_GET_GB18030_MBLENGTH( sTrimStr,
                                  & sTrimByte );
        
        sTrimInfo[i].mTrimByte = sTrimByte;
        sTrimInfo[i].mTrimStr = sTrimStr;
        
        sTrimStr += sTrimInfo[i].mTrimByte;
        i++;
    }
    
    if( sTrimStr < sTrimStrEnd )
    {
        sIsRemainTrimStr = STL_TRUE;
        
        /* trim_character의 문자길이를 구한다. */
        STL_TRY( dtsGetMbstrlenWithLenFuncList[ sCharSetID ]( sTrimStr,
                                                              ( sTrimStrEnd - sTrimStr ),
                                                              & sTrimCharLen,
                                                              aVectorFunc,
                                                              aVectorArg,
                                                              sErrorStack )
                 == STL_SUCCESS );
        
        sTrimCharLenOrg = i + sTrimCharLen;
    }
    else
    {
        STL_DASSERT( sTrimStr == sTrimStrEnd );
        
        sIsRemainTrimStr = STL_FALSE;
        /* trim_character의 문자길이를 구한다. */        
        sTrimCharLenOrg = i;
    }
    
    /*
     * ltrim
     */
    
    sTrimCharLen = ( sTrimCharLenOrg <= DTF_LTRIM_INFO_ARR_MAX ) ? sTrimCharLenOrg : DTF_LTRIM_INFO_ARR_MAX;
    sTrimStr = DTF_VARCHAR( sValue2 );
    while( sStringByteLen > 0 )
    {
        /**
         * 체크할 sString의 첫번째 하나의 char의 length를 구한다.
         */

        DTS_GET_GB18030_MBLENGTH( sString,
                                  &sStrByte );
        
        for( i = 0; i < sTrimCharLen; i++ )
        {
            if( sStrByte == sTrimInfo[i].mTrimByte )
            {
                DTS_IS_EQUAL_SAME_BYTELEN_ONE_CHAR( sString,
                                                    sTrimInfo[i].mTrimStr,
                                                    sTrimInfo[i].mTrimByte,
                                                    & sIsEqual );
                if( sIsEqual == STL_TRUE )
                {
                    /* Match */
                    sString += sTrimInfo[i].mTrimByte;
                    sStringByteLen -= sTrimInfo[i].mTrimByte;
                    
                    break;
                }
                else
                {
                    /* No Match */
                    /* Do Nothing */                
                }
            }
            else
            {
                /* No Match */
                /* Do Nothing */                
            }
        }
        
        if( i < sTrimCharLen )
        {
            /* Match */
            continue;
        }
        else
        {
            /* Do Nothing */
        }
        
        STL_DASSERT( i == sTrimCharLen );
        
        if( sIsRemainTrimStr == STL_FALSE )
        {
            /* No Match */
            break;
        }
        else
        {
            /*
             * trim_character가 DTF_LTRIM_INFO_ARR_MAX characters를 넘는 경우로, 
             * sTrimInfo 를 구성하지 못한 나머지 trim string 처리.
             */
            
            STL_DASSERT( sTrimCharLen == DTF_LTRIM_INFO_ARR_MAX );
            
            sTempTrimStr = ( sTrimInfo[ sTrimCharLen - 1 ].mTrimStr + sTrimInfo[ sTrimCharLen - 1 ].mTrimByte );
            
            for( i = sTrimCharLen; i < sTrimCharLenOrg; i++ )
            {
                /**
                 * 체크할 sTrimStr의 첫번째 하나의 char의 length를 구한다.
                 */

                DTS_GET_GB18030_MBLENGTH( sTempTrimStr,
                                          & sTrimByte );
                
                if( sStrByte == sTrimByte )
                {
                    DTS_IS_EQUAL_SAME_BYTELEN_ONE_CHAR( sString,
                                                        sTempTrimStr,
                                                        sTrimByte,
                                                        & sIsEqual );
                    if( sIsEqual == STL_TRUE )
                    {
                        /* Match */
                        sString += sTrimByte;
                        sStringByteLen -= sTrimByte;
                        
                        break;
                    }
                    else
                    {
                        /* No Match */
                        /* Do Nothing */
                    }
                }
                else
                {
                    /* No Match */
                    /* Do Nothing */
                }
                
                /* trim_character의 다음 문자로 이동 */
                sTempTrimStr += sTrimByte;
            }
            
            if( i < sTrimCharLenOrg )
            {
                /* Match */
                continue;
            }
            else
            {
                /* No Match */
                break;
            }
        }
    }
    
    STL_RAMP( SET_RESULT_VALUE );
    
    stlMemcpy( DTF_VARCHAR( sResultValue ), sString, sStringByteLen );
    sResultValue->mLength = sStringByteLen;
    
    STL_DASSERT( sResultValue->mLength <= sResultValue->mBufferSize );
    
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
 * @brief LTRIM( trim_source, trim_character )
 *        <BR> LTRIM( trim_source, trim_character )
 *        <BR>   LTRIM( trim_source, trim_character ) => [ P, O ]
 *        <BR> ex) LTRIM( X'AABBCC', 'AA' ) => BBCC
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> ltrim(VARBINARY, VARBINARY)
 *                               <BR> ltrim(LONGVARBINARY, LONGVARBINARY)
 * @param[out] aResultValue      연산 결과 (결과타입 VARBINARY || LONGVARBINARY)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfBinaryStringLtrim( stlUInt16        aInputArgumentCnt,
                                dtlValueEntry  * aInputArgument,
                                void           * aResultValue,
                                dtlFuncVector  * aVectorFunc,
                                void           * aVectorArg,
                                void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2;
    dtlDataValue    * sResultValue;
    
    stlChar         * sString;
    stlChar         * sTrimStr;
    stlInt64          sStringLen;
    stlInt64          sTrimLen;
    stlChar           sDefaultTrimChar[1];

    stlInt64          i;
    
    /**
     * Parameter Validation
     */
    
    DTL_PARAM_VALIDATE( ( aInputArgumentCnt >= 1 ) &&
                        ( aInputArgumentCnt <= 2 ),
                        (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    
    DTF_CHECK_DTL_GROUP2( sValue1,
                          DTL_GROUP_BINARY_STRING,
                          DTL_GROUP_LONGVARBINARY_STRING,
                          (stlErrorStack *)aEnv );
    /* NULL value는 들어오지 않는다. */
    STL_DASSERT( DTL_IS_NULL( sValue1 ) != STL_TRUE );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue,
                           DTL_TYPE_VARBINARY,
                           DTL_TYPE_LONGVARBINARY,
                           (stlErrorStack *)aEnv );
    
    if( aInputArgumentCnt == 1 )
    {
        /*
         * trim_character가 생략되었을 경우, X'00'으로 처리됨.
         * 예) LTRIM( X'AABBCC' ) => LTRIM( X'AABBCC', X'00' )
         */

        *(stlInt8 *)sDefaultTrimChar = 0;
        
        sTrimStr = sDefaultTrimChar;
        sTrimLen = 1;
    }
    else
    {
        sValue2 = aInputArgument[1].mValue.mDataValue;
        
        DTF_CHECK_DTL_GROUP2( sValue2,
                              DTL_GROUP_BINARY_STRING,
                              DTL_GROUP_LONGVARBINARY_STRING,
                              (stlErrorStack *)aEnv );
        /* NULL value는 들어오지 않는다. */
        STL_DASSERT( DTL_IS_NULL( sValue2 ) != STL_TRUE );
            
        
        sTrimStr = DTF_VARBINARY( sValue2 );
        sTrimLen = sValue2->mLength;
    }
    
    /*
     * sString : target data
     * sStringLen : target data의 길이
     */
    sString = DTF_VARBINARY( sValue1 );
    sStringLen = sValue1->mLength;
    
    /*
     * ltrim
     */
    while( sStringLen > 0 )
    {
        for( i = 0; i < sTrimLen; i++ )
        {
            if( *sString == sTrimStr[i] )
            {
                /* Match*/
                sString++;
                sStringLen--;
                
                break;
            }
            else
            {
                /* Do Nothing */
            }
        }
        
        if( i < sTrimLen )
        {
            /* Match*/
            /* Do Nothing */
        }
        else
        {
            /* No Match */            
            STL_DASSERT( i == sTrimLen );
            
            break;
        }
    }        
    
    stlMemcpy( DTF_VARBINARY( sResultValue ), sString, sStringLen );
    sResultValue->mLength = sStringLen;
    
    STL_DASSERT( sResultValue->mLength <= sResultValue->mBufferSize );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}



/** @} */
