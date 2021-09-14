/*******************************************************************************
 * dtfRtrim.c
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
 * @file dtfRtrim.c
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

#define DTF_RTRIM_INFO_ARR_MAX ( 500 )

typedef struct dtfRtrim
{
    stlInt8    mTrimByte;
    stlChar    mAlign1[3];   /**< 8byte align을 위한 padding */
#if (STL_SIZEOF_VOIDP == 8)
    stlUInt32  mAlign2;      /**< 8byte align을 위한 추가 padding */
#endif 
    stlChar  * mTrimStr;        
} dtfRtrim;


stlStatus dtfStringRtrimForAscii( stlUInt16        aInputArgumentCnt,
                                  dtlValueEntry  * aInputArgument,
                                  void           * aResultValue,
                                  dtlFuncVector  * aVectorFunc,
                                  void           * aVectorArg,
                                  void           * aEnv );

stlStatus dtfStringRtrimForUtf8( stlUInt16        aInputArgumentCnt,
                                 dtlValueEntry  * aInputArgument,
                                 void           * aResultValue,
                                 dtlFuncVector  * aVectorFunc,
                                 void           * aVectorArg,
                                 void           * aEnv );

stlStatus dtfStringRtrimForUhc( stlUInt16        aInputArgumentCnt,
                                dtlValueEntry  * aInputArgument,
                                void           * aResultValue,
                                dtlFuncVector  * aVectorFunc,
                                void           * aVectorArg,
                                void           * aEnv );

stlStatus dtfStringRtrimForGb18030( stlUInt16        aInputArgumentCnt,
                                    dtlValueEntry  * aInputArgument,
                                    void           * aResultValue,
                                    dtlFuncVector  * aVectorFunc,
                                    void           * aVectorArg,
                                    void           * aEnv );

dtfStringRtrimFunc dtfStringRtrimFuncList[ DTL_CHARACTERSET_MAX + 1 ] =
{
    dtfStringRtrimForAscii,
    dtfStringRtrimForUtf8,
    dtfStringRtrimForUhc,
    dtfStringRtrimForGb18030,
    
    NULL
};

/*******************************************************************************
 * RTRIM( trim_source, trim_character )
 *   RTRIM( trim_source, trim_character ) => [ P, O ]
 * ex) RTRIM( '___TRIM FUNCTION___', '_' ) => ___TRIM FUNCTION
 *******************************************************************************/

/**
 * @brief RTRIM( trim_source, trim_character )
 *        <BR> RTRIM( trim_source, trim_character )
 *        <BR>   RTRIM( trim_source, trim_character ) => [ P, O ]
 *        <BR> ex) RTRIM( '___TRIM FUNCTION___', '_' ) => ___TRIM FUNCTION
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> rtrim(VARCHAR, VARCHAR)
 *                               <BR> rtrim(LONGVARCHAR, LONGVARCHAR)
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR || LONGVARCHAR)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfStringRtrim( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv )
{
    /* character set이 추가될때 그에 따른 function list가 누락되는 일이 없도록 하기 위한 검사 코드 */
    DTF_CHECK_CHAR_SET_FUNC_LIST( dtfStringRtrimFuncList );
    
    return dtfStringRtrimFuncList[ aVectorFunc->mGetCharSetIDFunc(aVectorArg) ](
        aInputArgumentCnt,
        aInputArgument,
        aResultValue,
        aVectorFunc,
        aVectorArg,
        aEnv );
}


/**
 * @brief ascii에 대한 rtrim 처리. RTRIM( trim_source, trim_character )
 *        <BR> RTRIM( trim_source, trim_character )
 *        <BR>   RTRIM( trim_source, trim_character ) => [ P, O ]
 *        <BR> ex) RTRIM( '___TRIM FUNCTION___', '_' ) => ___TRIM FUNCTION
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> rtrim(VARCHAR, VARCHAR)
 *                               <BR> rtrim(LONGVARCHAR, LONGVARCHAR)
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR || LONGVARCHAR)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfStringRtrimForAscii( stlUInt16        aInputArgumentCnt,
                                  dtlValueEntry  * aInputArgument,
                                  void           * aResultValue,
                                  dtlFuncVector  * aVectorFunc,
                                  void           * aVectorArg,
                                  void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2 = NULL;
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;
    
    stlChar         * sString;
    stlChar         * sTrimStr;
    stlChar         * sTrimStrEnd = NULL;
    stlInt64          sStringByteLen = 0;
    stlInt64          sStrByte = 0;
    stlInt8           sTrimByte = 0;
    stlInt64          sTrimCharLen = 0;
    stlInt64          sTrimCharLenOrg = 0;    
    stlChar           sDefaultTrimChar[1];
    
    dtlCharacterSet   sCharSetID = DTL_SQL_ASCII;
    stlInt64          i = 0;
    stlBool           sIsRemainTrimStr = STL_FALSE;
    stlBool           sIsEqual = STL_FALSE;
    
    stlInt8           sTempLen  = 0;
    stlInt8           sTempLen2 = 0;
    stlInt8           sMaxCharLen = DTS_GET_ASCII_MB_MAX_LENGTH;
    stlBool           sVerify;
    
    stlChar         * sTempTrimStr;
    dtfRtrim          sTrimInfo[ DTF_RTRIM_INFO_ARR_MAX ];

    
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
    sString = DTF_VARCHAR( sValue1 ) + sValue1->mLength;
    sStringByteLen = sValue1->mLength;
    
    if( aInputArgumentCnt == 1 )
    {
        /*
         * trim_character가 생략되었을 경우, ' '으로 처리됨.
         * 예) RTRIM( 'abcde' ) => RTRIM( 'abcde', ' ' )
         */
        
        /*
         * @todo
         * ' ' 는 character set에 맞는 code로 가져와야 한다.
         * character set에 맞는 CharLen, ByteLen을 구해야 함.
         */
        sDefaultTrimChar[0] = ' ';
        sTrimByte = 1;
        sTrimCharLen = 1;
        
        sTrimStr = sDefaultTrimChar;        
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
        
        /*
         * trim_character 에 대한 정보구축
         * 우선, trim_character가 one character인지를 알아보기 위해 여기서 한자만 체크한다.
         */
        
        DTS_GET_ASCII_MBLENGTH( sTrimStr,
                                & sTrimByte );
        
        if( sTrimByte == sValue2->mLength )
        {
            sTrimCharLen = 1;
        }
        else
        {
            /* Do Nothing */
        }
    }
    
    if( sTrimCharLen == 1 )
    {
        /*
         * trim_character가 1인 경우
         */
        
        while( sStringByteLen > 0  )
        {
            /**
             * 체크할 sString의 마지막 하나의 char의 length를 구한다.
             */
            
            if( sStringByteLen < sMaxCharLen )
            {
                sMaxCharLen = sStringByteLen;
            }
            else
            {
                /* Do Noghint */
            }
            
            for( i = sMaxCharLen; i > 0; i-- )
            {
                DTS_IS_VERIFY_ASCII( &sTempLen2,
                                     &sVerify );
                
                if ( sVerify == STL_TRUE )
                {
                    sStrByte = sTempLen2;
                    /*
                     * char를 찾았음으로 1Char만큼 건너뛰도록.
                     */
                    i = i - sStrByte + 1;
                }
                else
                {
                    /* Do Nothing */
                }
                /*
                 * 마지막에 찾은 char의 length의 길이가 sTempLen에 저장된다.
                 */
            }
        
            if( sStrByte == sTrimByte )
            {
                DTS_IS_EQUAL_SAME_BYTELEN_ONE_CHAR( ( sString - sStrByte ),
                                                    sTrimStr,
                                                    sTrimByte,
                                                    & sIsEqual );
                if( sIsEqual == STL_TRUE )
                {
                    /* Match */
                    sString -= sTrimByte;
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
        /* Do Nothing */
    }
    
    i = 0;
    sTrimInfo[i].mTrimByte = sTrimByte;
    sTrimInfo[i].mTrimStr = sTrimStr;
    
    sTrimStr += sTrimInfo[i].mTrimByte;
    i++;

    while( ( sTrimStr < sTrimStrEnd ) && ( i < DTF_RTRIM_INFO_ARR_MAX )  )
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
     * rtrim
     */
    
    sTrimCharLen = ( sTrimCharLenOrg <= DTF_RTRIM_INFO_ARR_MAX ) ? sTrimCharLenOrg : DTF_RTRIM_INFO_ARR_MAX;
    sTrimStr = DTF_VARCHAR( sValue2 );

    while( sStringByteLen > 0 )
    {
        /**
         * 체크할 sString의 마지막 하나의 char의 length를 구한다.
         */

        if( sStringByteLen < sMaxCharLen )
        {
            sMaxCharLen = sStringByteLen;
        }
        else
        {
            /* Do Noghint */
        }
        
        for( i = sMaxCharLen; i > 0; i-- )
        {
            DTS_IS_VERIFY_ASCII( &sTempLen2,
                                 &sVerify );
            
            if ( sVerify == STL_TRUE )
            {
                sTempLen = sTempLen2;
                /*
                 * char를 찾았음으로 1Char만큼 건너뛰도록.
                 */
                i = i - sTempLen + 1;
            }
            else
            {
                /* Do Nothing */
            }
            /*
             * 마지막에 찾은 char의 length의 길이가 sTempLen에 저장된다.
             */
        }
        
        sStrByte = sTempLen;
        
        for( i = 0; i < sTrimCharLen; i++ )
        {
            if( sStrByte == sTrimInfo[i].mTrimByte )
            {
                DTS_IS_EQUAL_SAME_BYTELEN_ONE_CHAR( (sString - sStrByte),
                                                    sTrimInfo[i].mTrimStr,
                                                    sTrimInfo[i].mTrimByte,
                                                    & sIsEqual );
                if( sIsEqual == STL_TRUE )                    
                {
                    /* Match */
                    sString -= sTrimInfo[i].mTrimByte;
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

            STL_DASSERT( sTrimCharLen == DTF_RTRIM_INFO_ARR_MAX );
            
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
                    DTS_IS_EQUAL_SAME_BYTELEN_ONE_CHAR( (sString - sStrByte),
                                                        sTempTrimStr,
                                                        sTrimByte,
                                                        & sIsEqual );
                    if( sIsEqual == STL_TRUE )
                    {
                        /* Match */
                        sString -= sTrimByte;
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
    
    stlMemcpy( DTF_VARCHAR( sResultValue ), DTF_VARCHAR( sValue1 ), sStringByteLen );
    sResultValue->mLength = sStringByteLen;
    
    STL_DASSERT( sResultValue->mLength <= sResultValue->mBufferSize );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief utf8에 대한 rtrim 처리. RTRIM( trim_source, trim_character )
 *        <BR> RTRIM( trim_source, trim_character )
 *        <BR>   RTRIM( trim_source, trim_character ) => [ P, O ]
 *        <BR> ex) RTRIM( '___TRIM FUNCTION___', '_' ) => ___TRIM FUNCTION
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> rtrim(VARCHAR, VARCHAR)
 *                               <BR> rtrim(LONGVARCHAR, LONGVARCHAR)
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR || LONGVARCHAR)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfStringRtrimForUtf8( stlUInt16        aInputArgumentCnt,
                                 dtlValueEntry  * aInputArgument,
                                 void           * aResultValue,
                                 dtlFuncVector  * aVectorFunc,
                                 void           * aVectorArg,
                                 void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2 = NULL;
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;
    
    stlChar         * sString;
    stlChar         * sTrimStr;
    stlChar         * sTrimStrEnd = NULL;
    stlInt64          sStringByteLen = 0;
    stlInt64          sStrByte = 0;
    stlInt8           sTrimByte = 0;
    stlInt64          sTrimCharLen = 0;
    stlInt64          sTrimCharLenOrg = 0;    
    stlChar           sDefaultTrimChar[1];
    
    dtlCharacterSet   sCharSetID = DTL_UTF8;
    stlInt64          i = 0;
    stlBool           sIsRemainTrimStr = STL_FALSE;
    stlBool           sIsEqual = STL_FALSE;

    stlInt8           sTempLen  = 0;
    stlInt8           sTempLen2 = 0;
    stlInt8           sMaxCharLen = DTS_GET_UTF8_MB_MAX_LENGTH;
    stlBool           sVerify;
    
    stlChar         * sTempTrimStr;
    dtfRtrim          sTrimInfo[ DTF_RTRIM_INFO_ARR_MAX ];

    
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
    sString = DTF_VARCHAR( sValue1 ) + sValue1->mLength;
    sStringByteLen = sValue1->mLength;
    
    if( aInputArgumentCnt == 1 )
    {
        /*
         * trim_character가 생략되었을 경우, ' '으로 처리됨.
         * 예) RTRIM( 'abcde' ) => RTRIM( 'abcde', ' ' )
         */
        
        /*
         * @todo
         * ' ' 는 character set에 맞는 code로 가져와야 한다.
         * character set에 맞는 CharLen, ByteLen을 구해야 함.
         */
        sDefaultTrimChar[0] = ' ';
        sTrimByte = 1;
        sTrimCharLen = 1;
        
        sTrimStr = sDefaultTrimChar;        
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
        
        /*
         * trim_character 에 대한 정보구축
         * 우선, trim_character가 one character인지를 알아보기 위해 여기서 한자만 체크한다.
         */
        
        DTS_GET_UTF8_MBLENGTH( sTrimStr,
                               & sTrimByte );
        
        if( sTrimByte == sValue2->mLength )
        {
            sTrimCharLen = 1;
        }
        else
        {
            /* Do Nothing */
        }
    }
    
    if( sTrimCharLen == 1 )
    {
        /*
         * trim_character가 1인 경우
         */
        
        while( sStringByteLen > 0  )
        {
            /**
             * 체크할 sString의 마지막 하나의 char의 length를 구한다.
             */
            
            if( sStringByteLen < sMaxCharLen )
            {
                sMaxCharLen = sStringByteLen;
            }
            else
            {
                /* Do Noghint */
            }
            
            for( i = sMaxCharLen; i > 0; i-- )
            {
                DTS_IS_VERIFY_UTF8( sString - i,
                                    i,
                                    &sTempLen2,
                                    &sVerify );
                
                if ( sVerify == STL_TRUE )
                {
                    sStrByte = sTempLen2;
                    /*
                     * char를 찾았음으로 1Char만큼 건너뛰도록.
                     */
                    i = i - sStrByte + 1;
                }
                else
                {
                    /* Do Nothing */
                }
                /*
                 * 마지막에 찾은 char의 length의 길이가 sTempLen에 저장된다.
                 */
            }
        
            if( sStrByte == sTrimByte )
            {
                DTS_IS_EQUAL_SAME_BYTELEN_ONE_CHAR( ( sString - sStrByte ),
                                                    sTrimStr,
                                                    sTrimByte,
                                                    & sIsEqual );
                if( sIsEqual == STL_TRUE )
                {
                    /* Match */
                    sString -= sTrimByte;
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
        /* Do Nothing */
    }
    
    i = 0;
    sTrimInfo[i].mTrimByte = sTrimByte;
    sTrimInfo[i].mTrimStr = sTrimStr;
    
    sTrimStr += sTrimInfo[i].mTrimByte;
    i++;

    while( ( sTrimStr < sTrimStrEnd ) && ( i < DTF_RTRIM_INFO_ARR_MAX )  )
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
     * rtrim
     */
    
    sTrimCharLen = ( sTrimCharLenOrg <= DTF_RTRIM_INFO_ARR_MAX ) ? sTrimCharLenOrg : DTF_RTRIM_INFO_ARR_MAX;
    sTrimStr = DTF_VARCHAR( sValue2 );
    
    while( sStringByteLen > 0 )
    {
        /**
         * 체크할 sString의 마지막 하나의 char의 length를 구한다.
         */
        
        if( sStringByteLen < sMaxCharLen )
        {
            sMaxCharLen = sStringByteLen;
        }
        else
        {
            /* Do Noghint */
        }
        
        for( i = sMaxCharLen; i > 0; i-- )
        {
            DTS_IS_VERIFY_UTF8( sString - i,
                                i,
                                &sTempLen2,
                                &sVerify );
            
            if ( sVerify == STL_TRUE )
            {
                sTempLen = sTempLen2;
                /*
                 * char를 찾았음으로 1Char만큼 건너뛰도록.
                 */
                i = i - sTempLen + 1;
            }
            else
            {
                /* Do Nothing */
            }
            /*
             * 마지막에 찾은 char의 length의 길이가 sTempLen에 저장된다.
             */
        }
        
        sStrByte = sTempLen;
        
        for( i = 0; i < sTrimCharLen; i++ )
        {
            if( sStrByte == sTrimInfo[i].mTrimByte )
            {
                DTS_IS_EQUAL_SAME_BYTELEN_ONE_CHAR( (sString - sStrByte),
                                                    sTrimInfo[i].mTrimStr,
                                                    sTrimInfo[i].mTrimByte,
                                                    & sIsEqual );
                if( sIsEqual == STL_TRUE )                    
                {
                    /* Match */
                    sString -= sTrimInfo[i].mTrimByte;
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

            STL_DASSERT( sTrimCharLen == DTF_RTRIM_INFO_ARR_MAX );
            
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
                    DTS_IS_EQUAL_SAME_BYTELEN_ONE_CHAR( (sString - sStrByte),
                                                        sTempTrimStr,
                                                        sTrimByte,
                                                        & sIsEqual );
                    if( sIsEqual == STL_TRUE )
                    {
                        /* Match */
                        sString -= sTrimByte;
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
    
    stlMemcpy( DTF_VARCHAR( sResultValue ), DTF_VARCHAR( sValue1 ), sStringByteLen );
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
 * @brief uhc에 대한 rtrim 처리. RTRIM( trim_source, trim_character )
 *        <BR> RTRIM( trim_source, trim_character )
 *        <BR>   RTRIM( trim_source, trim_character ) => [ P, O ]
 *        <BR> ex) RTRIM( '___TRIM FUNCTION___', '_' ) => ___TRIM FUNCTION
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> rtrim(VARCHAR, VARCHAR)
 *                               <BR> rtrim(LONGVARCHAR, LONGVARCHAR)
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR || LONGVARCHAR)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfStringRtrimForUhc( stlUInt16        aInputArgumentCnt,
                                dtlValueEntry  * aInputArgument,
                                void           * aResultValue,
                                dtlFuncVector  * aVectorFunc,
                                void           * aVectorArg,
                                void           * aEnv )
{
    dtlDataValue    * sValue1 = NULL;
    dtlDataValue    * sValue2 = NULL;
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;
    
    stlChar         * sString = NULL;
    stlChar         * sTrimStr = NULL;
    stlChar         * sTrimStrEnd = NULL;
    stlInt64          sStringByteLen = 0;
    stlInt64          sStrByte = 0;
    stlInt8           sTrimByte = 0;
    stlInt64          sTrimCharLen = 0;
    stlInt64          sTrimCharLenOrg = 0;    
    stlChar           sDefaultTrimChar[1];
    
    dtlCharacterSet   sCharSetID = DTL_UHC;
    stlInt64          i = 0;
    stlBool           sIsRemainTrimStr = STL_FALSE;
    stlBool           sIsEqual = STL_FALSE;

    stlInt8           sTempLen  = 0;
    stlInt8           sTempLen2 = 0;
    stlInt8           sMaxCharLen = DTS_GET_UHC_MB_MAX_LENGTH;
    stlBool           sVerify;
    
    stlChar         * sTempTrimStr;
    dtfRtrim          sTrimInfo[ DTF_RTRIM_INFO_ARR_MAX ];

    
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
    sString = DTF_VARCHAR( sValue1 ) + sValue1->mLength;
    sStringByteLen = sValue1->mLength;
    
    if( aInputArgumentCnt == 1 )
    {
        /*
         * trim_character가 생략되었을 경우, ' '으로 처리됨.
         * 예) RTRIM( 'abcde' ) => RTRIM( 'abcde', ' ' )
         */
        
        /*
         * @todo
         * ' ' 는 character set에 맞는 code로 가져와야 한다.
         * character set에 맞는 CharLen, ByteLen을 구해야 함.
         */
        sDefaultTrimChar[0] = ' ';
        sTrimByte = 1;
        sTrimCharLen = 1;
        
        sTrimStr = sDefaultTrimChar;        
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
        
        /*
         * trim_character 에 대한 정보구축
         * 우선, trim_character가 one character인지를 알아보기 위해 여기서 한자만 체크한다.
         */
        
        DTS_GET_UHC_MBLENGTH( sTrimStr,
                              & sTrimByte );
        
        if( sTrimByte == sValue2->mLength )
        {
            sTrimCharLen = 1;
        }
        else
        {
            /* Do Nothing */
        }
    }
    
    if( sTrimCharLen == 1 )
    {
        /*
         * trim_character가 1인 경우
         */
        
        while( sStringByteLen > 0  )
        {
            /**
             * 체크할 sString의 마지막 하나의 char의 length를 구한다.
             */
            
            if( sStringByteLen < sMaxCharLen )
            {
                sMaxCharLen = sStringByteLen;
            }
            else
            {
                /* Do Noghint */
            }
            
            for( i = sMaxCharLen; i > 0; i-- )
            {
                DTS_IS_VERIFY_UHC( sString - i,
                                   i,
                                   &sTempLen2,
                                   &sVerify );
                
                if ( sVerify == STL_TRUE )
                {
                    sStrByte = sTempLen2;
                    /*
                     * char를 찾았음으로 1Char만큼 건너뛰도록.
                     */
                    i = i - sStrByte + 1;
                }
                else
                {
                    /* Do Nothing */
                }
                /*
                 * 마지막에 찾은 char의 length의 길이가 sTempLen에 저장된다.
                 */
            }
        
            if( sStrByte == sTrimByte )
            {
                DTS_IS_EQUAL_SAME_BYTELEN_ONE_CHAR( ( sString - sStrByte ),
                                                    sTrimStr,
                                                    sTrimByte,
                                                    & sIsEqual );
                if( sIsEqual == STL_TRUE )
                {
                    /* Match */
                    sString -= sTrimByte;
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
        /* Do Nothing */
    }
    
    i = 0;
    sTrimInfo[i].mTrimByte = sTrimByte;
    sTrimInfo[i].mTrimStr = sTrimStr;
    
    sTrimStr += sTrimInfo[i].mTrimByte;
    i++;

    while( ( sTrimStr < sTrimStrEnd ) && ( i < DTF_RTRIM_INFO_ARR_MAX )  )
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
     * rtrim
     */
    
    sTrimCharLen = ( sTrimCharLenOrg <= DTF_RTRIM_INFO_ARR_MAX ) ? sTrimCharLenOrg : DTF_RTRIM_INFO_ARR_MAX;
    sTrimStr = DTF_VARCHAR( sValue2 );
    
    while( sStringByteLen > 0 )
    {
        /**
         * 체크할 sString의 마지막 하나의 char의 length를 구한다.
         */
        
        if( sStringByteLen < sMaxCharLen )
        {
            sMaxCharLen = sStringByteLen;
        }
        else
        {
            /* Do Noghint */
        }
        
        for( i = sMaxCharLen; i > 0; i-- )
        {
            DTS_IS_VERIFY_UHC( sString - i,
                               i,
                               &sTempLen2,
                               &sVerify );
            
            if ( sVerify == STL_TRUE )
            {
                sTempLen = sTempLen2;
                /*
                 * char를 찾았음으로 1Char만큼 건너뛰도록.
                 */
                i = i - sTempLen + 1;
            }
            else
            {
                /* Do Nothing */
            }
            /*
             * 마지막에 찾은 char의 length의 길이가 sTempLen에 저장된다.
             */
        }
        
        sStrByte = sTempLen;
        
        for( i = 0; i < sTrimCharLen; i++ )
        {
            if( sStrByte == sTrimInfo[i].mTrimByte )
            {
                DTS_IS_EQUAL_SAME_BYTELEN_ONE_CHAR( (sString - sStrByte),
                                                    sTrimInfo[i].mTrimStr,
                                                    sTrimInfo[i].mTrimByte,
                                                    & sIsEqual );
                if( sIsEqual == STL_TRUE )                    
                {
                    /* Match */
                    sString -= sTrimInfo[i].mTrimByte;
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

            STL_DASSERT( sTrimCharLen == DTF_RTRIM_INFO_ARR_MAX );
            
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
                    DTS_IS_EQUAL_SAME_BYTELEN_ONE_CHAR( (sString - sStrByte),
                                                        sTempTrimStr,
                                                        sTrimByte,
                                                        & sIsEqual );
                    if( sIsEqual == STL_TRUE )
                    {
                        /* Match */
                        sString -= sTrimByte;
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
    
    stlMemcpy( DTF_VARCHAR( sResultValue ), DTF_VARCHAR( sValue1 ), sStringByteLen );
    sResultValue->mLength = sStringByteLen;
    
    STL_DASSERT( sResultValue->mLength <= sResultValue->mBufferSize );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief gb18030에 대한 rtrim 처리. RTRIM( trim_source, trim_character )
 *        <BR> RTRIM( trim_source, trim_character )
 *        <BR>   RTRIM( trim_source, trim_character ) => [ P, O ]
 *        <BR> ex) RTRIM( '___TRIM FUNCTION___', '_' ) => ___TRIM FUNCTION
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> rtrim(VARCHAR, VARCHAR)
 *                               <BR> rtrim(LONGVARCHAR, LONGVARCHAR)
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR || LONGVARCHAR)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfStringRtrimForGb18030( stlUInt16        aInputArgumentCnt,
                                    dtlValueEntry  * aInputArgument,
                                    void           * aResultValue,
                                    dtlFuncVector  * aVectorFunc,
                                    void           * aVectorArg,
                                    void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sValue2 = NULL;
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;
    
    stlChar         * sString;
    stlChar         * sTrimStr;
    stlChar         * sTrimStrEnd = NULL;
    stlInt64          sStringByteLen = 0;
    stlInt64          sStrByte = 0;
    stlInt8           sTrimByte = 0;
    stlInt64          sTrimCharLen = 0;
    stlInt64          sTrimCharLenOrg = 0;    
    stlChar           sDefaultTrimChar[1];
    
    dtlCharacterSet   sCharSetID = DTL_GB18030;
    stlInt64          i = 0;
    stlBool           sIsRemainTrimStr = STL_FALSE;
    stlBool           sIsEqual = STL_FALSE;

    stlInt8           sTempLen  = 0;
    stlInt8           sTempLen2 = 0;
    stlInt8           sMaxCharLen = DTS_GET_GB18030_MB_MAX_LENGTH;
    stlBool           sVerify;
    
    stlChar         * sTempTrimStr;
    dtfRtrim          sTrimInfo[ DTF_RTRIM_INFO_ARR_MAX ];

    
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
    sString = DTF_VARCHAR( sValue1 ) + sValue1->mLength;
    sStringByteLen = sValue1->mLength;
    
    if( aInputArgumentCnt == 1 )
    {
        /*
         * trim_character가 생략되었을 경우, ' '으로 처리됨.
         * 예) RTRIM( 'abcde' ) => RTRIM( 'abcde', ' ' )
         */
        
        /*
         * @todo
         * ' ' 는 character set에 맞는 code로 가져와야 한다.
         * character set에 맞는 CharLen, ByteLen을 구해야 함.
         */
        sDefaultTrimChar[0] = ' ';
        sTrimByte = 1;
        sTrimCharLen = 1;
        
        sTrimStr = sDefaultTrimChar;        
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
        
        /*
         * trim_character 에 대한 정보구축
         * 우선, trim_character가 one character인지를 알아보기 위해 여기서 한자만 체크한다.
         */
        
        DTS_GET_GB18030_MBLENGTH( sTrimStr,
                                  & sTrimByte );
        
        if( sTrimByte == sValue2->mLength )
        {
            sTrimCharLen = 1;
        }
        else
        {
            /* Do Nothing */
        }
    }
    
    if( sTrimCharLen == 1 )
    {
        /*
         * trim_character가 1인 경우
         */
        
        while( sStringByteLen > 0  )
        {
            /**
             * 체크할 sString의 마지막 하나의 char의 length를 구한다.
             */
            
            if( sStringByteLen < sMaxCharLen )
            {
                sMaxCharLen = sStringByteLen;
            }
            else
            {
                /* Do Noghint */
            }
            
            for( i = sMaxCharLen; i > 0; i-- )
            {
                DTS_IS_VERIFY_GB18030( sString - i,
                                       i,
                                       &sTempLen2,
                                       &sVerify );
                
                if ( sVerify == STL_TRUE )
                {
                    sStrByte = sTempLen2;
                    /*
                     * char를 찾았음으로 1Char만큼 건너뛰도록.
                     */
                    i = i - sStrByte + 1;
                }
                else
                {
                    /* Do Nothing */
                }
                /*
                 * 마지막에 찾은 char의 length의 길이가 sTempLen에 저장된다.
                 */
            }
        
            if( sStrByte == sTrimByte )
            {
                DTS_IS_EQUAL_SAME_BYTELEN_ONE_CHAR( ( sString - sStrByte ),
                                                    sTrimStr,
                                                    sTrimByte,
                                                    & sIsEqual );
                if( sIsEqual == STL_TRUE )
                {
                    /* Match */
                    sString -= sTrimByte;
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
        /* Do Nothing */
    }
    
    i = 0;
    sTrimInfo[i].mTrimByte = sTrimByte;
    sTrimInfo[i].mTrimStr = sTrimStr;
    
    sTrimStr += sTrimInfo[i].mTrimByte;
    i++;

    while( ( sTrimStr < sTrimStrEnd ) && ( i < DTF_RTRIM_INFO_ARR_MAX )  )
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
     * rtrim
     */
    
    sTrimCharLen = ( sTrimCharLenOrg <= DTF_RTRIM_INFO_ARR_MAX ) ? sTrimCharLenOrg : DTF_RTRIM_INFO_ARR_MAX;
    sTrimStr = DTF_VARCHAR( sValue2 );
    
    while( sStringByteLen > 0 )
    {
        /**
         * 체크할 sString의 마지막 하나의 char의 length를 구한다.
         */
        
        if( sStringByteLen < sMaxCharLen )
        {
            sMaxCharLen = sStringByteLen;
        }
        else
        {
            /* Do Noghint */
        }
        
        for( i = sMaxCharLen; i > 0; i-- )
        {
            DTS_IS_VERIFY_GB18030( sString - i,
                                   i,
                                   &sTempLen2,
                                   &sVerify );
            
            if ( sVerify == STL_TRUE )
            {
                sTempLen = sTempLen2;
                /*
                 * char를 찾았음으로 1Char만큼 건너뛰도록.
                 */
                i = i - sTempLen + 1;
            }
            else
            {
                /* Do Nothing */
            }
            /*
             * 마지막에 찾은 char의 length의 길이가 sTempLen에 저장된다.
             */
        }
        
        sStrByte = sTempLen;
        
        for( i = 0; i < sTrimCharLen; i++ )
        {
            if( sStrByte == sTrimInfo[i].mTrimByte )
            {
                DTS_IS_EQUAL_SAME_BYTELEN_ONE_CHAR( (sString - sStrByte),
                                                    sTrimInfo[i].mTrimStr,
                                                    sTrimInfo[i].mTrimByte,
                                                    & sIsEqual );
                if( sIsEqual == STL_TRUE )                    
                {
                    /* Match */
                    sString -= sTrimInfo[i].mTrimByte;
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

            STL_DASSERT( sTrimCharLen == DTF_RTRIM_INFO_ARR_MAX );
            
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
                    DTS_IS_EQUAL_SAME_BYTELEN_ONE_CHAR( (sString - sStrByte),
                                                        sTempTrimStr,
                                                        sTrimByte,
                                                        & sIsEqual );
                    if( sIsEqual == STL_TRUE )
                    {
                        /* Match */
                        sString -= sTrimByte;
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
    
    stlMemcpy( DTF_VARCHAR( sResultValue ), DTF_VARCHAR( sValue1 ), sStringByteLen );
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
 * @brief RTRIM( trim_source, trim_character )
 *        <BR> RTRIM( trim_source, trim_character )
 *        <BR>   RTRIM( trim_source, trim_character ) => [ P, O ]
 *        <BR> ex) RTRIM( X'AABBCC', 'CC' ) => AABB
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> rtrim(VARBINARY, VARBINARY)
 *                               <BR> rtrim(LONGVARBINARY, LONGVARBINARY)
 * @param[out] aResultValue      연산 결과 (결과타입 VARBINARY || LONGVARBINARY)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfBinaryStringRtrim( stlUInt16        aInputArgumentCnt,
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
         * 예) RTRIM( X'AABBCC' ) => LTRIM( X'AABBCC', X'00' )
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
    sString = DTF_VARBINARY( sValue1 ) + sValue1->mLength;
    sStringLen = sValue1->mLength;
    
    /*
     * rtrim
     */
    while( sStringLen > 0 )
    {
        for( i = 0; i < sTrimLen; i++ )
        {
            if( *(sString - 1) == sTrimStr[i] )
            {
                /* Match*/
                sString--;
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
    
    stlMemcpy( DTF_VARBINARY( sResultValue ), DTF_VARBINARY( sValue1 ), sStringLen );
    sResultValue->mLength = sStringLen;
    
    STL_DASSERT( sResultValue->mLength <= sResultValue->mBufferSize );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}



/** @} */
