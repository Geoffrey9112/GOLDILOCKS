/*******************************************************************************
 * dtfBitwiseAnd.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
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
 * @file dtfBitwiseAnd.c
 * @brief dtfBitwiseAnd Function DataType Library Layer
 */

#include <dtl.h>
#include <dtlDef.h>
#include <dtDef.h>
#include <dtfBitwise.h>
#include <dtfNumeric.h>


/**
 * @addtogroup dtfBitwiseAnd bitwiseAnd
 * @ingroup dtf
 * @internal
 * @{
 */


/*******************************************************************************
 * bitwise AND ( & )
 *               &  => [ P, M ]
 *   BITAND( x, y ) => [ O ]
 * ex) 91 & 15  =>  11
 ******************************************************************************/

/**
 * @brief integer(smallint/integer/bigint) value의 bitwise AND(&) 연산
 *        <BR> bitwise AND ( & )
 *        <BR>               &  => [ P, M ]
 *        <BR>   BITAND( x, y ) => [ O ]
 *        <BR> ex) 91 & 15  =>  11
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> (BIGINT) & (BIGINT)
 * @param[out] aResultValue      연산 결과 (결과타입 BIGINT)
 * @param[out] aEnv              environment (stlErrorStack)
 */

stlStatus dtfBigIntBitwiseAnd( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sValue2;
    dtlDataValue  * sResultValue;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_BIGINT, (stlErrorStack *)aEnv );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_NATIVE_BIGINT, (stlErrorStack *)aEnv );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_BIGINT, (stlErrorStack *)aEnv );

    /* and 연산을 한다. */
    DTF_BIGINT( sResultValue ) = DTF_BIGINT( sValue1 ) & DTF_BIGINT( sValue2 );
    sResultValue->mLength = DTL_NATIVE_BIGINT_SIZE;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief NUMBER value의 bitwise AND(&) 연산
 *        <BR> bitwise AND ( & )
 *        <BR>               &  => [ P, M ]
 *        <BR>   BITAND( x, y ) => [ O ]
 *        <BR> ex) 91 & 15  =>  11
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> (NUMBER) & (NUMBER)
 * @param[out] aResultValue      연산 결과 (결과타입 NUMBER)
 * @param[out] aEnv              environment (stlErrorStack)
 */

stlStatus dtfNumberBitwiseAnd( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sValue2;
    dtlDataValue  * sResultValue;

    stlInt64        sIntNum1 = 0;
    stlInt64        sIntNum2 = 0;
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
     * sValue1, sValue2를 stlInt64로 변환한다. : 반올림하지 않는다.
     * 64bit까지 AND 연산지원. 
     */
    
    STL_TRY( dtlNumericToInt64( sValue1,
                                & sIntNum1,
                                & sIsTruncated,
                                (stlErrorStack *)aEnv )
             == STL_SUCCESS );
    
    STL_TRY( dtlNumericToInt64( sValue2,
                                & sIntNum2,
                                & sIsTruncated,
                                (stlErrorStack *)aEnv )
             == STL_SUCCESS );
    
    /* and 연산을 한다. */
    DTF_BIGINT( sResultValue ) = sIntNum1 & sIntNum2;
    sResultValue->mLength = DTL_NATIVE_BIGINT_SIZE;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}



/** @} */
