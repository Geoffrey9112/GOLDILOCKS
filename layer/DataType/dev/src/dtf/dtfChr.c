/*******************************************************************************
 * dtfChr.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file dtfChr.c
 * @brief String Function DataType Library Layer
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtsCharacterSet.h>


/**
 * @ingroup dtf
 * @internal
 * @{
 */


/**
 * @brief Chr 문자열내의 Chr Code value 반환
 * 
 * @param[in]  aChrCode          chr code 값
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aErrorStack       stlErrorStack
 */

stlStatus dtfChr( stlUInt64        aChrCode,
                  dtlDataValue   * aResultValue,
                  dtlFuncVector  * aVectorFunc,
                  void           * aVectorArg,
                  stlErrorStack  * aErrorStack )
{
    stlChar         * sResultValuePtr = NULL;
    dtlCharacterSet   sCharSetID = DTL_CHARACTERSET_MAX;
    stlUInt32         sChrValue = 0;
    stlInt8           sChrLength = 0;
    stlInt64          sResultLength = 0;
    stlBool           sIsMbCharset = STL_FALSE;
    
    sCharSetID = aVectorFunc->mGetCharSetIDFunc(aVectorArg);
    
    /* Integer Length STL_UINT32_MAX 이상은 Overflow */    
    STL_TRY_THROW( aChrCode <= (stlUInt64)STL_UINT32_MAX, ERROR_INVALID_CHARACTER_OVERFLOW );

    /* Numeric To Integer 를 위해 UInt64 -> UInt32 */
    sChrValue = (stlUInt32)aChrCode;
    sResultValuePtr = DTF_VARCHAR( aResultValue );
    
    sIsMbCharset = dtsGetMbMaxLengthFuncList[ sCharSetID ]() > 1;

    if( sIsMbCharset == STL_FALSE )
    {
        STL_TRY_THROW( sChrValue <= 255,
                       ERROR_INVALID_CHARACTER_OVERFLOW );
        
        sResultValuePtr[0] = (stlUInt8)sChrValue;
        sResultLength = 1;
        
        STL_THROW( RAMP_FUNC_END );
    }
    else
    {
        /* Do Nothing */
    }
    
    
    /*
     * multi-byte character set
     */
    
    if( sChrValue < 0x100 )
    {
        /* 1byte Character 0 ~ 255 */
        sResultValuePtr[0] = (stlUInt8)sChrValue;
        sResultLength = 1;
    }
    else if( ( 0x100 <= sChrValue ) && ( sChrValue < 0x10000 ) )
    {
        /* 2Byte의 Character 256 ~ 65535 */
        /* 0000 0000 [1111 1111] -> 255 */
        sResultValuePtr[1] = sChrValue & 0xFF ;
        /* [1111 1111] 0000 0000 -> 65280 = 255 */
        sResultValuePtr[0] = ( sChrValue >> 8 ) & 0xFF ;
        sResultLength = 2;
    }
    else if( ( 0x10000 <= sChrValue ) && ( sChrValue < 0x1000000 ) )
    {
        /* 3byte Character 65536 ~ 16777215 */
        sResultValuePtr[2] = sChrValue & 0xFF ;
        sResultValuePtr[1] = ( sChrValue >> 8 ) & 0xFF ;
        sResultValuePtr[0] = ( sChrValue >> 16 ) & 0xFF ;
        sResultLength = 3;
    }
    else
    {
        /* 4byte utf-8 문자 */
        sResultValuePtr[3] = sChrValue & 0xFF ;
        sResultValuePtr[2] = ( sChrValue >> 8 ) & 0xFF ;
        sResultValuePtr[1] = ( sChrValue >> 16 ) & 0xFF ;
        sResultValuePtr[0] = ( sChrValue >> 24 ) & 0xFF ;
        sResultLength = 4;
    }
    
    /**
     * utf-8 , uhc 로 표현이 가능한 문자인지 검사함
     **/
    STL_TRY( dtsMbVerifyFuncList[sCharSetID]( sResultValuePtr,
                                              sResultLength,
                                              &sChrLength,
                                              aErrorStack ) == STL_SUCCESS );

    /**
     * @todo 
     * 예)  select chr( cast( 27279 as native_bigint ) ) from dual; 의 처리
     * 계산상으로 2 byte이지만, utf8 (1byte)로 인식됨.
     * 이에 대한 처리를 어떻게 할지 결정이 필요함.
     */
    STL_TRY_THROW( sResultLength == sChrLength, ERROR_INVALID_CHARACTER_LENGTH );
    
    STL_RAMP( RAMP_FUNC_END );
    
    aResultValue->mLength = sResultLength;
    
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_INVALID_CHARACTER_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_DATA_OUTSIDE_RANGE_NUMBER_CONVERTED,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( ERROR_INVALID_CHARACTER_LENGTH )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_CHARACTER_IN_CHARSET_REPERTOIRE,
                      NULL,
                      aErrorStack );  
    }    
    
    STL_FINISH;
    
    return STL_FAILURE;    
}


/*******************************************************************************
 *   Chr
 *   Chr  => [ P, M, O]
 * ex) Chr( 71 )  =>  G
 *******************************************************************************/

/**
 * @brief chr(int) 코드의 문자 반환
 *        <BR> Chr(int)
 *        <BR>   Chr => [ P, O ]
 *        <BR>   Chr( int, ... ) => [ M ] 구문이 조금 다름.
 *        <BR> ex) Chr( 71 )  =>  G
 *        <BR> ex) Chr( null )  =>  null
 *        <BR> ex) Chr( '' )  =>  null
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> Chr(BIGINT)
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */

stlStatus dtfBigIntChr( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        dtlFuncVector  * aVectorFunc,
                        void           * aVectorArg,
                        void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;
    
    stlUInt64         sUInt64Chr = 0;
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;
    
    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_BIGINT, sErrorStack );
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_VARCHAR, sErrorStack );
    
    /*
     * BigInt -> Integer
     * 음수인 경우 overflow
     */
    STL_TRY_THROW( DTF_BIGINT( sValue1 ) >= 0,
                   ERROR_INVALID_CHARACTER_OVERFLOW );
    
    sUInt64Chr = DTF_BIGINT( sValue1 );
    
    /*
     * chr(int) 코드의 문자 반환
     */ 
    STL_TRY( dtfChr( sUInt64Chr,
                     sResultValue,
                     aVectorFunc,
                     aVectorArg,
                     sErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_INVALID_CHARACTER_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_DATA_OUTSIDE_RANGE_NUMBER_CONVERTED,
                      NULL,
                      sErrorStack );
    }                    
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief chr(int) 코드의 문자 반환
 *        <BR> Chr(int)
 *        <BR>   Chr => [ P, O ]
 *        <BR>   Chr( int, ... ) => [ M ] 구문이 조금 다름.
 *        <BR> ex) Chr( 71 )  =>  G
 *        <BR> ex) Chr( null )  =>  null
 *        <BR> ex) Chr( '' )  =>  null
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> Chr(NUMERIC)
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */

stlStatus dtfNumericChr( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;
    
    stlUInt64         sUInt64Chr = 0;
    stlBool           sIsTruncated = STL_FALSE;
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;
    
    DTF_CHECK_DTL_VALUE2( sValue1, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_VARCHAR, sErrorStack );
    
    /*
     * numeric -> Integer
     * 음수인 경우 overflow 이고, round 되지 않음.
     */
    STL_TRY( dtlNumericToUInt64( sValue1,
                                 & sUInt64Chr,
                                 & sIsTruncated,
                                 sErrorStack )
             == STL_SUCCESS );
    
    /*
     * chr(int) 코드의 문자 반환
     */ 
    STL_TRY( dtfChr( sUInt64Chr,
                     sResultValue,
                     aVectorFunc,
                     aVectorArg,
                     sErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief chr(int) 코드의 문자 반환
 *        <BR> Chr(int)
 *        <BR>   Chr => [ P, O ]
 *        <BR>   Chr( int, ... ) => [ M ] 구문이 조금 다름.
 *        <BR> ex) Chr( 71 )  =>  G
 *        <BR> ex) Chr( null )  =>  null
 *        <BR> ex) Chr( '' )  =>  null
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> Chr(DOUBLE)
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */

stlStatus dtfDoubleChr( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        dtlFuncVector  * aVectorFunc,
                        void           * aVectorArg,
                        void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;
    
    stlUInt64         sUInt64Chr = 0;
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;
    
    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_DOUBLE, sErrorStack );
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_VARCHAR, sErrorStack );
    
    /*
     * double -> Integer
     * 음수인 경우 overflow 이고, round 되지 않음.
     */
    STL_TRY_THROW( DTF_DOUBLE( sValue1 ) >= 0,
                   ERROR_INVALID_CHARACTER_OVERFLOW );

    sUInt64Chr = (dtlBigIntType)(DTF_DOUBLE( sValue1 ));
    
    /*
     * chr(int) 코드의 문자 반환
     */ 
    STL_TRY( dtfChr( sUInt64Chr,
                     sResultValue,
                     aVectorFunc,
                     aVectorArg,
                     sErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_INVALID_CHARACTER_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_DATA_OUTSIDE_RANGE_NUMBER_CONVERTED,
                      NULL,
                      sErrorStack );
    }                    
    
    STL_FINISH;

    return STL_FAILURE;
}



/** @} */
