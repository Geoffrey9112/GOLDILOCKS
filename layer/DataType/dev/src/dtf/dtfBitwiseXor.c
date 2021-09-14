/*******************************************************************************
 * dtfBitwiseXor.c
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
 * @file dtfBitwiseXor.c
 * @brief dtfBitwiseXor Function DataType Library Layer
 */

#include <dtl.h>
#include <dtlDef.h>
#include <dtDef.h>
#include <dtfBitwise.h>
#include <dtfNumeric.h>


/**
 * @addtogroup dtfBitwiseXor bitwiseXor
 * @ingroup dtf
 * @internal
 * @{
 */


/*******************************************************************************
 * bitwise XOR ( # )
 *      #  => [ P ]
 *      ^  -> [ M ]
 *        Mathematical에서 ^를  2.0 ^ 3.0 => 8 용도로 사용하므로 #를 XOR연산자로 써야할지
 *        Math에서 ^를 제거하고, ^를 XOR연산자로 써야할지 결정이 필요함.(C에서는 ^임)
 * ex) 17 # 5  =>  20
 ******************************************************************************/

/**
 * @brief integer(smallint/integer/bigint) value의 bitwise XOR(|) 연산
 *        <BR> bitwise XOR ( # )
 *        <BR>   #  => [ P ]
 *        <BR>   ^  -> [ M ]
 *        <BR> @todo
 *        <BR> Mathematical에서 ^ 를  2.0 ^ 3.0 => 8 용도로 사용하므로 # 를 XOR연산자로 써야할지
 *        <BR> Math에서 ^ 를 제거하고, ^ 를 XOR연산자로 써야할지 결정이 필요함.(C에서는 ^임)
 *        <BR> ex) 17 # 5  =>  20
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                              <BR> (BIGINT) # 또는 ^ (BIGINT)
 * @param[out] aResultValue     연산 결과 (결과타입 BIGINT)
 * @param[out] aEnv             environment (stlErrorStack)
 */
stlStatus dtfBigIntBitwiseXor( stlUInt16        aInputArgumentCnt,
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

    /* xor 연산을 한다. */
    DTF_BIGINT( sResultValue ) = DTF_BIGINT( sValue1 ) ^ DTF_BIGINT( sValue2 );
    sResultValue->mLength = DTL_NATIVE_BIGINT_SIZE;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief NUMBER value의 bitwise XOR(|) 연산
 *        <BR> bitwise XOR ( # )
 *        <BR>   #  => [ P ]
 *        <BR>   ^  -> [ M ]
 *        <BR> @todo
 *        <BR> Mathematical에서 ^ 를  2.0 ^ 3.0 => 8 용도로 사용하므로 # 를 XOR연산자로 써야할지
 *        <BR> Math에서 ^ 를 제거하고, ^ 를 XOR연산자로 써야할지 결정이 필요함.(C에서는 ^임)
 *        <BR> ex) 17 # 5  =>  20
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                              <BR> (NUMBER) # 또는 ^ (NUMBER)
 * @param[out] aResultValue     연산 결과 (결과타입 NUMBER)
 * @param[out] aEnv             environment (stlErrorStack)
 */
stlStatus dtfNumberBitwiseXor( stlUInt16        aInputArgumentCnt,
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
     * 64bit까지 BITXOR 연산지원. 
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
    
    
    /* Xor 연산을 한다. */
    DTF_BIGINT( sResultValue ) = sIntNum1 ^ sIntNum2;
    sResultValue->mLength = DTL_NATIVE_BIGINT_SIZE;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}



/** @} */
