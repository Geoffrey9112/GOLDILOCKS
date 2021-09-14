/*******************************************************************************
 * dtfShiftRight.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtfShiftRight.c 1389 2011-07-12 02:23:18Z ehpark $
 *
 * NOTES
 *   1. BigInt, Double, Numeric공통.
 *      함수의 피연산자에 NULL값이 오지 않는다고 가정하고 구현되어 있다.
 *   2. Double형 연산 관련.
 *      현재 NaN은 고려되어 있지 않다.
 *      infinite 값이 들어오면 argument out of range 로 처리한다.
 *   3. Bit 연산은 overflow 체크 안함.
 *      <<, >>은 overflow, underflow가 날수 있지만 별도 처리 없음.
 *      postgres: overflow 처리 없음. ( 9223372036854775807 << 4 => -16 )
 *      mysql: 9223372036854775807 << 4 => 18446744073709551600
 *             9223372036854775807 << 300 => 0
 *      <<, >> 연산에서 두번째 인자에 음수가 올경우
 *             mysql, postgres 각각 값이 다름.
 *             select 4 << -3 => -9223372036854775808   (gliese) c결과 임.
 *             select 4 << -3 => -2147483648            (postgres)
 *             select 4 << -3 => 0                      (mysql)
 *
 ******************************************************************************/

/**
 * @file dtfShiftRight.c
 * @brief dtfShiftRight Function DataType Library Layer
 */

#include <dtl.h>
#include <dtlDef.h>
#include <dtDef.h>
#include <dtfBitwise.h>
#include <dtfNumeric.h>


/**
 * @addtogroup dtfShiftRight ShiftRight
 * @ingroup dtf
 * @internal
 * @{
 */

dtlBuiltInFuncPtr  gDtfBitwiseShiftRightFunc[ DTL_TYPE_MAX ] = 
{
    NULL,                         /**< BOOLEAN */
    dtfSmallIntBitwiseShiftRight, /**< NATIVE_SMALLINT */
    dtfIntegerBitwiseShiftRight,  /**< NATIVE_INTEGER */
    dtfBigIntBitwiseShiftRight,   /**< NATIVE_BIGINT */

    NULL,                         /**< NATIVE_REAL */
    NULL,                         /**< NATIVE_DOUBLE */

    dtfNumberBitwiseShiftRight,   /**< FLOAT */
    dtfNumberBitwiseShiftRight,   /**< NUMBER */
    NULL,                  /**< unsupported feature, DECIMAL */
    
    NULL,                /**< CHARACTER */
    NULL,                /**< CHARACTER VARYING */
    NULL,                /**< CHARACTER LONG VARYING */
    NULL,                /**< unsupported feature, CHARACTER LARGE OBJECT */
    
    NULL,                /**< BINARY */
    NULL,                /**< BINARY VARYING */
    NULL,                /**< BINARY LONG VARYING */
    NULL,                /**< unsupported feature, BINARY LARGE OBJECT */

    NULL,                /**< DATE */
    NULL,                /**< TIME WITHOUT TIME ZONE */
    NULL,                /**< TIMESTAMP WITHOUT TIME ZONE */
    NULL,                /**< TIME WITH TIME ZONE */
    NULL,                /**< TIMESTAMP WITH TIME ZONE */

    NULL,                /**< DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
    NULL,                /**< DTL_TYPE_INTERVAL_DAY_TO_SECOND */

    NULL                 /**< DTL_TYPE_ROWID */
};


/*******************************************************************************
 * bitwise shift right ( >> )
 *      >> => [ P, M ]
 * ex) 8 >> 2  =>  2
 ******************************************************************************/

/**
 * @brief NATIVE_SMALLINT value의 shift right(>>) 연산
 *        <BR> bitwise shift right ( >> )
 *        <BR>   >> => [ P, M ]
 *        <BR> ex) 8 >> 2  =>  2
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> (NATIVE_SMALLINT) >> (NUMBER)
 * @param[out] aResultValue      연산 결과 (결과타입 BIGINT)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfSmallIntBitwiseShiftRight( stlUInt16        aInputArgumentCnt,
                                        dtlValueEntry  * aInputArgument,
                                        void           * aResultValue,
                                        void           * aInfo,
                                        dtlFuncVector  * aVectorFunc,
                                        void           * aVectorArg,
                                        void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sValue2;
    dtlDataValue  * sResultValue;

    stlInt64        sShiftValue;
    stlBool         sIsTruncated = STL_FALSE;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_SMALLINT, (stlErrorStack *)aEnv );
    DTF_CHECK_DTL_VALUE2( sValue2, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, (stlErrorStack *)aEnv );        

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_BIGINT, (stlErrorStack *)aEnv );

    /*
     * sValu2를 bigint 값으로 변환한다. : 반올림하지 않는다.
     */
    
    STL_TRY( dtlNumericToInt64( sValue2,
                                & sShiftValue,
                                & sIsTruncated,
                                (stlErrorStack *)aEnv )
             == STL_SUCCESS );
    
    /* 
     * platform마다 shift value로 처리하는 bit의 범위가 달라서
     * shift value로 올 수 있는 값을 6bit범위내의 값으로 한다.
     * 예) linux : 6bit , aix : 7bit
     */
    
    sShiftValue &= 0x0000003F;

    /* shift right 연산을 한다. */
    DTF_BIGINT( sResultValue ) = (dtlBigIntType)(DTF_SMALLINT(sValue1)) >> sShiftValue;
    sResultValue->mLength = DTL_NATIVE_BIGINT_SIZE;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief NATIVE_INTEGER value의 shift right(>>) 연산
 *        <BR> bitwise shift right ( >> )
 *        <BR>   >> => [ P, M ]
 *        <BR> ex) 8 >> 2  =>  2
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> (NATIVE_INTEGER) >> (NUMBER)
 * @param[out] aResultValue      연산 결과 (결과타입 BIGINT)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfIntegerBitwiseShiftRight( stlUInt16        aInputArgumentCnt,
                                       dtlValueEntry  * aInputArgument,
                                       void           * aResultValue,
                                       void           * aInfo,
                                       dtlFuncVector  * aVectorFunc,
                                       void           * aVectorArg,
                                       void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sValue2;
    dtlDataValue  * sResultValue;

    stlInt64        sShiftValue;
    stlBool         sIsTruncated = STL_FALSE;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_INTEGER, (stlErrorStack *)aEnv );
    DTF_CHECK_DTL_VALUE2( sValue2, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, (stlErrorStack *)aEnv );        
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_BIGINT, (stlErrorStack *)aEnv );
    
    /*
     * sValu2를 bigint 값으로 변환한다. : 반올림하지 않는다.
     */
    
    STL_TRY( dtlNumericToInt64( sValue2,
                                & sShiftValue,
                                & sIsTruncated,
                                (stlErrorStack *)aEnv )
             == STL_SUCCESS );
    
    /* 
     * platform마다 shift value로 처리하는 bit의 범위가 달라서
     * shift value로 올 수 있는 값을 6bit범위내의 값으로 한다.
     * 예) linux : 6bit , aix : 7bit
     */
    
    sShiftValue &= 0x0000003F;
    
    /* shift right 연산을 한다. */
    DTF_BIGINT( sResultValue ) = (dtlBigIntType)(DTF_INTEGER(sValue1)) >> sShiftValue;
    sResultValue->mLength = DTL_NATIVE_BIGINT_SIZE;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief NATIVE_BIGINT value의 shift right(>>) 연산
 *        <BR> bitwise shift right ( >> )
 *        <BR>   >> => [ P, M ]
 *        <BR> ex) 8 >> 2  =>  2
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> (NATIVE_BIGINT) >> (NUMBER)
 * @param[out] aResultValue      연산 결과 (결과타입 BIGINT)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfBigIntBitwiseShiftRight( stlUInt16        aInputArgumentCnt,
                                      dtlValueEntry  * aInputArgument,
                                      void           * aResultValue,
                                      void           * aInfo,
                                      dtlFuncVector  * aVectorFunc,
                                      void           * aVectorArg,
                                      void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sValue2;
    dtlDataValue  * sResultValue;

    stlInt64        sShiftValue;
    stlBool         sIsTruncated = STL_FALSE;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_BIGINT, (stlErrorStack *)aEnv );
    DTF_CHECK_DTL_VALUE2( sValue2, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, (stlErrorStack *)aEnv );        
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_BIGINT, (stlErrorStack *)aEnv );
    
    /*
     * sValu2를 bigint 값으로 변환한다. : 반올림하지 않는다.
     */
    
    STL_TRY( dtlNumericToInt64( sValue2,
                                & sShiftValue,
                                & sIsTruncated,
                                (stlErrorStack *)aEnv )
             == STL_SUCCESS );
    
    /* 
     * platform마다 shift value로 처리하는 bit의 범위가 달라서
     * shift value로 올 수 있는 값을 6bit범위내의 값으로 한다.
     * 예) linux : 6bit , aix : 7bit
     */
    
    sShiftValue &= 0x0000003F;
    
    /* shift right 연산을 한다. */
    DTF_BIGINT( sResultValue ) = DTF_BIGINT(sValue1) >> sShiftValue;
    sResultValue->mLength = DTL_NATIVE_BIGINT_SIZE;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief NUMBER value의 shift right(>>) 연산
 *        <BR> bitwise shift right ( >> )
 *        <BR>   >> => [ P, M ]
 *        <BR> ex) 8 >> 2  =>  2
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> (NUMBER) >> (NUMBER)
 * @param[out] aResultValue      연산 결과 (결과타입 BIGINT)
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorArg
 * @param[in]  aVectorArg        aVectorArg
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfNumberBitwiseShiftRight( stlUInt16        aInputArgumentCnt,
                                      dtlValueEntry  * aInputArgument,
                                      void           * aResultValue,
                                      void           * aInfo,
                                      dtlFuncVector  * aVectorFunc,
                                      void           * aVectorArg,
                                      void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sValue2;
    dtlDataValue  * sResultValue;

    stlInt64        sIntNum;
    stlInt64        sShiftValue;
    stlBool         sIsTruncated = STL_FALSE;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE2( sValue1, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, (stlErrorStack *)aEnv );
    DTF_CHECK_DTL_VALUE2( sValue2, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, (stlErrorStack *)aEnv );        
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_BIGINT, (stlErrorStack *)aEnv );
    
    /*
     * sValue1, sValu2를 bigint 값으로 변환한다. : 반올림하지 않는다.
     */
    
    STL_TRY( dtlNumericToInt64( sValue1,
                                & sIntNum,
                                & sIsTruncated,
                                (stlErrorStack *)aEnv )
             == STL_SUCCESS );
    
    STL_TRY( dtlNumericToInt64( sValue2,
                                & sShiftValue,
                                & sIsTruncated,
                                (stlErrorStack *)aEnv )
             == STL_SUCCESS );
    
    /* 
     * platform마다 shift value로 처리하는 bit의 범위가 달라서
     * shift value로 올 수 있는 값을 6bit범위내의 값으로 한다.
     * 예) linux : 6bit , aix : 7bit
     */
    
    sShiftValue &= 0x0000003F;
    
    /* shift right 연산을 한다. */
    DTF_BIGINT( sResultValue ) = sIntNum >> sShiftValue;
    sResultValue->mLength = DTL_NATIVE_BIGINT_SIZE;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
