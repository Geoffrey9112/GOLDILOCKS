/*******************************************************************************
 * dtfPi.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtfPi.c 1296 2011-06-30 07:24:30Z ehpark $
 *
 * NOTES
 *   1. BigInt, Double, Numeric공통.
 *      함수의 피연산자에 NULL값이 오지 않는다고 가정하고 구현되어 있다.
 *   2. Double형 연산 관련.
 *      현재 NaN은 고려되어 있지 않다.
 *      infinite 값이 들어오면 argument out of range 로 처리한다.
 *   3. Numeric형 연산 관련.
 *      numeric은 precision, scale과 연관되어 검증되어야 함으로 상위단에서
 *      세밀하게 검증이 필요함.
 *
 ******************************************************************************/

/**
 * @file dtfPi.c
 * @brief dtfPi Function DataType Library Layer
 */

#include <dtl.h>
#include <dtlDef.h>
#include <dtDef.h>
#include <dtfArithmetic.h>
#include <dtfMathematical.h>



/**
 * @addtogroup dtfPi Pi
 * @ingroup dtf
 * @internal
 * @{
 */


const dtfPredefinedNumeric dtfPINumeric[ 1 ] =
{
    { 21, (dtlNumericType*) "\xc1\x4\xf\x10\x5d\x42\x24\x5a\x50\x21\x27\x2f\x1b\x2c\x27\x21\x50\x33\x1d\x55\x15" }
};

/*******************************************************************************
 * pi()  "π" constant
 *   pi() => [ P, M ]
 * ex) pi()  =>  3.14159265358979
 ******************************************************************************/

/**
 * @brief "π" constant 를 반환하는 함수
 *        <BR> pi()  "π" constant
 *        <BR>   pi() => [ P, M ]
 *        <BR> ex) pi()  =>  3.14159265358979
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> input argument가 없음.
 * @param[out] aResultValue      연산 결과 (결과타입 NUMBER)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfPi( stlUInt16        aInputArgumentCnt,
                 dtlValueEntry  * aInputArgument,
                 void           * aResultValue,
                 void           * aEnv )
{
    dtlDataValue  * sResultValue;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 0, (stlErrorStack *)aEnv );

    sResultValue = (dtlDataValue *)aResultValue;

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, (stlErrorStack *)aEnv );

    /* pi값을 설정 한다. */
    stlMemcpy( DTF_NUMERIC( sResultValue ),
               dtfPINumeric->mNumeric,
               dtfPINumeric->mLength );

    sResultValue->mLength = dtfPINumeric->mLength;

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
