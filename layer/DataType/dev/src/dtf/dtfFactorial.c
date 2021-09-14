/*******************************************************************************
 * dtfFactorial.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtfFactorial.c 1296 2011-06-30 07:24:30Z ehpark $
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
 * @file dtfFactorial.c
 * @brief dtfFactorial Function DataType Library Layer
 */

#include <dtl.h>
#include <dtlDef.h>
#include <dtDef.h>
#include <dtfArithmetic.h>
#include <dtfNumeric.h>
#include <dtfMathematical.h>



/**
 * @addtogroup dtfFactorial Factorial
 * @ingroup dtf
 * @internal
 * @{
 */


const stlInt64 dtfFactorialBigInt[ DTF_MAX_FACTORIAL_BIGINT + 1 ] =
{
    STL_INT64_C(1), /* 0 */
    STL_INT64_C(1), STL_INT64_C(2), STL_INT64_C(6), STL_INT64_C(24), STL_INT64_C(120),
    STL_INT64_C(720), STL_INT64_C(5040), STL_INT64_C(40320), STL_INT64_C(362880), STL_INT64_C(3628800),
    STL_INT64_C(39916800), STL_INT64_C(479001600), STL_INT64_C(6227020800), STL_INT64_C(87178291200), STL_INT64_C(1307674368000),
    STL_INT64_C(20922789888000), STL_INT64_C(355687428096000), STL_INT64_C(6402373705728000), STL_INT64_C(121645100408832000), STL_INT64_C(2432902008176640000)  /* 20 */
};

const dtfPredefinedNumeric dtfFactorialNumeric[ DTF_MAX_FACTORIAL_NUMERIC + 1 ] =
{
    /* 0 */
    { 2, (dtlNumericType*)"\xC1\x2" },

    /* 1 ~ 10 */
    { 2, (dtlNumericType*)"\xC1\x2" },
    { 2, (dtlNumericType*)"\xC1\x3" },
    { 2, (dtlNumericType*)"\xC1\x7" },
    { 2, (dtlNumericType*)"\xC1\x19" },
    { 3, (dtlNumericType*)"\xC2\x2\x15" },
    { 3, (dtlNumericType*)"\xC2\x8\x15" },
    { 3, (dtlNumericType*)"\xC2\x33\x29" },
    { 4, (dtlNumericType*)"\xC3\x5\x4\x15" },
    { 4, (dtlNumericType*)"\xC3\x25\x1D\x51" },
    { 4, (dtlNumericType*)"\xC4\x4\x3F\x59" },
    
    /* 11 ~ 20 */
    { 4, (dtlNumericType*)"\xC4\x28\x5C\x45" },
    { 5, (dtlNumericType*)"\xC5\x5\x50\x1\x11" },
    { 5, (dtlNumericType*)"\xC5\x3F\x1C\x3\x9" },
    { 6, (dtlNumericType*)"\xC6\x9\x48\x4F\x1E\xD" },
    { 7, (dtlNumericType*)"\xC7\x2\x1F\x4D\x4B\x25\x51" },
    { 7, (dtlNumericType*)"\xC7\x15\x5D\x1C\x5A\x59\x51" },
    { 8, (dtlNumericType*)"\xC8\x4\x38\x45\x4B\x1D\xA\x3D" },
    { 8, (dtlNumericType*)"\xC8\x41\x3\x26\x26\x6\x49\x51" },
    { 9, (dtlNumericType*)"\xC9\xD\x11\x2E\xB\x5\x9\x54\x15" },
    { 9, (dtlNumericType*)"\xCA\x3\x2C\x1E\x3\x1\x52\x4D\x41" },
    
    /* 21 ~ 30 */
    { 9, (dtlNumericType*)"\xCA\x34\xA\xA\x2B\x12\x12\xA\x2D" },
    { 10, (dtlNumericType*)"\xCB\xC\x19\x1\x8\x1C\x4E\x4D\x8\x45" },
    { 11, (dtlNumericType*)"\xCC\x3\x3B\x35\x2\x44\x27\x59\x32\x4D\x41" },
    { 11, (dtlNumericType*)"\xCC\x3F\x5\x31\x29\x12\x22\x18\x5F\x28\x25" },
    { 11, (dtlNumericType*)"\xCD\x10\x34\xD\xB\x5\x22\x1F\x63\x3C\x55" },
    { 12, (dtlNumericType*)"\xCE\x5\x4\x1E\xF\x3E\xD\x43\x6\x40\x38\x55" },
    { 13, (dtlNumericType*)"\xCF\x2\x9\x59\x57\x5F\x33\x2A\x54\x35\x11\x8\x45" },
    { 13, (dtlNumericType*)"\xCF\x1F\x31\x59\x23\x2F\xC\x48\x27\x3D\x33\x10\x5" },
    { 13, (dtlNumericType*)"\xD0\x9\x55\x12\x3E\x64\x26\x28\x47\x14\x37\x37\x25\x11" },
    { 15, (dtlNumericType*)"\xD1\x3\x42\x1A\x1D\x3C\x52\x16\x5C\x6\x57\x25\x1F\x55\x51" },

    /* 31 ~ 40 */
    { 15, (dtlNumericType*)"\xD1\x53\x17\x54\x57\x37\x12\x50\x17\x52\x4E\x1A\x39\x1D\x51" },
    { 16, (dtlNumericType*)"\xD2\x1B\x20\x1F\x54\x46\x22\x46\x24\x1F\x11\x49\x13\x2\x16\x3D" },
    { 17, (dtlNumericType*)"\xD3\x9\x45\x22\x12\x3E\x59\xC\x59\x41\x60\x34\x52\x5F\x29\xD\x51" },
    { 18, (dtlNumericType*)"\xD4\x3\x60\x18\x1C\x64\x4\x61\x5\xF\x9\x30\x3E\x57\xA\x41\x24\x15" },
    { 18, (dtlNumericType*)"\xD5\x2\x4\x22\xF\x50\x43\x27\x3E\x2D\x5D\x61\x43\x42\xE\x26\x35\x21" },
    { 18, (dtlNumericType*)"\xD5\x26\x14\x5E\x21\x44\x5A\x5B\xD\x12\x2F\x50\x64\x2D\x52\x33\x54\x35" },
    { 19, (dtlNumericType*)"\xD6\xE\x4D\x26\x36\xA\xD\x1B\x23\x33\x2F\x20\x3C\x50\x3B\x10\x51\x5B\x19" },
    { 20, (dtlNumericType*)"\xD7\x6\x18\x3\x1B\x12\x2F\x43\x2\xC\x12\x3D\x1\x49\x19\xB\x1\x4B\x1E\xD" },
    { 21, (dtlNumericType*)"\xD8\x3\x4\x62\x59\x15\x52\x14\x4B\x2C\x24\x57\x29\x1D\x12\x28\x5B\x1D\x62\x25\x45" },
    { 20, (dtlNumericType*)"\xD8\x52\x3C\x10\x1D\x21\x30\x5A\x4E\x23\x23\x39\xC\x1B\x60\x61\xC\x3B\x5F\x1C" },
    
    /* 41 ~ 50 */
    { 20, (dtlNumericType*)"\xD9\x22\x2E\x1A\x1B\x3E\x20\x40\x51\x48\x9\x12\x1\x3F\x6\x23\x29\x4C\x11\x42" },
    { 20, (dtlNumericType*)"\xDA\xF\x6\x1\x3E\x12\x4C\x1D\x50\x5A\x56\x2C\xF\x1B\x7\x19\x2E\xC\x39\x64" },
    { 21, (dtlNumericType*)"\xDB\x7\x5\x10\x1B\x1F\x40\x26\x27\x24\x40\x4A\x38\xE\x15\x45\x34\x28\x62\x33\x49" },
    { 21, (dtlNumericType*)"\xDC\x3\x42\x53\x48\x3A\x30\x59\x2D\x58\x45\x5\x25\x1A\x52\xB\xF\x3E\x3B\x5B\x21" },
    { 21, (dtlNumericType*)"\xDD\x2\x14\x3F\x17\x15\x57\x37\x51\x14\x2E\x3E\x61\x20\x3E\x32\x39\x3A\x48\x33\x41" },
    { 20, (dtlNumericType*)"\xDD\x38\x3\x3F\x16\x3C\x52\x15\x59\x5F\x63\x33\x1F\x37\x1D\x51\x3\x37\x5A\x1E" },
    { 20, (dtlNumericType*)"\xDE\x1A\x57\x18\x19\x10\xC\x11\x52\x51\x41\x1E\x41\x24\x34\x36\x3E\x14\x50\x62" },
    { 20, (dtlNumericType*)"\xDF\xD\x2A\x28\x10\x3C\x1A\x25\x8\x1B\x47\x57\x17\x5A\x5\x4A\x4A\x26\x33\x27" },
    { 21, (dtlNumericType*)"\xE0\x7\x9\x1D\x13\x41\x4\x2B\x44\x39\x9\x49\x1A\x16\x40\x21\xD\x60\x26\x45\x57" },
    { 21, (dtlNumericType*)"\xE1\x4\x5\xF\xA\x21\x2\x48\x22\x4F\x5\x25\xD\x3D\x52\x43\x7\x30\x45\x55\x2C" },
    
    /* 51 ~ 60 */
    { 21, (dtlNumericType*)"\xE2\x2\x38\xC\x13\x4C\x21\x58\x27\x17\x51\x17\x2B\x2C\x2\x41\x46\x1F\x21\xC\x7" },
    { 20, (dtlNumericType*)"\xE2\x51\x42\x52\x4C\x12\xA\x2C\x58\x56\x48\x43\x7\x25\x56\x41\x4\x4D\x46\x4C" },
    { 20, (dtlNumericType*)"\xE3\x2B\x4B\x59\x21\x55\x7\x1\x1A\x39\x2B\x63\x2\x26\x36\x27\x5E\x64\x41\x62" },
    { 20, (dtlNumericType*)"\xE4\x18\x9\x2C\x46\x4A\x28\x19\xE\x51\x30\x15\x5D\x4B\x1B\x54\x3\x4C\x52\x9" },
    { 20, (dtlNumericType*)"\xE5\xD\x46\x41\x4\x24\x25\x3B\x1C\x3C\x1A\x61\x34\x1\x55\x4C\x43\x34\x46\x3D" },
    { 21, (dtlNumericType*)"\xE6\x8\xB\x64\x56\x58\x51\x31\x40\x2E\x13\x37\x5\x39\x30\x2F\x26\x19\x5F\x62\x24" },
    { 21, (dtlNumericType*)"\xE7\x5\x6\x1B\x5C\x60\x5\x58\x49\x11\x4C\x39\x51\x3D\x14\x6\x2C\x18\x17\xE\x32" },
    { 21, (dtlNumericType*)"\xE8\x3\x24\x6\x3E\x22\xD\x53\x58\x56\x48\x53\x5F\x4B\x5C\x6\x10\x8\x2F\x54\x53" },
    { 21, (dtlNumericType*)"\xE9\x2\x27\x45\x20\x13\x37\x39\x5A\x54\x3A\x26\x5E\x5B\x14\x49\x4\x5A\x29\x40\x2E" },
    { 20, (dtlNumericType*)"\xE9\x54\x15\x63\x48\xD\x4B\xE\x5B\xF\x2B\x4D\x23\xC\x54\x17\x22\x41\x27\x8" },
    
    /* 61 ~ 70 */
    { 20, (dtlNumericType*)"\xEA\x33\x4C\x51\x16\x27\x4E\x17\x30\x63\x51\x9\x39\x52\x16\x4D\x3F\x35\x1C\x17" },
    { 20, (dtlNumericType*)"\xEB\x20\x2F\x64\x4A\x1B\x4\x58\x5E\x4C\x1A\x42\x20\x17\x24\x32\x33\x4D\x29\x59" },
    { 20, (dtlNumericType*)"\xEC\x14\x53\x3D\x54\x10\x29\x2D\x29\x7\x2A\x11\xF\x44\x9\x25\x13\x63\xE\x4C" },
    { 20, (dtlNumericType*)"\xED\xD\x45\x57\x5E\x16\x56\x59\x2A\x41\xB\x23\x22\x27\x5E\x24\x11\xF\x51\x51" },
    { 21, (dtlNumericType*)"\xEE\x9\x19\x4D\x33\x3C\x15\x53\x30\x7\x43\x49\x20\x47\x1F\x44\x56\x32\x3F\x35\xE" },
    { 21, (dtlNumericType*)"\xEF\x6\x2D\x23\x32\x28\x8\x4B\x2C\x7\x29\x4\x49\x5D\x29\x19\x4F\x2B\x4C\x1B\x2A" },
    { 21, (dtlNumericType*)"\xF0\x4\x41\x48\xC\xA\x13\x13\x57\x56\x1D\x53\x32\x56\x5B\x61\x3D\x37\x41\x2B\x46" },
    { 21, (dtlNumericType*)"\xF1\x3\x31\x1\x24\x37\x19\x25\x54\x6\x64\x3D\xA\x5B\x2A\x56\x46\x12\x10\x52\x4" },
    { 21, (dtlNumericType*)"\xF2\x2\x48\xD\x19\x35\x2B\x52\x2A\x20\xE\x49\x2F\x54\x27\x59\xD\x49\x54\x5B\x5C" },
    { 21, (dtlNumericType*)"\xF3\x2\x14\x4F\x3A\x11\x46\x61\x63\x5C\x50\x3D\x49\x4F\x26\x16\x45\x5B\x63\x4A\x41" },

    /* 71 ~ 80 */
    { 20, (dtlNumericType*)"\xF3\x56\x5\x4F\x3B\x56\x44\x57\x18\x12\x35\xC\x44\x41\x2B\x28\x5D\x3D\xB\x1D" },
    { 20, (dtlNumericType*)"\xF4\x3E\x18\x2D\x3B\x26\x45\x57\x9\x45\x3E\x35\x29\x47\x27\x35\x4B\x44\x1C\x29" },
    { 20, (dtlNumericType*)"\xF5\x2D\x47\xC\x37\x3E\x34\x1B\x55\x23\x9\x5C\x1A\x48\x27\xD\x33\x34\xC\x2" },
    { 20, (dtlNumericType*)"\xF6\x22\x8\x59\x37\x2A\x34\x5E\x57\x2A\x17\x3C\x36\x3\x53\x16\x1A\x26\x53\xF" },
    { 20, (dtlNumericType*)"\xF7\x19\x51\x5C\x29\x52\xE\x60\x28\x51\x5C\x5F\x41\x4E\xC\x42\x5F\x4\x25\x3E" },
    { 20, (dtlNumericType*)"\xF8\x13\x56\x32\x30\x2\x43\x3D\x33\x1A\x32\x58\x5E\x17\x3D\x57\xC\x2F\x38\x53" },
    { 20, (dtlNumericType*)"\xF9\xF\x34\x54\xA\x15\x1D\x1D\x3B\x46\x40\x29\x47\x4F\x29\x57\x1F\x53\x55\x63" },
    { 20, (dtlNumericType*)"\xFA\xC\x21\x2B\x52\x12\x53\x7\x1E\x4F\x20\x2E\x4C\x16\x10\x58\x21\x5\x3F\x1E" },
    { 21, (dtlNumericType*)"\xFB\x9\x5F\x3E\x53\xE\x8\x53\x62\x35\x57\x56\xF\x2A\x48\x36\x63\x20\x42\x15\x43" },
    { 21, (dtlNumericType*)"\xFC\x8\x10\x46\x2E\x47\x2F\x1B\x27\x3\x1E\x31\xC\x36\x26\x18\x13\x42\x21\x11\x36" },
    
    /* 81 ~ 83 */
    { 21, (dtlNumericType*)"\xFD\x6\x50\x48\x1B\x3\x8\x30\x25\x50\x56\x58\x62\x23\x18\x10\x4F\xB\x5C\x6\x28" },
    { 21, (dtlNumericType*)"\xFE\x5\x4C\x25\x2C\x22\x47\xD\x55\x12\x31\x2B\xE\x53\x7\x63\x5F\x5\x5F\x43\x2B" },
    { 21, (dtlNumericType*)"\xFF\x4\x5F\x38\x18\x61\x62\x15\x42\x57\x34\x13\x62\x30\xC\x51\xD\x7\xB\x3A\xE" },
};


/*******************************************************************************
 * factorial ( ! ) , factorial (prefix operator) ( !! )
 *   factorial ( ! )                    => [ P ]
 *   factorial (prefix operator) ( !! ) => [ P ]
 * ex) 5!  =>  120
 * ex) !!5  => 120
 ******************************************************************************/

/**
 * @brief factorial 함수 [ factorial ( ! ) , factorial (prefix operator) ( !! ) ]
 *        <BR> factorial ( ! ) , factorial (prefix operator) ( !! )
 *        <BR>   factorial ( ! )                    => [ P ]
 *        <BR>   factorial (prefix operator) ( !! ) => [ P ]
 *        <BR> ex) 5!  =>  120
 *        <BR> ex) !!5  => 120
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> (BIGINT)!
 * @param[out] aResultValue      연산 결과 (결과타입 BIGINT)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfBigIntFact( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_NATIVE_BIGINT, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_BIGINT, sErrorStack );

    /* factorial 연산을 한다. */
    STL_TRY_THROW( DTF_BIGINT( sValue1 ) <= DTF_MAX_FACTORIAL_BIGINT,
                   ERROR_OUT_OF_RANGE );
    
    if( DTF_BIGINT( sValue1 ) <= 1 )
    {
        /* 1보다 작거나 같으면 무조건 1임. */
        DTF_BIGINT( sResultValue ) = 1;
    }
    else 
    {
        DTF_BIGINT( sResultValue ) = dtfFactorialBigInt[ DTF_BIGINT( sValue1 ) ];
    }

    sResultValue->mLength = DTL_NATIVE_BIGINT_SIZE;

    
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_NUMERIC_DATA_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
                      NULL,
                      sErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief factorial 함수 [ factorial ( ! ) , factorial (prefix operator) ( !! ) ]
 *        <BR> factorial ( ! ) , factorial (prefix operator) ( !! )
 *        <BR>   factorial ( ! )                    => [ P ]
 *        <BR>   factorial (prefix operator) ( !! ) => [ P ]
 *        <BR> ex) 5!  =>  120
 *        <BR> ex) !!5  => 120
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> (NUMERIC)!
 * @param[out] aResultValue      연산 결과 (결과타입 NUMERIC)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfNumericFact( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;
    
    dtlNumericType  * sNumeric;
    dtlNumericType  * sResultNumeric;

    stlInt32          sFact;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE2( sValue1, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue, DTL_TYPE_FLOAT, DTL_TYPE_NUMBER, sErrorStack );

    sNumeric       = DTF_NUMERIC( sValue1 );
    sResultNumeric = DTF_NUMERIC( sResultValue );

    /**
     * numeric factorial 범위 체크
     * -----------------------------------
     *  1. 정수이어야 한다.
     *  2. 1보다 작은 정수이면 무조건 1이 결과값
     *  3. 입력값의 허용범위는 83보다 작거나 같아야 한다.
     */

    if( DTL_NUMERIC_IS_ZERO( sNumeric, sValue1->mLength ) == STL_TRUE )
    {
        DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( sResultNumeric, 0 );
        *DTL_NUMERIC_GET_DIGIT_PTR( sResultNumeric ) = DTL_NUMERIC_GET_POSITIVE_DIGIT_FROM_VALUE( 1 );
        sResultValue->mLength = 2;
    }
    else
    {
        STL_TRY_THROW( (DTL_NUMERIC_GET_EXPONENT( sNumeric ) + 1) >=
                       DTL_NUMERIC_GET_DIGIT_COUNT( sValue1->mLength ),
                       ERROR_INVALID_ARGUMENT );

        if( DTL_NUMERIC_IS_NEGATIVE( sNumeric ) == STL_TRUE )
        {
            DTL_NUMERIC_SET_SIGN_POSITIVE_AND_EXPONENT( sResultNumeric, 0 );
            *DTL_NUMERIC_GET_DIGIT_PTR( sResultNumeric ) =
                DTL_NUMERIC_GET_POSITIVE_DIGIT_FROM_VALUE( 1 );
            sResultValue->mLength = 2;
        }
        else
        {
            STL_TRY_THROW( (DTL_NUMERIC_GET_DIGIT_COUNT( sValue1->mLength ) == 1) &&
                           (DTL_NUMERIC_GET_EXPONENT_FROM_POSITIVE_VALUE( sNumeric ) == 0),
                           ERROR_OUT_OF_RANGE );

            sFact = DTL_NUMERIC_GET_VALUE_FROM_POSITIVE_DIGIT(
                        *DTL_NUMERIC_GET_DIGIT_PTR( sNumeric ) );

            STL_TRY_THROW( sFact <= DTF_MAX_FACTORIAL_NUMERIC,
                           ERROR_OUT_OF_RANGE );

            sResultValue->mLength = dtfFactorialNumeric[ sFact ].mLength;

            stlMemcpy( sResultNumeric,
                       dtfFactorialNumeric[ sFact ].mNumeric,
                       sResultValue->mLength );
        }
    }

    
    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_NUMERIC_DATA_OUTSIDE_RANGE_DATA_TYPE_NUMBER_CONVERTED,
                      NULL,
                      sErrorStack );
    }

    STL_CATCH( ERROR_INVALID_ARGUMENT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_ARGUMENT_FOR_FACTORIAL_FUNCTION,
                      NULL,
                      sErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
