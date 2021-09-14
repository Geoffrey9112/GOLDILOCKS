/*******************************************************************************
 * dtfString.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtfString.c 1398 2011-07-13 03:22:22Z ehpark $
 *
 * NOTES
 *    NULL (dtlDataValue의 mLength가 0인 경우)처리도 하였음.
 *    input, output 모두 NULL 가능함.
 *    - Add, sub, ... 함수의 경우는 NULL 고려 안되어 있음.
 *      고려 해야 할지??
 *
 *    postgres, mysql은 SQL문에서 null과 ''를 다르게 처리함.
 *    예)
 *    select position('' in  'abc') from dual;
 *    => 1
 *    select position(null in  'abc') from dual;
 *    => [   ]
 *
 *
 *    현재 character set인 UTF-8로만 테스트 하였음.
 *    character에 종속적인 함수는 모두 dtsCharacterSet.c 에
 *    있는 함수를 이용하였음으로 문제 없을것 같음.
 *
 ******************************************************************************/

/**
 * @file dtfString.c
 * @brief String Function DataType Library Layer
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtfString.h>
#include <dtsCharacterSet.h>

/**
 * @addtogroup dtfString String
 * @ingroup dtf
 * @internal
 * @{
 */

/*******************************************************************************
 * concatenation ( || ) , CONCAT(string1, string2)
 *   concatenation ( || )     => [ P, S, O ]
 *   CONCAT(string1, string2) => [ M, O ]
 * ex) 'Goldi' || 'locks'  =>  Goldilocks
 * ex) 'Value: ' || 42     =>  Value: 42
 *******************************************************************************/


/**
 * @brief concatenation ( || )  문자열 결합
 *        <BR> concatenation ( || ) , CONCAT(string1, string2)
 *        <BR>   concatenation ( || )     => [ P, S, O ]
 *        <BR>   CONCAT(string1, string2) => [ M, O ]
 *        <BR> ex) 'Goldi' || 'locks'  =>  Goldilocks
 *        <BR> ex) 'Value: ' || 42     =>  Value: 42
 *        <BR> ex) 'Goldi' || null  =>  Goldi
 *        <BR> ex) null || 'locks'  =>  locks
 *        <BR> ex) null || null  =>  null
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> VARCHAR || VARCHAR
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[in]  aPrecision        해당 타입의 max precision
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfVarcharConcatenate( stlUInt16        aInputArgumentCnt,
                                 dtlValueEntry  * aInputArgument,
                                 void           * aResultValue,
                                 stlInt64         aPrecision,
                                 dtlFuncVector  * aVectorFunc,
                                 void           * aVectorArg,
                                 void           * aEnv )
{
    /**
     * @todo 논의대상
     * 2. 표준에는 리턴타입이 VARCHAR인 경우,
     *    VARCHAR_MAX_LEN을 넘는 부분이 모두 trailing blank이면,
     *    이 부분을 제거하고 VARCHAR_MAX_LEN까지 반환
     *    . 현재 trailing blank도 길이에 포함되어,
     *      len(str1) + len(str2) > VARCHAR_MAX_LEN이면 에러처리됨.
     * 3. 표준에는 str1 과 str2 중 하나가 null인 경우, 결과는 NULL 이라고 되어 있음.
     *    postgres가 표준을 따르고 있으나,
     *    Gliese는 oracle 결과를 따르도록 한다.
     *    . 표준에서 의미하는 바는 다음과 같을 것으로 추정됨.
     *      ( NULL 과 zero length value는 다른 값임. )
     *         예1) NULL || 'AA' ===> NULL
     *         예2) ''   || 'AA' ===> 'AA'
     *    . 현재 str1과 str2 중 하나가 null인 경우, null이 아닌 나머지 str이 반환되고,
     *      str1과 str2가 모두 null인 경우 NULL이 반환
     *      이는 오라클을 따라한 것인데 (즉, zero length value와 NULL의 구분이 없음),
     *      오라클메뉴얼에 향후 버전에는 변경될 것으로 보이며,
     *      null value인 경우 zero length string으로 변환하기 위해
     *      NVL함수를 사용하라고 되어 있음.
     */      
    
    /*
     * postgres가 표준을 따르고 있으나,
     * Gliese는 oracle 결과를 따르도록 한다.
     *  
     * 1. null과의 concatenate 결과
     * 
     *    oracle   : select 'a' || null => 'a'
     *    postgres : select 'a' || null => [   ]
     *    mysql    : select concat('a' || null ) => NULL
     *
     *    => oracle만 'a'로 결과 나옴.
     *    gliese도 'a'결과 나오도록 되어 있음.
     * 
     * 2. zero-length value와의 concatenate 결과
     * 
     *    oracle   : select 'a' || '' => 'a'
     *    postgres : select 'a' || '' => 'a'
     * 
     */
    
    dtlDataValue  * sValue1;
    dtlDataValue  * sValue2;
    dtlDataValue  * sResultValue;
    stlInt64        sCharPrecision1;
    stlInt64        sCharPrecision2;
    stlErrorStack * sErrorStack;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    /**
     * @todo null이 들어올 수 있는 함수임.
     */
    
//    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_VARCHAR, sErrorStack );
//    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_VARCHAR, sErrorStack );
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue,
                           DTL_TYPE_CHAR,
                           DTL_TYPE_VARCHAR,
                           sErrorStack );
    
    /*
     * overflow check.
     * oracle> SP2-0027: Input is too long (> 2499 characters) - line ignored
     */

    STL_TRY_THROW( sValue1->mLength + sValue2->mLength <= sResultValue->mBufferSize,
                   ERROR_OUT_OF_RANGE );
    
    if( (sValue1->mLength + sValue2->mLength) > aPrecision )
    {
        STL_TRY( dtsGetMbstrlenWithLenFuncList[ aVectorFunc->mGetCharSetIDFunc(aVectorArg) ](
                     sValue1->mValue,
                     sValue1->mLength,
                     &sCharPrecision1,
                     aVectorFunc,
                     aVectorArg,
                     sErrorStack )
                 == STL_SUCCESS );

        STL_TRY( dtsGetMbstrlenWithLenFuncList[ aVectorFunc->mGetCharSetIDFunc(aVectorArg) ](
                     sValue2->mValue,
                     sValue2->mLength,
                     &sCharPrecision2,
                     aVectorFunc,
                     aVectorArg,
                     sErrorStack )
                 == STL_SUCCESS );
        
        STL_TRY_THROW( sCharPrecision1 + sCharPrecision2 <= aPrecision,
                       ERROR_OUT_OF_RANGE );
    }

    /* Concatenate 연산을 한다. */
    if( sValue1->mLength > 0 )
    {
        stlMemcpy( DTF_VARCHAR( sResultValue ),
                   DTF_VARCHAR( sValue1 ),
                   sValue1->mLength );
    }
    else
    {
        /* Do Nothing */
    }

    if( sValue2->mLength > 0 )
    {
        stlMemcpy( DTF_VARCHAR( sResultValue ) + sValue1->mLength,
                   DTF_VARCHAR( sValue2 ),
                   sValue2->mLength );
    }
    else
    {
        /* Do Nothing */
    }
    sResultValue->mLength = sValue1->mLength + sValue2->mLength;
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_CHARACTER_LENGTH_GREATER_THAN_COLUMN_LENGTH,
                      NULL,
                      sErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief concatenation ( || )  문자열 결합
 *        <BR> concatenation ( || ) , CONCAT(string1, string2)
 *        <BR>   concatenation ( || )     => [ P, S, O ]
 *        <BR>   CONCAT(string1, string2) => [ M, O ]
 *        <BR> ex) 'Goldi' || 'locks'  =>  Goldilocks
 *        <BR> ex) 'Value: ' || 42     =>  Value: 42
 *        <BR> ex) 'Goldi' || null  =>  Goldi
 *        <BR> ex) null || 'locks'  =>  locks
 *        <BR> ex) null || null  =>  null
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> VARCHAR || VARCHAR
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfLongvarcharConcatenate( stlUInt16        aInputArgumentCnt,
                                     dtlValueEntry  * aInputArgument,
                                     void           * aResultValue,
                                     dtlFuncVector  * aVectorFunc,
                                     void           * aVectorArg,
                                     void           * aEnv )
{
    /**
     * @todo 논의대상
     * 2. 표준에는 리턴타입이 VARCHAR인 경우,
     *    VARCHAR_MAX_LEN을 넘는 부분이 모두 trailing blank이면,
     *    이 부분을 제거하고 VARCHAR_MAX_LEN까지 반환
     *    . 현재 trailing blank도 길이에 포함되어,
     *      len(str1) + len(str2) > VARCHAR_MAX_LEN이면 에러처리됨.
     * 3. 표준에는 str1 과 str2 중 하나가 null인 경우, 결과는 NULL 이라고 되어 있음.
     *    postgres가 표준을 따르고 있으나,
     *    Gliese는 oracle 결과를 따르도록 한다.
     *    . 표준에서 의미하는 바는 다음과 같을 것으로 추정됨.
     *      ( NULL 과 zero length value는 다른 값임. )
     *         예1) NULL || 'AA' ===> NULL
     *         예2) ''   || 'AA' ===> 'AA'
     *    . 현재 str1과 str2 중 하나가 null인 경우, null이 아닌 나머지 str이 반환되고,
     *      str1과 str2가 모두 null인 경우 NULL이 반환
     *      이는 오라클을 따라한 것인데 (즉, zero length value와 NULL의 구분이 없음),
     *      오라클메뉴얼에 향후 버전에는 변경될 것으로 보이며,
     *      null value인 경우 zero length string으로 변환하기 위해
     *      NVL함수를 사용하라고 되어 있음.
     */      
    
    /*
     * postgres가 표준을 따르고 있으나,
     * Gliese는 oracle 결과를 따르도록 한다.
     *  
     * 1. null과의 concatenate 결과
     * 
     *    oracle   : select 'a' || null => 'a'
     *    postgres : select 'a' || null => [   ]
     *    mysql    : select concat('a' || null ) => NULL
     *
     *    => oracle만 'a'로 결과 나옴.
     *    gliese도 'a'결과 나오도록 되어 있음.
     * 
     * 2. zero-length value와의 concatenate 결과
     * 
     *    oracle   : select 'a' || '' => 'a'
     *    postgres : select 'a' || '' => 'a'
     * 
     */
    
    dtlDataValue  * sValue1;
    dtlDataValue  * sValue2;
    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;
    stlInt64        sResultLength;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    /**
     * @todo null이 들어올 수 있는 함수임.
     */
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_LONGVARCHAR, sErrorStack );
    
    /*
     * overflow check.
     * oracle> SP2-0027: Input is too long (> 2499 characters) - line ignored
     */

    sResultLength = sValue1->mLength + sValue2->mLength;
    
    STL_TRY_THROW( sResultLength <= DTL_LONGVARCHAR_MAX_PRECISION,
                   ERROR_OUT_OF_RANGE );

    /* 공간이 부족하다면 공간을 확보한다. */
    if( sResultLength > sResultValue->mBufferSize )
    {
        STL_TRY( aVectorFunc->mReallocLongVaryingMemFunc( aVectorArg,
                                                          sResultLength,
                                                          &sResultValue->mValue,
                                                          sErrorStack )
                 == STL_SUCCESS );

        sResultValue->mBufferSize = sResultLength;
    }

    /* Concatenate 연산을 한다. */
    if( sValue1->mLength > 0 )
    {
        stlMemcpy( DTF_VARCHAR( sResultValue ),
                   DTF_VARCHAR( sValue1 ),
                   sValue1->mLength );
    }
    else
    {
        /* Do Nothing */
    }

    if( sValue2->mLength > 0 )
    {
        stlMemcpy( DTF_VARCHAR( sResultValue ) + sValue1->mLength,
                   DTF_VARCHAR( sValue2 ),
                   sValue2->mLength );
    }
    else
    {
        /* Do Nothing */
    }
    
    sResultValue->mLength = sResultLength;
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_RANGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_CHARACTER_LENGTH_GREATER_THAN_COLUMN_LENGTH,
                      NULL,
                      sErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief concatenation ( || ) binary 문자열 결합
 *        <BR> concatenation ( || ) , CONCAT(string1, string2)
 *        <BR>   concatenation ( || )     => [ P, S, O ]
 *        <BR>   CONCAT(string1, string2) => [ M, O ]
 *        <BR> ex) 'Goldi' || 'locks'  =>  Goldilocks
 *        <BR> ex) 'Value: ' || 42     =>  Value: 42
 *        <BR> ex) 'Goldi' || null  =>  Goldi
 *        <BR> ex) null || 'locks'  =>  locks
 *        <BR> ex) null || null  =>  null
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> VARBINARY || VARBINARY
 * @param[out] aResultValue      연산 결과 (결과타입 VARBINARY)
 * @param[in]  aPrecision        해당 타입의 max precision
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfVarbinaryConcatenate( stlUInt16        aInputArgumentCnt,
                                   dtlValueEntry  * aInputArgument,
                                   void           * aResultValue,
                                   stlInt64         aPrecision,
                                   dtlFuncVector  * aVectorFunc,
                                   void           * aVectorArg,                                      
                                   void           * aEnv )
{
    /**
     * @todo 논의대상
     * 2. 표준에는 리턴타입이 VARCHAR인 경우,
     *    VARCHAR_MAX_LEN을 넘는 부분이 모두 trailing blank이면,
     *    이 부분을 제거하고 VARCHAR_MAX_LEN까지 반환
     *    . 현재 trailing blank도 길이에 포함되어,
     *      len(str1) + len(str2) > VARCHAR_MAX_LEN이면 에러처리됨.
     * 3. 표준에는 str1 과 str2 중 하나가 null인 경우, 결과는 NULL 이라고 되어 있음.
     *    . 현재 str1과 str2 중 하나가 null인 경우, null이 아닌 나머지 str이 반환되고,
     *      str1과 str2가 모두 null인 경우 NULL이 반환
     *      이는 오라클을 따라한 것인데,
     *      오라클메뉴얼에 이런 부분이 지속되지 않을것처럼 되어있고, NVL을 쓰라고 함.
     */      
    
    /*
     * null관련 연산은 dtfStringConcatenate 정책과 동일함.
     */
    dtlDataValue  * sValue1;
    dtlDataValue  * sValue2;
    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    /**
     * @todo null이 들어올 수 있는 함수임.
     */
    
//    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_VARBINARY, sErrorStack );
//    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_VARBINARY, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue,
                           DTL_TYPE_BINARY,
                           DTL_TYPE_VARBINARY,
                           sErrorStack );

    /*
     * overflow check.
     */
    STL_TRY_THROW( ( sValue1->mLength + sValue2->mLength <= sResultValue->mBufferSize ) &&
                   ( sValue1->mLength + sValue2->mLength <= aPrecision ),
                   ERROR_OUT_OF_RANGE );

    /* Concatenate 연산을 한다. */
    if( sValue1->mLength > 0 )
    {
        stlMemcpy( DTF_VARBINARY( sResultValue ),
                   DTF_VARBINARY( sValue1 ),
                   sValue1->mLength );
    }
    else
    {
        /* Do Nothing */
    }

    if( sValue2->mLength > 0 )
    {
        stlMemcpy( DTF_VARBINARY( sResultValue ) + sValue1->mLength,
                   DTF_VARBINARY( sValue2 ),
                   sValue2->mLength );
    }
    else
    {
        /* Do Nothing */
    }
    sResultValue->mLength = sValue1->mLength + sValue2->mLength;
    
    return STL_SUCCESS;

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
 * @brief concatenation ( || ) binary 문자열 결합
 *        <BR> concatenation ( || ) , CONCAT(string1, string2)
 *        <BR>   concatenation ( || )     => [ P, S, O ]
 *        <BR>   CONCAT(string1, string2) => [ M, O ]
 *        <BR> ex) 'Goldi' || 'locks'  =>  Goldilocks
 *        <BR> ex) 'Value: ' || 42     =>  Value: 42
 *        <BR> ex) 'Goldi' || null  =>  Goldi
 *        <BR> ex) null || 'locks'  =>  locks
 *        <BR> ex) null || null  =>  null
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> BINARY || BINARY
 * @param[out] aResultValue      연산 결과 (결과타입 BINARY)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfLongvarbinaryConcatenate( stlUInt16        aInputArgumentCnt,
                                       dtlValueEntry  * aInputArgument,
                                       void           * aResultValue,
                                       dtlFuncVector  * aVectorFunc,
                                       void           * aVectorArg,                                 
                                       void           * aEnv )
{
    /**
     * CONCATENATE( BINARY TYPE, BINARY TYPE ) 인 경우,
     * 결과타입은 BINARY TYPE.
     * 표준에 있는 내용.
     * 결과타입의 크기는 len(str1)+len(str2)가 DTL_LONGVARBINARY_MAX_PRECISION 범위이어야 함.
     */

    /**
     * @todo 논의대상
     * 2. 표준에는 리턴타입이 VARCHAR인 경우,
     *    VARCHAR_MAX_LEN을 넘는 부분이 모두 trailing blank이면,
     *    이 부분을 제거하고 VARCHAR_MAX_LEN까지 반환
     *    . 현재 trailing blank도 길이에 포함되어,
     *      len(str1) + len(str2) > VARCHAR_MAX_LEN이면 에러처리됨.
     * 3. 표준에는 str1 과 str2 중 하나가 null인 경우, 결과는 NULL 이라고 되어 있음.
     *    . 현재 str1과 str2 중 하나가 null인 경우, null이 아닌 나머지 str이 반환되고,
     *      str1과 str2가 모두 null인 경우 NULL이 반환
     *      이는 오라클을 따라한 것인데,
     *      오라클메뉴얼에 이런 부분이 지속되지 않을것처럼 되어있고, NVL을 쓰라고 함.
     */      
    
    /*
     * null관련 연산은 dtfStringConcatenate 정책과 동일함.
     */
    dtlDataValue  * sValue1;
    dtlDataValue  * sValue2;
    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;
    stlInt64        sResultLength;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    /**
     * @todo null이 들어올 수 있는 함수임.
     */
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_LONGVARBINARY, sErrorStack );

    /*
     * overflow check.
     */
    
    sResultLength = sValue1->mLength + sValue2->mLength;
    
    STL_TRY_THROW( sResultLength <= DTL_LONGVARBINARY_MAX_PRECISION,
                   ERROR_OUT_OF_RANGE );

    /* 공간이 부족하다면 공간을 확보한다. */
    if( sResultLength > sResultValue->mBufferSize )
    {
        STL_TRY( aVectorFunc->mReallocLongVaryingMemFunc( aVectorArg,
                                                          sResultLength,
                                                          &sResultValue->mValue,
                                                          sErrorStack )
                 == STL_SUCCESS );

        sResultValue->mBufferSize = sResultLength;
    }

    /* Concatenate 연산을 한다. */
    if( sValue1->mLength > 0 )
    {
        stlMemcpy( DTF_BINARY( sResultValue ),
                   DTF_BINARY( sValue1 ),
                   sValue1->mLength );
    }
    else
    {
        /* Do Nothing */
    }

    if( sValue2->mLength > 0 )
    {
        stlMemcpy( DTF_BINARY( sResultValue ) + sValue1->mLength,
                   DTF_BINARY( sValue2 ),
                   sValue2->mLength );
    }
    else
    {
        /* Do Nothing */
    }
    
    sResultValue->mLength = sResultLength;
    
    return STL_SUCCESS;

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


/*******************************************************************************
 * bit_length
 *   bit_length => [ P, M ]
 * ex) bit_length('jose')  =>  32
 *******************************************************************************/

/**
 * @brief bit_length 문자열내의 비트수 반환
 *        <BR> bit_length
 *        <BR>   bit_length => [ P, M ]
 *        <BR> ex) bit_length('jose')  =>  32
 *        <BR> ex) bit_length(null)  =>  null
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> bit_length(VARCHAR)
 * @param[out] aResultValue      연산 결과 (결과타입 BIGINT)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfBitLength( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aEnv )
{
    /*
     * input이 null인경우 결과
     * oracle   : select lengthb(null) =>  [   ]
     * postgres : select bit_length(null) =>  [   ]
     * mysql    : select bit_length(null) => NULL
     *
     * input이 '' 인경우 결과
     * oracle   : select lengthb('') =>  [   ]
     * postgres : select bit_length('') => 0
     * mysql    : select bit_length('') => 0
     *
     * gliese는 null인 경우 oracle 따라감.
     * gliese에서 ''는 없는 data type임.
     */
    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE2( sValue1,
                          DTL_TYPE_CHAR,
                          DTL_TYPE_VARCHAR,
                          (stlErrorStack *)aEnv );
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_BIGINT, (stlErrorStack *)aEnv );
    
    /* bit 수를 계산 한다. */
    STL_DASSERT( DTL_IS_NULL( sValue1 ) != STL_TRUE );
    
    DTF_BIGINT( sResultValue ) = sValue1->mLength * 8;
    sResultValue->mLength = DTL_NATIVE_BIGINT_SIZE;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/*******************************************************************************
 * long_varchar_bit_length
 *   bit_length => [ P, M ]
 * ex) bit_length('jose')  =>  32
 *******************************************************************************/

/**
 * @brief bit_length 문자열내의 비트수 반환
 *        <BR> bit_length
 *        <BR>   bit_length => [ P, M ]
 *        <BR> ex) bit_length('jose')  =>  32
 *        <BR> ex) bit_length(null)  =>  null
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> bit_length(VARCHAR)
 * @param[out] aResultValue      연산 결과 (결과타입 BIGINT)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfLongvarcharBitLength( stlUInt16        aInputArgumentCnt,
                                   dtlValueEntry  * aInputArgument,
                                   void           * aResultValue,
                                   void           * aEnv )
{
    /*
     * input이 null인경우 결과
     * oracle   : select lengthb(null) =>  [   ]
     * postgres : select bit_length(null) =>  [   ]
     * mysql    : select bit_length(null) => NULL
     *
     * input이 '' 인경우 결과
     * oracle   : select lengthb('') =>  [   ]
     * postgres : select bit_length('') => 0
     * mysql    : select bit_length('') => 0
     *
     * gliese는 null인 경우 oracle 따라감.
     * gliese에서 ''는 없는 data type임.
     */
    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_GROUP2( sValue1,
                          DTL_GROUP_CHAR_STRING,
                          DTL_GROUP_LONGVARCHAR_STRING,
                          (stlErrorStack *)aEnv );
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_BIGINT, (stlErrorStack *)aEnv );

    /* bit 수를 계산 한다. */
    STL_DASSERT( DTL_IS_NULL( sValue1 ) != STL_TRUE );
    
    DTF_BIGINT( sResultValue ) = sValue1->mLength * 8;
    sResultValue->mLength = DTL_NATIVE_BIGINT_SIZE;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*******************************************************************************
 * char_length(string) or character_length(string), length(string)
 *   char_length(string) or character_length(string) => [ P, S, M ]
 *   length(string) => [ O ]
 *   O사에서는 length(string)을 byte length를 반환하는 octect_length임.
 * ex) char_length('jose')  =>  4
 *******************************************************************************/

/**
 * @brief char_length(string) or character_length(string)
 *        <BR> 문자열내의 문자수 반환
 *        <BR> char_length(string) or character_length(string), length(string)
 *        <BR>   char_length(string) or character_length(string) => [ P, S, M ]
 *        <BR>   length(string) => [ O ]
 *        <BR>   O사에서는 length(string)을 byte length를 반환하는 octect_length임.
 *        <BR> ex) char_length('jose')  =>  4
 *        <BR> ex) char_length(null)  =>  null
 *   
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> char_length(VARCHAR)
 * @param[out] aResultValue      연산 결과 (결과타입 BIGINT)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 * @remark  현재 character set 기준으로 문자 개수를 반환한다.
 */
stlStatus dtfCharLength( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv )
{
    /*
     * mysql, postgres는 모두 character set에 기반한 문자 개수 반환함.
     * oracle> select length('ab 소프트') from dual; => 12  ( utf8 임 )
     * mysql> select char_length('ab 소프트') from dual; => 6
     * postgres> select char_length('ab 소프트') from dual; => 6
     * gliese 에서는 mysql, postgres처럼 6이 나오도록 했음.
     *
     * input이 null인 경우는 dtfBitLength 정책 따라감.
     */

    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;
    stlInt64        sCharCount;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_VALUE2( sValue1,
                          DTL_TYPE_CHAR,
                          DTL_TYPE_VARCHAR,
                          (stlErrorStack *)aEnv );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_BIGINT, sErrorStack );
    
    /* Character 수를 계산 한다. */
    STL_DASSERT( DTL_IS_NULL( sValue1 ) != STL_TRUE );
    
    STL_TRY( dtsGetMbstrlenWithLenFuncList[ aVectorFunc->mGetCharSetIDFunc(aVectorArg) ](
                 DTF_VARCHAR( sValue1 ),
                 sValue1->mLength,
                 &sCharCount,
                 aVectorFunc,
                 aVectorArg,
                 sErrorStack )
             == STL_SUCCESS );
    DTF_BIGINT( sResultValue ) = sCharCount;
    sResultValue->mLength = DTL_NATIVE_BIGINT_SIZE;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/*******************************************************************************
 * char_length(string) or character_length(string), length(string)
 *   char_length(string) or character_length(string) => [ P, S, M ]
 *   length(string) => [ O ]
 *   O사에서는 length(string)을 byte length를 반환하는 octect_length임.
 * ex) char_length('jose')  =>  4
 *******************************************************************************/

/**
 * @brief char_length(string) or character_length(string)
 *        <BR> 문자열내의 문자수 반환
 *        <BR> char_length(string) or character_length(string), length(string)
 *        <BR>   char_length(string) or character_length(string) => [ P, S, M ]
 *        <BR>   length(string) => [ O ]
 *        <BR>   O사에서는 length(string)을 byte length를 반환하는 octect_length임.
 *        <BR> ex) char_length('jose')  =>  4
 *        <BR> ex) char_length(null)  =>  null
 *   
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> char_length(VARCHAR)
 * @param[out] aResultValue      연산 결과 (결과타입 BIGINT)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 * @remark  현재 character set 기준으로 문자 개수를 반환한다.
 */
stlStatus dtfLongvarcharLength( stlUInt16        aInputArgumentCnt,
                                dtlValueEntry  * aInputArgument,
                                void           * aResultValue,
                                dtlFuncVector  * aVectorFunc,
                                void           * aVectorArg,
                                void           * aEnv )
{
    /*
     * mysql, postgres는 모두 character set에 기반한 문자 개수 반환함.
     * oracle> select length('ab 소프트') from dual; => 12  ( utf8 임 )
     * mysql> select char_length('ab 소프트') from dual; => 6
     * postgres> select char_length('ab 소프트') from dual; => 6
     * gliese 에서는 mysql, postgres처럼 6이 나오도록 했음.
     *
     * input이 null인 경우는 dtfBitLength 정책 따라감.
     */

    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;
    stlInt64        sCharCount;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_GROUP2( sValue1,
                          DTL_GROUP_CHAR_STRING,
                          DTL_GROUP_LONGVARCHAR_STRING,
                          sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_BIGINT, sErrorStack );

    /* Character 수를 계산 한다. */
    STL_DASSERT( DTL_IS_NULL( sValue1 ) != STL_TRUE );
    
    STL_TRY( dtsGetMbstrlenWithLenFuncList[ aVectorFunc->mGetCharSetIDFunc(aVectorArg) ](
                 DTF_VARCHAR( sValue1 ),
                 sValue1->mLength,
                 &sCharCount,
                 aVectorFunc,
                 aVectorArg,
                 sErrorStack )
             == STL_SUCCESS );
    DTF_BIGINT( sResultValue ) = sCharCount;
    sResultValue->mLength = DTL_NATIVE_BIGINT_SIZE;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/*******************************************************************************
 * lower(string)
 *   lower(string) => [ P, S, M, O ]
 * ex) lower('TOM')  =>  tom
 *******************************************************************************/

/**
 * @brief lower(string) 문자열을 소문자로 반환
 *        <BR> lower(string)
 *        <BR>   lower(string) => [ P, S, M, O ]
 *        <BR> ex) lower('TOM')  =>  tom
 *        <BR> ex) lower(null)  =>  null
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> lower(CHAR or VARCHAR)
 * @param[out] aResultValue      연산 결과 (결과타입 CHAR or VARCHAR)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfLower( stlUInt16        aInputArgumentCnt,
                    dtlValueEntry  * aInputArgument,
                    void           * aResultValue,
                    dtlFuncVector  * aVectorFunc,
                    void           * aVectorArg,
                    void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;
    stlChar         * sInput;
    stlChar         * sResult;
    stlInt32          sGap;
    stlInt64          i;
    stlInt8           sLen;

    dtlCharacterSet   sCharSetID;
    
    /**
     * Parameter Validation
     */
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    /**
     * aInputArgument type은 DTL_TYPE_CHAR 또는 DTL_TYPE_VARCHAR 이어야 한다.
     * aResultValue type은 DTL_TYPE_CHAR 또는 DTL_TYPE_VARCHAR 이어야 한다.
     * 예) lower( DTL_TYPE_CHAR )    ==> result : DTL_TYPE_CHAR
     *     lower( DTL_TYPE_VARCHAR ) ==> result : DTL_TYPE_VARCHAR
     */

    DTF_CHECK_DTL_GROUP2( sValue1,
                          DTL_GROUP_CHAR_STRING,
                          DTL_GROUP_LONGVARCHAR_STRING,
                          sErrorStack );
    
    DTF_CHECK_DTL_RESULT3( sValue1,
                           DTL_TYPE_CHAR,
                           DTL_TYPE_VARCHAR,
                           DTL_TYPE_LONGVARCHAR,
                           sErrorStack );    
    
    sCharSetID = aVectorFunc->mGetCharSetIDFunc(aVectorArg);

    STL_DASSERT( sValue1->mLength <= sResultValue->mBufferSize );
    
    /* 소문자로 변경 한다. */
    sGap = 'A' - 'a';
    sInput = sValue1->mValue;
    sResult = sResultValue->mValue;    
    i = 0;
    while( i < sValue1->mLength )
    {
        /*
         * 현재 character set의 1char의 길이를 구한다.
         * 1char이 1인 경우만 변경한다.
         * 현재는 ascii, utf8, ms949 뿐임으로 이 방식이 가능하다.
         * unicode등의 character가 추가되면 logic이 변경되어야 한다.
         */
        STL_TRY( dtsGetMbLengthFuncList[sCharSetID]( sInput,
                                                     &sLen,
                                                     sErrorStack )
                 == STL_SUCCESS );
        
        if ( sLen == 1)
        {
            if( ( *sInput >= 'A' ) && ( *sInput <= 'Z' ) )
            {
                *sResult = *sInput - sGap;
            }
            else
            {
                *sResult = *sInput;
            }
            sInput++;
            sResult++;
            i++;
        }
        else
        {
            /*
             * 1char의 길이가 2이상임으로 다음 char로 넘어간다.
             */
            stlMemcpy( sResult,
                       sInput,
                       sLen );
            i += sLen;
            sInput += sLen;
            sResult += sLen;
        }
    }

    sResultValue->mLength = sValue1->mLength;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*******************************************************************************
 * octet_length(string), lengthb(string)
 *   octet_length(string) => [ P, S, M ]
 *   lengthb(string) => [ O ]
 *   M사에서는 length(string)이 byte length를 반환하는 octet_length()임.
 * ex) octet_length('jose')  =>  4
 *******************************************************************************/

/**
 * @brief octet_length(string) 문자열내의 바이트수를 반환
 *        <BR> octet_length(string), lengthb(string)
 *        <BR>   octet_length(string) => [ P, S, M ]
 *        <BR>   lengthb(string) => [ O ]
 *        <BR> ex) octet_length('jose')  =>  4
 *        <BR> ex) octet_length(null)  =>  null
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> octet_length(VARCHAR)
 * @param[out] aResultValue      연산 결과 (결과타입 BIGINT)
 * @param[out] aEnv              environment (stlErrorStack)
 * @remark character set과 상관없이 사용하는 메모리 byte수를 반환한다.
 */
stlStatus dtfStringOctetLength( stlUInt16        aInputArgumentCnt,
                                dtlValueEntry  * aInputArgument,
                                void           * aResultValue,
                                void           * aEnv )
{
    /*
     * input이 null인 경우는 dtfBitLength 정책 따라감.
     */
    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    
    DTF_CHECK_DTL_GROUP2( sValue1,
                          DTL_GROUP_CHAR_STRING,
                          DTL_GROUP_LONGVARCHAR_STRING,
                          (stlErrorStack *)aEnv );    
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_BIGINT, (stlErrorStack *)aEnv );
    
    /* byte 수를 계산 한다. */
    STL_DASSERT( DTL_IS_NULL( sValue1 ) != STL_TRUE );
    
    DTF_BIGINT( sResultValue ) = sValue1->mLength;
    sResultValue->mLength = DTL_NATIVE_BIGINT_SIZE;
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief octet_length(string) binary 문자열내의 바이트수를 반환
 *        <BR> octet_length(string), lengthb(string)
 *        <BR>   octet_length(string) => [ P, S, M ]
 *        <BR>   lengthb(string) => [ O ]
 *        <BR> ex) octet_length('jose')  =>  4
 *        <BR> ex) octet_length(null)  =>  null
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> octet_length(VARBINARY)
 * @param[out] aResultValue      연산 결과 (결과타입 BIGINT)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfBinaryStringOctetLength( stlUInt16        aInputArgumentCnt,
                                      dtlValueEntry  * aInputArgument,
                                      void           * aResultValue,
                                      void           * aEnv )
{
    /*
     * input이 null인 경우는 dtfBitLength 정책 따라감.
     */
    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_GROUP2( sValue1,
                          DTL_GROUP_BINARY_STRING,
                          DTL_GROUP_LONGVARBINARY_STRING,
                          (stlErrorStack *)aEnv );    

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_BIGINT, (stlErrorStack *)aEnv );

    /* byte 수를 계산 한다. */
    STL_DASSERT( DTL_IS_NULL( sValue1 ) != STL_TRUE );
    
    DTF_BIGINT( sResultValue ) = sValue1->mLength;
    sResultValue->mLength = DTL_NATIVE_BIGINT_SIZE;
    
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/*******************************************************************************
 * overlay(string placing string from int [for int] [using char_length_unit] )
 *   overlay(string placing string from int [for int] [using char_length_unit]) => [ P, S ]
 * ex) overlay('Tdtlxas' placing 'hom' from 2 for 4)  =>  Thomas
 *******************************************************************************/

/**
 * @brief overlay(string placing string from int [for int])
 *        <BR> string의 substring의 치환
 *        <BR> overlay(string placing string from int [for int] [using char_length_unit] )
 *        <BR>   overlay(string placing string from int [for int] [using char_length_unit]) => [ P, S ]
 *        <BR> ex) overlay('Tdtlxas' placing 'hom' from 2 for 4)  =>  Thomas
 *        <BR> ex) overlay('Tdtlxas' placing 'hom' from 0 for 4)  =>  homxas
 *        <BR> ex) overlay('Tdtlxas' placing 'hom' from -2 for 4)  =>  Tdtlxhom
 *        <BR> ex) overlay('Tdtlxas' placing 'hom' from 12 for 4)  =>  Tdtlxashom
 *        <BR> ex) overlay('Tdtlxas' placing 'hom' from -12 for 4)  =>  hom
 *        <BR> ex) overlay('Tdtlxas' placing 'hom'xfrom 2 for -4)  =>  Thomas
 *        <BR> ex) overlay('Tdtlxas' placing null from 2 for 4)  =>  Tas
 *        <BR> ex) overlay(null placing 'hom' from 2 for 4)  =>  hom
 *   
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> overlay(VARCHAR, VARCHAR, INTEGER, INTEGER, INTEGER)
 *                               <BR> overlay(string, placing string, from , for, using )
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfStringOverlay( stlUInt16        aInputArgumentCnt,
                            dtlValueEntry  * aInputArgument,
                            void           * aResultValue,
                            dtlFuncVector  * aVectorFunc,
                            void           * aVectorArg,
                            void           * aEnv )
{
    /*
     * 표준에 다음과 같이 기술되어 있다. (우선 using절 생략하고 ...)
     *
     * 구문 : OVERLAY( str1 PLACING str2 FROM start_position [ FOR string_length ] ) 
     *                 ----         ----      --------------       -------------
     * 
     * OVERLAY는 다음과 같은 결과이다.
     *
     *   * FOR가 지정된 경우
     *     SUBSTRING( str1 FROM 1 FOR (start_position - 1) )
     *     || str2
     *     || SUBSTRING( str1 FROM (start_position + string_length )
     * 
     *   * FOR가 생략된 경우
     *      SUBSTRING( str1 FROM 1 FOR (start_position - 1) )
     *      || str2
     *      || SUBSTRING( str1 FROM (start_position + CHAR_LENGTH(str2) )
     *
     * <참고> 
     * substring함수와 ||함수는 postgres가 표준을 따르고 있으나,
     * oracle 결과를 따라가는 정책으로 정해짐.
     * 참조: substring, || 함수 주석.
     */
        
    /*
     * postgres의 결과는 아래와 같다.
     * select overlay('abcdefghi' placing 'dtlx' from -2 for 4) from dual;
     * postgres> ERROR:  negative substring length not allowed
     *
     * => length는 4인고 position이 -2인데 error message가 이상함.
     * 소스를 봐도 position이 음수면 'negaitve length' 메시지가 나오도록 되어 있음.
     *
     * gliese는 dtfStringSubstring 정책을 따라간다.
     * dtfStringSubstring에서는 oracle과 동일하게 처리함으로 error 가 아니다.
     * 위의 ex 참고
     */
    

    dtlDataValue  * sValue1;
    dtlDataValue  * sValue2;
    dtlDataValue  * sValue3;
    dtlDataValue  * sValue4;
//  dtlDataValue  * sValue5;  /* asign만 하고 사용하지 않는 변수 주석처리 */
    dtlDataValue  * sResultValue;
    stlInt64        sResultLength;
    stlErrorStack * sErrorStack;
    stlInt64        sFromLength = 0;
    stlInt64        sStartPosition = 0;
    stlInt64        sByteStartPosition = 0;
    stlInt64        sStringLength = 0;
    stlInt64        sByteStringLength = 0;
    stlInt64        sValue2CharLength = 0;
    stlInt64        sCharLength = 0;
    stlInt64        sCheckResultPrecision = 0;
    stlBool         sCalculateValue2CharLength = STL_FALSE;
    stlBool         sIsOver = STL_FALSE;
    dtlCharacterSet sCharSetID = DTL_CHARACTERSET_MAX;

    /**
     * @todo USING절에 대한 점검이 필요해서
     *  <BR> 우선 parsing 단계에서 USING절은 파싱되지 않도록 막음. 
     */
    DTL_PARAM_VALIDATE( (aInputArgumentCnt >= 3) && (aInputArgumentCnt <=  5),
                        (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sValue3 = aInputArgument[2].mValue.mDataValue;
    sValue4 = NULL;
//  sValue5 = aInputArgument[4].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    /*
     * input argument validate는 생략한다.
     * sValue1(str1), sValue2(str2), sValue3( FROM ) 에는 NULL이 올 수 있다.
     * sValue4( FOR )는 NULL이 올 수 있고, 생략될 수 있다.
     * sValue5( USING )는 생략될 수 있다.
     */
        
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_VARCHAR, sErrorStack );

    /*----------------------------------------------------------------------------
     * OVERLAY( str1 PLACING str2 FROM start_position [ FOR string_length ] ) 에서
     * 
     * 1. (str1 == NULL) && (str2 == NULL) 이면 결과는 NULL이다.
     *       SUBSTRING( NULL FROM 1 FOR (start_position - 1) )      <-- NULL
     *    || NULL                                                   <-- NULL
     *    || SUBSTRING( NULL FROM (start_position + string_length ) <-- NULL
     * 
     * 2. (str2 == NULL) && (start_position == NULL) 이면 결과는 NULL이다.
     *       SUBSTRING( str1 FROM 1 FOR (NULL - 1) )      <-- NULL
     *    || NULL                                         <-- NULL
     *    || SUBSTRING( str1 FROM (NULL + string_length ) <-- NULL
     *----------------------------------------------------------------------------*/
    if( ( (sValue1->mLength == 0) && (sValue2->mLength == 0) ) ||
        ( (sValue2->mLength == 0) && (sValue3->mLength == 0) ) )
    {
        DTL_SET_NULL( sResultValue );

        STL_THROW( SUCCESS_END );
    }
    else
    {
        // Do Nothing
    }

    /*
     * From length 정보 설정
     */
    if( sValue3->mLength == 0 )
    {
        /* From length 가 NULL인 경우 */
        sFromLength = STL_INT64_MAX;
    }
    else
    {
        sFromLength = DTF_INTEGER(sValue3);
        if( sFromLength == 0 )
        {
            sFromLength = 1;
        }
        else
        {
            /* Do Nothing */
        }
    }

    /*----------------------------------------------------------------------------
     * SUBSTRING( str1 FROM 1 FOR (start_position - 1) )  <===== 이 부분에 대한 수행.
     *     || str2
     *     || SUBSTRING( str1 FROM (start_position + string_length )
     *----------------------------------------------------------------------------*/
    
    sResultValue->mLength = 0;
    sCharSetID = aVectorFunc->mGetCharSetIDFunc(aVectorArg);
    
    if( (sValue1->mLength == 0) ||
        (sFromLength == STL_INT64_MAX) )
    {
        /*
         * 1. (sValue1->mLength == 0) 
         *    SUBSTRING( NULL FROM 1 FOR (start_position - 1) )
         * 2. (sFromLength == STL_INT64_MAX)   
         *    SUBSTRING( str1 FROM 1 FOR (NULL - 1) )
         *    
         * 인 경우로, 이 부분의 수행 결과는 NULL
         */ 
        sStringLength = 0;
    }
    else
    {
        sStringLength = sFromLength - 1;

        if( sStringLength <= 0 )
        {
            /*
             * SUBSTRING( str1 FROM 1 FOR (start_position - 1) )
             *                            --------------------
             * 에서 FOR value가 0 또는 음수인 경우로 이 부분의 수행 결과는 NULL                                
             */
            
            sStringLength = 0;
        }
        else
        {
            STL_TRY( dtfMbLength2BinaryLength( DTF_VARCHAR(sValue1),
                                               sValue1->mLength,
                                               sStringLength,
                                               & sByteStringLength,
                                               & sIsOver,
                                               aVectorFunc,
                                               aVectorArg,
                                               sErrorStack )
                     == STL_SUCCESS );

            /* 복사 */
            STL_TRY( dtfBinaryStringSubstringInternal( DTF_VARCHAR(sValue1),
                                                       sValue1->mLength,
                                                       1,
                                                       sByteStringLength,
                                                       DTF_VARCHAR(sResultValue),
                                                       & sResultValue->mLength,
                                                       sErrorStack )
                     == STL_SUCCESS );
        }
    }

    sCheckResultPrecision = sStringLength;
    
    /*----------------------------------------------------------------------------
     * SUBSTRING( str1 FROM 1 FOR (start_position - 1) )  
     *     || str2                                        <===== 이 부분에 대한 수행.
     *     || SUBSTRING( str1 FROM (start_position + string_length )
     *----------------------------------------------------------------------------*/

    if( sValue2->mLength == 0 )
    {
        /* str2 가 NULL 인  경우로 이 부분 수행결과는 NULL */
        // Do Nothing
    }
    else
    {
        /*
         * overflow check
         */

        if( (sCheckResultPrecision + sValue2->mLength) <= DTL_VARCHAR_MAX_PRECISION )
        {
            // Do Nothing
        }
        else
        {
            /* CHAR_LENGTH(sValue2)를 구한다. */
            STL_TRY( dtsGetMbstrlenWithLenFuncList[ sCharSetID ]( DTF_VARCHAR(sValue2),
                                                                  sValue2->mLength,
                                                                  & sValue2CharLength,
                                                                  aVectorFunc,
                                                                  aVectorArg,
                                                                  sErrorStack )
                     == STL_SUCCESS );
            
            sCalculateValue2CharLength = STL_TRUE;
            
            sCheckResultPrecision += sValue2CharLength;
            STL_TRY_THROW( sCheckResultPrecision <= DTL_VARCHAR_MAX_PRECISION,
                           ERROR_OUT_OF_PRECISION );
        }

        /* str2 복사 */
        stlMemcpy( (stlChar*)sResultValue->mValue + sResultValue->mLength,
                   DTF_VARCHAR(sValue2),
                   sValue2->mLength );
        sResultValue->mLength += sValue2->mLength;
    }
    
    /*----------------------------------------------------------------------------
     *   * FOR가 지정된 경우
     *     SUBSTRING( str1 FROM 1 FOR (start_position - 1) )
     *     || str2
     *     || SUBSTRING( str1 FROM (start_position + string_length )      <=== 이 부분에 대한 수행.
     * 
     *   * FOR가 생략된 경우
     *      SUBSTRING( str1 FROM 1 FOR (start_position - 1) )
     *      || str2
     *      || SUBSTRING( str1 FROM (start_position + CHAR_LENGTH(str2) ) <=== 이 부분에 대한 수행.
     *----------------------------------------------------------------------------*/    

    if( (sValue1->mLength == 0)
        || (sFromLength == STL_INT64_MAX)
        || ((aInputArgumentCnt == 3) && (sValue2->mLength == 0)) )
    {
        /*
         *  1. (sValue1->mLength == 0)
         *     SUBSTRING( NULL FROM (start_position + string_length )
         *  2. (sFromLength == STL_INT64_MAX)   
         *     SUBSTRING( str1 FROM (NULL + string_length )
         *  3. ((aInputArgumentCnt == 3) && (sValue2->mLength == 0))
         *     FOR가 생략된 경우로,
         *     SUBSTRING( str1 FROM (start_position + CHAR_LENGTH(NULL) )
         *
         * 인 경우로 이 부분의 결과는 NULL이다.
         */ 
        STL_THROW( SUCCESS_END );
    }
    else
    {
        if( aInputArgumentCnt == 4 )
        {
            /*
             *  4. ((aInputArgumentCnt == 4) && (sValue4->mLength == 0))
             *     FOR가 지정되었고 FOR의 value가 NULL인 경우로,
             *     SUBSTRING( str1 FROM (start_position + NULL )
             *
             * 인 경우로 이 부분의 결과는 NULL이다.
             */
            
            sValue4 = aInputArgument[3].mValue.mDataValue;
            
            if( sValue4->mLength == 0 )
            {
                STL_THROW( SUCCESS_END );
            }
            else
            {
                // Do Nothing
            }
        }
        else
        {
            // Do Nothing
        }        
    }
    
    /*
     * For length 정보 설정
     */
    if( aInputArgumentCnt == 3 )
    {
        /**
         * FOR phase 가 생략된 경우
         * start_position + CHAR_LENGTH(str2)
         */
        
        if( sCalculateValue2CharLength == STL_TRUE )
        {
            // Do Nothing
        }
        else
        {
            /*
             * CHAR_LENGTH(sValue2)를 구한다.
             */
            STL_TRY( dtsGetMbstrlenWithLenFuncList[ sCharSetID ]( DTF_VARCHAR(sValue2),
                                                                  sValue2->mLength,
                                                                  & sValue2CharLength,
                                                                  aVectorFunc,
                                                                  aVectorArg,
                                                                  sErrorStack )
                     == STL_SUCCESS );
            
            sCalculateValue2CharLength = STL_TRUE;
        }
        
        sStartPosition = sFromLength + sValue2CharLength;
    }
    else
    {
        /*
         * FOR phase가 지정된 경우 
         * start_position + string_length
         */
        sStartPosition = sFromLength + DTF_INTEGER(sValue4);
    }
    
    if( sStartPosition == 0 )
    {
        sStartPosition = 1;
    }
    else
    {
        // Do Nothing
    }

    STL_TRY( dtfMbPosition2BinaryPosition( sCharSetID,
                                           DTF_VARCHAR(sValue1),
                                           sValue1->mLength,
                                           sStartPosition,
                                           & sByteStartPosition,
                                           & sIsOver,
                                           aVectorFunc,
                                           aVectorArg,
                                           sErrorStack )
             == STL_SUCCESS );
    
    if( sIsOver == STL_TRUE )
    {
        // Do Nothing
    }
    else
    {
        /* sValue1의 복사해야 할 나머지 부분의 byte 길이 */
        sByteStringLength = sValue1->mLength - sByteStartPosition + 1;        
        
        /*
         * overflow check
         */
        
        if( (sResultValue->mLength + sByteStringLength) <= DTL_VARCHAR_MAX_PRECISION )
        {
            // Do Nothing
        }
        else
        {
            if( sCalculateValue2CharLength == STL_TRUE )
            {
                // Do Nothing
            }
            else
            {
                /*
                 * CHAR_LENGTH(sValue2)를 구한다.
                 */
                STL_TRY( dtsGetMbstrlenWithLenFuncList[ sCharSetID ]( DTF_VARCHAR(sValue2),
                                                                      sValue2->mLength,
                                                                      & sValue2CharLength,
                                                                      aVectorFunc,
                                                                      aVectorArg,
                                                                      sErrorStack )
                         == STL_SUCCESS );
                
                sCalculateValue2CharLength = STL_TRUE;
                sCheckResultPrecision += sValue2CharLength;
            }

            /* sValue1의 복사될 나머지 부분에 대한 char_length를 구한다. */
            STL_TRY( dtsGetMbstrlenWithLenFuncList[ sCharSetID ]( DTF_VARCHAR(sValue1),
                                                                  sByteStringLength,
                                                                  & sCharLength,
                                                                  aVectorFunc,
                                                                  aVectorArg,
                                                                  sErrorStack )
                     == STL_SUCCESS );
            
            sCheckResultPrecision += sCharLength;
            STL_TRY_THROW( sCheckResultPrecision <= DTL_VARCHAR_MAX_PRECISION,
                           ERROR_OUT_OF_PRECISION );
        }

        /* 복사 */
        STL_TRY( dtfBinaryStringSubstringInternal(
                     DTF_VARCHAR(sValue1),
                     sValue1->mLength,
                     sByteStartPosition,
                     sByteStringLength,
                     DTF_VARCHAR(sResultValue) + sResultValue->mLength,
                     & sResultLength,
                     sErrorStack )
                 == STL_SUCCESS );
        sResultValue->mLength += sResultLength;
    }

    STL_RAMP( SUCCESS_END );

    STL_DASSERT( sResultValue->mLength <= sResultValue->mBufferSize );
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_PRECISION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_CHARACTER_LENGTH_GREATER_THAN_COLUMN_LENGTH,
                      NULL,
                      sErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief longvarchar overlay(string placing string from int [for int])
 *        <BR> string의 substring의 치환
 *        <BR> overlay(string placing string from int [for int] [using char_length_unit] )
 *        <BR>   overlay(string placing string from int [for int] [using char_length_unit]) => [ P, S ]
 *        <BR> ex) overlay('Tdtlxas' placing 'hom' from 2 for 4)  =>  Thomas
 *        <BR> ex) overlay('Tdtlxas' placing 'hom' from 0 for 4)  =>  homxas
 *        <BR> ex) overlay('Tdtlxas' placing 'hom' from -2 for 4)  =>  Tdtlxhom
 *        <BR> ex) overlay('Tdtlxas' placing 'hom' from 12 for 4)  =>  Tdtlxashom
 *        <BR> ex) overlay('Tdtlxas' placing 'hom' from -12 for 4)  =>  hom
 *        <BR> ex) overlay('Tdtlxas' placing 'hom' from 2 for -4)  =>  Thomas
 *        <BR> ex) overlay('Tdtlxas' placing null from 2 for 4)  =>  Tas
 *        <BR> ex) overlay(null placing 'hom' from 2 for 4)  =>  hom
 *   
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> overlay(LONGVARCHAR, LONGVARCHAR, INTEGER, INTEGER, INTEGER)
 *                               <BR> overlay(string, placing string, from , for, using )
 * @param[out] aResultValue      연산 결과 (결과타입 LONGVARCHAR)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfLongvarcharStringOverlay( stlUInt16        aInputArgumentCnt,
                                       dtlValueEntry  * aInputArgument,
                                       void           * aResultValue,
                                       dtlFuncVector  * aVectorFunc,
                                       void           * aVectorArg,
                                       void           * aEnv )
{
    /*
     * 표준에 다음과 같이 기술되어 있다. (우선 using절 생략하고 ...)
     *
     * 구문 : OVERLAY( str1 PLACING str2 FROM start_position [ FOR string_length ] ) 
     *                 ----         ----      --------------       -------------
     * 
     * OVERLAY는 다음과 같은 결과이다.
     *
     *   * FOR가 지정된 경우
     *     SUBSTRING( str1 FROM 1 FOR (start_position - 1) )
     *     || str2
     *     || SUBSTRING( str1 FROM (start_position + string_length )
     * 
     *   * FOR가 생략된 경우
     *      SUBSTRING( str1 FROM 1 FOR (start_position - 1) )
     *      || str2
     *      || SUBSTRING( str1 FROM (start_position + CHAR_LENGTH(str2) )
     *
     * <참고> 
     * substring함수와 ||함수는 postgres가 표준을 따르고 있으나,
     * oracle 결과를 따라가는 정책으로 정해짐.
     * 참조: substring, || 함수 주석.
     */
        
    /*
     * postgres의 결과는 아래와 같다.
     * select overlay('abcdefghi' placing 'dtlx' from -2 for 4) from dual;
     * postgres> ERROR:  negative substring length not allowed
     *
     * => length는 4인고 position이 -2인데 error message가 이상함.
     * 소스를 봐도 position이 음수면 'negaitve length' 메시지가 나오도록 되어 있음.
     *
     * gliese는 dtfStringSubstring 정책을 따라간다.
     * dtfStringSubstring에서는 oracle과 동일하게 처리함으로 error 가 아니다.
     * 위의 ex 참고
     */
    

    dtlDataValue  * sValue1;
    dtlDataValue  * sValue2;
    dtlDataValue  * sValue3;
    dtlDataValue  * sValue4;
//  dtlDataValue  * sValue5;  /* asign만 하고 사용하지 않는 변수 주석처리 */
    dtlDataValue  * sResultValue;
    stlInt64        sResultLength;
    stlErrorStack * sErrorStack;
    stlInt64        sFromLength = 0;
    stlInt64        sStartPosition = 0;
    stlInt64        sByteStartPosition = 0;
    stlInt64        sStringLength = 0;
    stlInt64        sByteStringLength = 0;
    stlInt64        sValue2CharLength = 0;
    stlInt64        sCheckResultPrecision = 0;
    stlBool         sCalculateValue2CharLength = STL_FALSE;
    stlBool         sIsOver = STL_FALSE;
    dtlCharacterSet sCharSetID = DTL_CHARACTERSET_MAX;

    /**
     * @todo USING절에 대한 점검이 필요해서
     *  <BR> 우선 parsing 단계에서 USING절은 파싱되지 않도록 막음. 
     */
    DTL_PARAM_VALIDATE( (aInputArgumentCnt >= 3) && (aInputArgumentCnt <=  5),
                        (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sValue3 = aInputArgument[2].mValue.mDataValue;
    sValue4 = NULL;
//  sValue5 = aInputArgument[4].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    /*
     * input argument validate는 생략한다.
     * sValue1(str1), sValue2(str2), sValue3( FROM ) 에는 NULL이 올 수 있다.
     * sValue4( FOR )는 NULL이 올 수 있고, 생략될 수 있다.
     * sValue5( USING )는 생략될 수 있다.
     */
        
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue,  DTL_TYPE_LONGVARCHAR, sErrorStack );

    /*----------------------------------------------------------------------------
     * OVERLAY( str1 PLACING str2 FROM start_position [ FOR string_length ] ) 에서
     * 
     * 1. (str1 == NULL) && (str2 == NULL) 이면 결과는 NULL이다.
     *       SUBSTRING( NULL FROM 1 FOR (start_position - 1) )      <-- NULL
     *    || NULL                                                   <-- NULL
     *    || SUBSTRING( NULL FROM (start_position + string_length ) <-- NULL
     * 
     * 2. (str2 == NULL) && (start_position == NULL) 이면 결과는 NULL이다.
     *       SUBSTRING( str1 FROM 1 FOR (NULL - 1) )      <-- NULL
     *    || NULL                                         <-- NULL
     *    || SUBSTRING( str1 FROM (NULL + string_length ) <-- NULL
     *----------------------------------------------------------------------------*/
    if( ( (sValue1->mLength == 0) && (sValue2->mLength == 0) ) ||
        ( (sValue2->mLength == 0) && (sValue3->mLength == 0) ) )
    {
        DTL_SET_NULL( sResultValue );

        STL_THROW( SUCCESS_END );
    }
    else
    {
        // Do Nothing
    }

    /*
     * From length 정보 설정
     */
    if( sValue3->mLength == 0 )
    {
        /* From length 가 NULL인 경우 */
        sFromLength = STL_INT64_MAX;
    }
    else
    {
        sFromLength = DTF_INTEGER(sValue3);
        if( sFromLength == 0 )
        {
            sFromLength = 1;
        }
        else
        {
            /* Do Nothing */
        }
    }

    /*----------------------------------------------------------------------------
     * SUBSTRING( str1 FROM 1 FOR (start_position - 1) )  <===== 이 부분에 대한 수행.
     *     || str2
     *     || SUBSTRING( str1 FROM (start_position + string_length )
     *----------------------------------------------------------------------------*/

    sResultValue->mLength = 0;
    sCharSetID = aVectorFunc->mGetCharSetIDFunc(aVectorArg);
    
    if( (sValue1->mLength == 0) ||
        (sFromLength == STL_INT64_MAX) )
    {
        /*
         * 1. (sValue1->mLength == 0) 
         *    SUBSTRING( NULL FROM 1 FOR (start_position - 1) )
         * 2. (sFromLength == STL_INT64_MAX)   
         *    SUBSTRING( str1 FROM 1 FOR (NULL - 1) )
         *    
         * 인 경우로, 이 부분의 수행 결과는 NULL
         */ 
        sStringLength = 0;
        sByteStringLength = 0;
    }
    else
    {
        sStringLength = sFromLength - 1;

        if( sStringLength <= 0 )
        {
            /*
             * SUBSTRING( str1 FROM 1 FOR (start_position - 1) )
             *                            --------------------
             * 에서 FOR value가 0 또는 음수인 경우로 이 부분의 수행 결과는 NULL                                
             */

            sByteStringLength = 0;
        }
        else
        {
            STL_TRY( dtfMbLength2BinaryLength( DTF_VARCHAR(sValue1),
                                               sValue1->mLength,
                                               sStringLength,
                                               & sByteStringLength,
                                               & sIsOver,
                                               aVectorFunc,
                                               aVectorArg,
                                               sErrorStack )
                     == STL_SUCCESS );

            /* 복사 */
            STL_TRY( dtfBinaryStringSubstringInternal( DTF_VARCHAR(sValue1),
                                                       sValue1->mLength,
                                                       1,
                                                       sByteStringLength,
                                                       DTF_VARCHAR(sResultValue),
                                                       & sResultValue->mLength,
                                                       sErrorStack )
                     == STL_SUCCESS );
        }
    }

    sCheckResultPrecision = sByteStringLength;
    
    /*----------------------------------------------------------------------------
     * SUBSTRING( str1 FROM 1 FOR (start_position - 1) )  
     *     || str2                                        <===== 이 부분에 대한 수행.
     *     || SUBSTRING( str1 FROM (start_position + string_length )
     *----------------------------------------------------------------------------*/

    if( sValue2->mLength == 0 )
    {
        /* str2 가 NULL 인  경우로 이 부분 수행결과는 NULL */
        // Do Nothing
    }
    else
    {
        /*
         * overflow check
         */

        sCheckResultPrecision += sValue2->mLength;
        STL_TRY_THROW( sCheckResultPrecision <= DTL_LONGVARCHAR_MAX_PRECISION,
                       ERROR_OUT_OF_PRECISION );

        /* str2 복사 */
        stlMemcpy( (stlChar*)sResultValue->mValue + sResultValue->mLength,
                   DTF_VARCHAR(sValue2),
                   sValue2->mLength );
        sResultValue->mLength += sValue2->mLength;
    }
    
    /*----------------------------------------------------------------------------
     *   * FOR가 지정된 경우
     *     SUBSTRING( str1 FROM 1 FOR (start_position - 1) )
     *     || str2
     *     || SUBSTRING( str1 FROM (start_position + string_length )      <=== 이 부분에 대한 수행.
     * 
     *   * FOR가 생략된 경우
     *      SUBSTRING( str1 FROM 1 FOR (start_position - 1) )
     *      || str2
     *      || SUBSTRING( str1 FROM (start_position + CHAR_LENGTH(str2) ) <=== 이 부분에 대한 수행.
     *----------------------------------------------------------------------------*/    

    if( (sValue1->mLength == 0)
        || (sFromLength == STL_INT64_MAX)
        || ((aInputArgumentCnt == 3) && (sValue2->mLength == 0)) )
    {
        /*
         *  1. (sValue1->mLength == 0)
         *     SUBSTRING( NULL FROM (start_position + string_length )
         *  2. (sFromLength == STL_INT64_MAX)   
         *     SUBSTRING( str1 FROM (NULL + string_length )
         *  3. ((aInputArgumentCnt == 3) && (sValue2->mLength == 0))
         *     FOR가 생략된 경우로,
         *     SUBSTRING( str1 FROM (start_position + CHAR_LENGTH(NULL) )
         *
         * 인 경우로 이 부분의 결과는 NULL이다.
         */ 
        STL_THROW( SUCCESS_END );
    }
    else
    {
        if( aInputArgumentCnt == 4 )
        {
            /*
             *  4. ((aInputArgumentCnt == 4) && (sValue4->mLength == 0))
             *     FOR가 지정되었고 FOR의 value가 NULL인 경우로,
             *     SUBSTRING( str1 FROM (start_position + NULL )
             *
             * 인 경우로 이 부분의 결과는 NULL이다.
             */
            
            sValue4 = aInputArgument[3].mValue.mDataValue;
            
            if( sValue4->mLength == 0 )
            {
                STL_THROW( SUCCESS_END );
            }
            else
            {
                // Do Nothing
            }
        }
        else
        {
            // Do Nothing
        }        
    }

    /*
     * For length 정보 설정
     */
    if( aInputArgumentCnt == 3 )
    {
        /**
         * FOR phase 가 생략된 경우
         * start_position + CHAR_LENGTH(str2)
         */

        if( sCalculateValue2CharLength == STL_TRUE )
        {
            // Do Nothing
        }
        else
        {
            /*
             * CHAR_LENGTH(sValue2)를 구한다.
             */
            STL_TRY( dtsGetMbstrlenWithLenFuncList[ sCharSetID ]( DTF_VARCHAR(sValue2),
                                                                  sValue2->mLength,
                                                                  & sValue2CharLength,
                                                                  aVectorFunc,
                                                                  aVectorArg,
                                                                  sErrorStack )
                     == STL_SUCCESS );
            
            sCalculateValue2CharLength = STL_TRUE;
        }
        
        sStartPosition = sFromLength + sValue2CharLength;
    }
    else
    {
        /*
         * FOR phase가 지정된 경우 
         * start_position + string_length
         */
        sStartPosition = sFromLength + DTF_INTEGER(sValue4);
    }

    if( sStartPosition == 0 )
    {
        sStartPosition = 1;
    }
    else
    {
        // Do Nothing
    }

    sCharSetID = aVectorFunc->mGetCharSetIDFunc(aVectorArg);

    STL_TRY( dtfMbPosition2BinaryPosition( sCharSetID,
                                           DTF_VARCHAR(sValue1),
                                           sValue1->mLength,
                                           sStartPosition,
                                           & sByteStartPosition,
                                           & sIsOver,
                                           aVectorFunc,
                                           aVectorArg,
                                           sErrorStack )
             == STL_SUCCESS );
    
    if( sIsOver == STL_TRUE )
    {
        // Do Nothing
    }
    else
    {
        /* sValue1의 복사해야 할 나머지 부분의 byte 길이 */
        sByteStringLength = sValue1->mLength - sByteStartPosition + 1;        
        
        /*
         * overflow check
         */
        
        sCheckResultPrecision += sByteStringLength;
        STL_TRY_THROW( sCheckResultPrecision <= DTL_LONGVARCHAR_MAX_PRECISION,
                       ERROR_OUT_OF_PRECISION );

        /* 복사 */
        STL_TRY( dtfBinaryStringSubstringInternal(
                     DTF_VARCHAR(sValue1),
                     sValue1->mLength,
                     sByteStartPosition,
                     sByteStringLength,
                     DTF_VARCHAR(sResultValue) + sResultValue->mLength,
                     & sResultLength,
                     sErrorStack )
                 == STL_SUCCESS );
        sResultValue->mLength += sResultLength;
    }
    
    STL_RAMP( SUCCESS_END );

    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_PRECISION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_STRING_CHARACTER_LENGTH_GREATER_THAN_COLUMN_LENGTH,
                      NULL,
                      sErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief overlay(string placing string from int [for int])
 *        <BR> binary string의 substring의 치환
 *        <BR> overlay(string placing string from int [for int])
 *        <BR>   overlay(string placing string from int [for int]) => [ P, S ]
 *        <BR> ex) overlay('Tdtlxas' placing 'hom' from 2 for 4)  =>  Thomas
 *        <BR> ex) overlay('Tdtlxas' placing 'hom' from 0 for 4)  =>  homxas
 *        <BR> ex) overlay('Tdtlxas' placing 'hom' from -2 for 4)  =>  Tdtlxhom
 *        <BR> ex) overlay('Tdtlxas' placing 'hom' from 12 for 4)  =>  Tdtlxashom
 *        <BR> ex) overlay('Tdtlxas' placing 'hom' from -12 for 4)  =>  hom
 *        <BR> ex) overlay('Tdtlxas' placing 'hom'xfrom 2 for -4)  =>  Thomas
 *        <BR> ex) overlay('Tdtlxas' placing null from 2 for 4)  =>  Tas
 *        <BR> ex) overlay(null placing 'hom' from 2 for 4)  =>  hom
 *   
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> overlay(VARBINARY, VARBINARY, INTEGER, INTEGER)
 *                               <BR> overlay(string, placing string, from, for)
 * @param[in]  aMaxPrecision     연산 결과의 Max Precision
 * @param[out] aResultValue      연산 결과 (결과타입 VARBINARY)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfBinaryStringOverlay( stlUInt16        aInputArgumentCnt,
                                  dtlValueEntry  * aInputArgument,
                                  stlInt64         aMaxPrecision,
                                  void           * aResultValue,
                                  void           * aEnv )
{
    /*
     * 표준에 다음과 같이 기술되어 있다. (우선 using절 생략하고 ...)
     *
     * 구문 : OVERLAY( str1 PLACING str2 FROM start_position [ FOR string_length ] ) 
     *                 ----         ----      --------------       -------------
     * 
     * OVERLAY는 다음과 같은 결과이다.
     *
     *   * FOR가 지정된 경우
     *     SUBSTRING( str1 FROM 1 FOR (start_position - 1) )
     *     || str2
     *     || SUBSTRING( str1 FROM (start_position + string_length )
     * 
     *   * FOR가 생략된 경우
     *      SUBSTRING( str1 FROM 1 FOR (start_position - 1) )
     *      || str2
     *      || SUBSTRING( str1 FROM (start_position + OCTET_LENGTH(str2) )
     *
     * <참고> 
     * substring함수와 ||함수는 postgres가 표준을 따르고 있으나,
     * oracle 결과를 따라가는 정책으로 정해짐.
     * 참조: substring, || 함수 주석.
     */
        
    /*
     * postgres의 결과는 아래와 같다.
     * select overlay('abcdefghi' placing 'dtlx' from -2 for 4) from dual;
     * postgres> ERROR:  negative substring length not allowed
     *
     * => length는 4인고 position이 -2인데 error message가 이상함.
     * 소스를 봐도 position이 음수면 'negaitve length' 메시지가 나오도록 되어 있음.
     *
     * gliese는 dtfStringSubstring 정책을 따라간다.
     * dtfStringSubstring에서는 oracle과 동일하게 처리함으로 error 가 아니다.
     * 위의 ex 참고
     */
    

    dtlDataValue  * sValue1;
    dtlDataValue  * sValue2;
    dtlDataValue  * sValue3;
    dtlDataValue  * sValue4;
//  dtlDataValue  * sValue5;  /* asign만 하고 사용하지 않는 변수 주석처리 */
    dtlDataValue  * sResultValue;
    stlInt64        sResultLength;
    stlErrorStack * sErrorStack;
    stlInt64        sFromLength = 0;
    stlInt64        sStartPosition = 0;
    stlInt64        sStringLength = 0;
    stlInt64        sCheckResultPrecision = 0;

    /**
     * @todo USING절에 대한 점검이 필요해서
     *  <BR> 우선 parsing 단계에서 USING절은 파싱되지 않도록 막음. 
     */
    DTL_PARAM_VALIDATE( (aInputArgumentCnt >= 3) && (aInputArgumentCnt <=  5),
                        (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sValue3 = aInputArgument[2].mValue.mDataValue;
    sValue4 = NULL;
//  sValue5 = aInputArgument[4].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    /*
     * input argument validate는 생략한다.
     * sValue1(str1), sValue2(str2), sValue3( FROM ) 에는 NULL이 올 수 있다.
     * sValue4( FOR )는 NULL이 올 수 있고, 생략될 수 있다.
     * binary string overlay에는 sValue5( USING )이 올 수 없다. 
     */
        
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue,
                           DTL_TYPE_VARBINARY,
                           DTL_TYPE_LONGVARBINARY,
                           sErrorStack );

    /*----------------------------------------------------------------------------
     * OVERLAY( str1 PLACING str2 FROM start_position [ FOR string_length ] ) 에서
     * 
     * 1. (str1 == NULL) && (str2 == NULL) 이면 결과는 NULL이다.
     *       SUBSTRING( NULL FROM 1 FOR (start_position - 1) )      <-- NULL
     *    || NULL                                                   <-- NULL
     *    || SUBSTRING( NULL FROM (start_position + string_length ) <-- NULL
     * 
     * 2. (str2 == NULL) && (start_position == NULL) 이면 결과는 NULL이다.
     *       SUBSTRING( str1 FROM 1 FOR (NULL - 1) )      <-- NULL
     *    || NULL                                         <-- NULL
     *    || SUBSTRING( str1 FROM (NULL + string_length ) <-- NULL
     *----------------------------------------------------------------------------*/
    if( ( (sValue1->mLength == 0) && (sValue2->mLength == 0) ) ||
        ( (sValue2->mLength == 0) && (sValue3->mLength == 0) ) )
    {
        DTL_SET_NULL( sResultValue );

        STL_THROW( SUCCESS_END );
    }
    else
    {
        // Do Nothing
    }

    /*
     * From length 정보 설정
     */
    if( sValue3->mLength == 0 )
    {
        /* From length 가 NULL인 경우 */
        sFromLength = STL_INT64_MAX;
    }
    else
    {
        sFromLength = DTF_INTEGER(sValue3);
        if( sFromLength == 0 )
        {
            sFromLength = 1;
        }
        else
        {
            /* Do Nothing */
        }
    }

    /*----------------------------------------------------------------------------
     * SUBSTRING( str1 FROM 1 FOR (start_position - 1) )  <===== 이 부분에 대한 수행.
     *     || str2
     *     || SUBSTRING( str1 FROM (start_position + string_length )
     *----------------------------------------------------------------------------*/

    sResultValue->mLength = 0;
    
    if( (sValue1->mLength == 0) ||
        (sFromLength == STL_INT64_MAX) )
    {
        /*
         * 1. (sValue1->mLength == 0) 
         *    SUBSTRING( NULL FROM 1 FOR (start_position - 1) )
         * 2. (sFromLength == STL_INT64_MAX)   
         *    SUBSTRING( str1 FROM 1 FOR (NULL - 1) )
         *    
         * 인 경우로, 이 부분의 수행 결과는 NULL
         */ 
        sStringLength = 0;
    }
    else
    {
        sStringLength = sFromLength - 1;

        if( sStringLength <= 0 )
        {
            /*
             * SUBSTRING( str1 FROM 1 FOR (start_position - 1) )
             *                            --------------------
             * 에서 FOR value가 0 또는 음수인 경우로 이 부분의 수행 결과는 NULL
             */ 
            
            sStringLength = 0;
        }
        else
        {
            /* 복사 */
            STL_TRY( dtfBinaryStringSubstringInternal( DTF_VARCHAR(sValue1),
                                                       sValue1->mLength,
                                                       1,
                                                       sStringLength,
                                                       DTF_VARCHAR(sResultValue),
                                                       & sResultValue->mLength,
                                                       sErrorStack )
                     == STL_SUCCESS );
        }
    }

    sCheckResultPrecision = sStringLength;
    
    /*----------------------------------------------------------------------------
     * SUBSTRING( str1 FROM 1 FOR (start_position - 1) )  
     *     || str2                                        <===== 이 부분에 대한 수행.
     *     || SUBSTRING( str1 FROM (start_position + string_length )
     *----------------------------------------------------------------------------*/

    if( sValue2->mLength == 0 )
    {
        /* str2 가 NULL 인  경우로 이 부분 수행결과는 NULL */
        // Do Nothing
    }
    else
    {
        /*
         * overflow check
         */
        sCheckResultPrecision += sValue2->mLength;

        STL_TRY_THROW( sCheckResultPrecision <= aMaxPrecision, ERROR_OUT_OF_PRECISION );
        
        /* str2 복사 */
        stlMemcpy( (stlChar*)sResultValue->mValue + sResultValue->mLength,
                   DTF_VARCHAR(sValue2),
                   sValue2->mLength );
        sResultValue->mLength += sValue2->mLength;
    }
    
    /*----------------------------------------------------------------------------
     *   * FOR가 지정된 경우
     *     SUBSTRING( str1 FROM 1 FOR (start_position - 1) )
     *     || str2
     *     || SUBSTRING( str1 FROM (start_position + string_length )      <=== 이 부분에 대한 수행.
     * 
     *   * FOR가 생략된 경우
     *      SUBSTRING( str1 FROM 1 FOR (start_position - 1) )
     *      || str2
     *      || SUBSTRING( str1 FROM (start_position + OCTET_LENGTH(str2) ) <=== 이 부분에 대한 수행.
     *----------------------------------------------------------------------------*/

    if( (sValue1->mLength == 0)
        || (sFromLength == STL_INT64_MAX)
        || ((aInputArgumentCnt == 3) && (sValue2->mLength == 0)) )
    {
        /*
         *  1. (sValue1->mLength == 0)
         *     SUBSTRING( NULL FROM (start_position + string_length )
         *  2. (sFromLength == STL_INT64_MAX)   
         *     SUBSTRING( str1 FROM (NULL + string_length )
         *  3. ((aInputArgumentCnt == 3) && (sValue2->mLength == 0))
         *     FOR가 생략된 경우로,
         *     SUBSTRING( str1 FROM (start_position + OCTET_LENGTH(NULL) )
         *
         * 인 경우로 이 부분의 결과는 NULL이다.
         */ 
        STL_THROW( SUCCESS_END );
    }
    else
    {
        if( aInputArgumentCnt == 4 )
        {
            /*
             *  4. ((aInputArgumentCnt == 4) && (sValue4->mLength == 0))
             *     FOR가 지정되었고 FOR의 value가 NULL인 경우로,
             *     SUBSTRING( str1 FROM (start_position + NULL )
             *
             * 인 경우로 이 부분의 결과는 NULL이다.
             */

            sValue4 = aInputArgument[3].mValue.mDataValue;
            
            if( sValue4->mLength == 0 )
            {
                STL_THROW( SUCCESS_END );
            }
            else
            {
                // Do Nothing
            }
        }
        else
        {
            // Do Nothing
        }        
    }
    
    /*
     * For length 정보 설정
     */
    if( aInputArgumentCnt == 3 )
    {
        /**
         * FOR phase 가 생략된 경우
         * start_position + OCTET_LENGTH(str2)
         */
        sStartPosition = sFromLength + sValue2->mLength;
    }
    else
    {
        /*
         * FOR phase가 지정된 경우 
         * start_position + string_length
         */
        sStartPosition = sFromLength + DTF_INTEGER(sValue4);
    }

    if( sStartPosition == 0 )
    {
        sStartPosition = 1;
    }
    else
    {
        // Do Nothing
    }

    /* sValue1의 복사해야 할 나머지 부분의 byte 길이 */
    sStringLength = sValue1->mLength - sStartPosition + 1;        
        
    /*
     * overflow check
     */
    sCheckResultPrecision += sStringLength;
    STL_TRY_THROW( sCheckResultPrecision <= aMaxPrecision, ERROR_OUT_OF_PRECISION );

    /* 복사 */
    STL_TRY( dtfBinaryStringSubstringInternal(
                 DTF_VARCHAR(sValue1),
                 sValue1->mLength,
                 sStartPosition,
                 sStringLength,
                 DTF_VARCHAR(sResultValue) + sResultValue->mLength,
                 & sResultLength,
                 sErrorStack )
             == STL_SUCCESS );
    sResultValue->mLength += sResultLength;

    STL_RAMP( SUCCESS_END );

    STL_DASSERT( sResultValue->mLength <= sResultValue->mBufferSize );

    return STL_SUCCESS;

    STL_CATCH( ERROR_OUT_OF_PRECISION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_BINARY_BYTE_LENGTH_GREATER_THAN_COLUMN_LENGTH,
                      NULL,
                      sErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/*******************************************************************************
 * position(substring in string) , strpos( string, substring )
 *   position(substring in string) => [ P, S, M ]
 *   strpos( string, substrin )    => [ P ]
 * ex) position('om' in 'Thomas')  =>  3
 *******************************************************************************/

/**
 * @brief position(substring in string)
 *        <BR> string의 특정한 substring의 위치를 반환
 *        <BR> position(substring in string) , strpos( string, substring )
 *        <BR>   position(substring in string) => [ P, S, M ]
 *        <BR>   strpos( string, substrin )    => [ P ]
 *        <BR> ex) position('om' in 'Thomas')  =>  3
 *        <BR> ex) position('omb' in 'Thomas')  =>  0
 *        <BR> ex) position('om' in null)  =>  null
 *        <BR> ex) position(null in 'Thomas')  =>  null
 *   
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> position(VARCHAR, VARCHAR)
 *                               <BR> position(string, substring)
 * @param[out] aResultValue      연산 결과 (결과타입 BIGINT)
 *                                  n: 1 base index
 *                                  0: not found
 *                                  null : 연산 불가능한 경우.
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfStringPosition( stlUInt16        aInputArgumentCnt,
                             dtlValueEntry  * aInputArgument,
                             void           * aResultValue,
                             dtlFuncVector  * aVectorFunc,
                             void           * aVectorArg,
                             void           * aEnv )
{
    /*
     * string이 null이면 or substring이  null이면
     * postgres> [   ]
     * mysql> NULL         : "NULL" 이라고 나옴.
     * gliese는 null로 나오도록 했음.
     *
     * substring이 ''이면 (string이 ''인것 포함.)
     * postgres>  1
     * mysql> 1
     * string이 ''이면 (substring이 ''이 아닌 경우임)
     * postgres>  0
     * mysql> 0
     *
     * gliese에서의 처리: ''에 대한 data type 없음.
     * => string또는 substring이 null이 오면 null로 리턴하도록 하였음.
     */

    dtlDataValue  * sValue1;
    dtlDataValue  * sValue2;
    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_GROUP2( sValue1,
                          DTL_GROUP_CHAR_STRING,
                          DTL_GROUP_LONGVARCHAR_STRING,
                          sErrorStack );
    DTF_CHECK_DTL_GROUP2( sValue2,
                          DTL_GROUP_CHAR_STRING,
                          DTL_GROUP_LONGVARCHAR_STRING,
                          sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_BIGINT, sErrorStack );

    /*
     * dtfBinaryStringPositionInternal 는 char 적용된 결과를 리턴한다.
     */
    STL_TRY( dtfCommonPositionInternal( sValue2,
                                        sValue1,
                                        STL_FALSE,  /* binary 여부 */
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
 * @brief position(substring in string)
 *   <BR> binary string의 특정한 substring의 위치를 반환
 *        <BR> position(substring in string) , strpos( string, substring )
 *        <BR>   position(substring in string) => [ P, S, M ]
 *        <BR>   strpos( string, substrin )    => [ P ]
 *        <BR> ex) position('om' in 'Thomas')  =>  3
 *        <BR> ex) position('om' in null)  =>  null
 *        <BR> ex) position(null in 'Thomas')  =>  null
 *   
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> position(VARBINARY, VARBINARY)
 *                               <BR> position(string, substring)
 * @param[out] aResultValue      연산 결과 (결과타입 BIGINT)
 *                                  n: 1 base index
 *                                  0: not found
 *                                  null : 연산 불가능한 경우.
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfBinaryStringPosition( stlUInt16        aInputArgumentCnt,
                                   dtlValueEntry  * aInputArgument,
                                   void           * aResultValue,
                                   dtlFuncVector  * aVectorFunc,
                                   void           * aVectorArg,
                                   void           * aEnv )
{
    /*
     * dtfStringPosition과 동일한 정책 적용
     */
    dtlDataValue  * sValue1;
    dtlDataValue  * sValue2;
    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_GROUP2( sValue1,
                          DTL_GROUP_BINARY_STRING,
                          DTL_GROUP_LONGVARBINARY_STRING,
                          sErrorStack );
    DTF_CHECK_DTL_GROUP2( sValue2,
                          DTL_GROUP_BINARY_STRING,
                          DTL_GROUP_LONGVARBINARY_STRING,
                          sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_NATIVE_BIGINT, sErrorStack );

    STL_TRY( dtfCommonPositionInternal( sValue2,
                                        sValue1,
                                        STL_TRUE,  /* binary 여부 */
                                        sResultValue,
                                        aVectorFunc,
                                        aVectorArg,
                                        sErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*******************************************************************************
 * substring(string from int [for int] [using char_length_unit]) ,
 * substr(string from int [for int] [using char_length_unit]) ,
 * substr( string, pos, len, char_length_unit)
 *   substring(string from int [for int] [using char_length_unit] ) => [ P, S, M ]
 *   substr( string, pos, len, char_length_unit )            => [ P, M, O ]
 * ex) substring('Thomas' from 2 for 3)  =>  hom
 *******************************************************************************/

/**
 * @brief substring(string from int [for int])
 *        <BR> string의 substring 추출
 *        <BR> substring(string from int [for int])
 *        <BR> substr(string from int [for int]) ,
 *        <BR> substr( string, pos, len)
 *        <BR>   substring(string from int [for int]) => [ P, S, M ]
 *        <BR>   substr( string, pos, len)            => [ P, M, O ]
 *        <BR> ex) substring('Thomas' from 2 for 3)   =>  hom
 *        <BR> ex) substring('Thomas' from -2 for 3)  =>  as
 *        <BR> ex) substring('Thomas' from 2 for -3)  =>  null
 *        <BR> ex) substring('Thomas' from 0 for 3)   =>  Tho
 *        <BR> ex) substring('Thomas' from -6 for 3)  =>  null
 *        <BR> ex) substring('Thomas' from 6 for 3)   =>  null
 *        <BR> ex) substring(null from 2 for 3)       =>  null
 *   
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> substring(CHAR|VARCHAR|LONGVARCHAR, INTEGER, INTEGER)
 *                               <BR> substring(string, from, for)
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR|LONGVARCHAR)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfStringSubstring( stlUInt16        aInputArgumentCnt,
                              dtlValueEntry  * aInputArgument,
                              void           * aResultValue,
                              dtlFuncVector  * aVectorFunc,
                              void           * aVectorArg,
                              void           * aEnv )
{
    /*
     * <참고> 
     * substring함수는 postgres가 표준을 따르고 있으나,
     * oracle 결과를 따라가는 정책으로 정해짐.
     *
     * <표준에는 다음과 같이 기술하고 있음>
     * SUBSTRING( str FROM start_position [ FOR string_length ] ........... )
     *            ---      --------------       -------------
     *             C         S                    L
     *
     *             LC = length(C)
     *
     *             E = S + L  : for절이 있는 경우
     *             E = max( LC + 1, S )
     *             
     *  a), b), c) 생략 ...
     *  d)  C, S, L 이 null value이면, 결과는 NULL
     *  e)  E < S => error
     *              <=== Postgres는 에러처리.
     *              <=== Gliese와 Oracle은 에러처리하지 않고, result로 NULL 리턴.
     *  f)  i)   S > LC 이거나 E < 1 이면, 결과는 zero-length string
     *      ii)  S1 = max( S, 1 ) <=== Postgres는 표준대로 ...
     *                            <=== Gliese와 Oracle은 C의 뒷부분에서부터 position의 위치를 찾는다.
     *           E1 = min( E, LC + 1 )
     *           L1 = E1 - S1
     *                <=== Postgres는 표준대로 ...
     */
    
    /*
     * select substr('abcdefghi', -2, 3) from dual;
     * oracle> hi       : gliese 적용
     * postgres> null
     * mysql> hi

     * select substr('abcdefghi', 2, -3) from dual;
     * oracle>          : gliese 적용
     * postgres> ERROR:  negative substring length not allowed
     * mysql>
     *
     * select substr('abcdefghi', 0, 3) from dual;
     * oracle> abc       : gliese 적용 (position이 0이면 1과 동일한 결과 나옴)
     * postgres> ab
     * mysql> [   ]
     *
     * select substr('abcdefghi', -10, 13) from dual;
     * oracle>  [   ]    : gliese 적용
     * postgres> ab
     * mysql>   [   ]
     *
     */

    dtlDataValue  * sValue1;
    dtlDataValue  * sValue2;
    dtlDataValue  * sValue3;
    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;
    stlInt64        sPosition;
    stlInt64        sLength;
    stlInt64        sForLength;
    stlBool         sIsOver;
    stlInt64        sTempPosition;
    stlChar       * sString;
    
    dtlCharacterSet sCharSetID = DTL_CHARACTERSET_MAX;

    /**
     * @todo USING절에 대한 점검이 필요해서
     *  <BR> 우선 parsing 단계에서 USING절은 파싱되지 않도록 막음. 
     */

    /**
     *  substring(string from int [for int] [using char_length_unit]) : Standard
     *
     *  substring(string from int [for int])
     *  
     * 1. aInputArgumentCnt == 2
     *    substring( string from int )
     *    
     * 2. aInputArgumentCnt == 3
     *    substring( string from int for int )
     */

    DTL_PARAM_VALIDATE( (aInputArgumentCnt >= 2) && (aInputArgumentCnt <= 3),
                        (stlErrorStack *)aEnv );

    /* character set이 추가될때 그에 따른 function list가 누락되는 일이 없도록 하기 위한 검사 코드 */
    DTF_CHECK_CHAR_SET_FUNC_LIST( dtfMbLength2BinaryLengthFuncList );    
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sValue3 = NULL;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_GROUP2( sValue1,
                          DTL_GROUP_CHAR_STRING,
                          DTL_GROUP_LONGVARCHAR_STRING,
                          sErrorStack );    
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_NATIVE_INTEGER, sErrorStack );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue,
                           DTL_TYPE_VARCHAR,
                           DTL_TYPE_LONGVARCHAR,
                           sErrorStack );

    /**
     * FOR string length 정보 설정
     */

    if( aInputArgumentCnt == 2 )
    {
        /**
         * FOR phase 가 생략된 경우
         * start_position으로부터 str의 마지막문자까지 리턴한다.
         */
        sForLength = STL_INT64_MAX;
    }
    else
    {
        sValue3 = aInputArgument[2].mValue.mDataValue;
        DTF_CHECK_DTL_VALUE( sValue3, DTL_TYPE_NATIVE_INTEGER, sErrorStack );
        
        sForLength = DTF_INTEGER( sValue3 );
        if ( sForLength <= 0 )
        {
            /*
             * string_length가 1보다 작은 경우, 결과값은 NULL이다.
             */                 
            DTL_SET_NULL( sResultValue );
                
            STL_THROW( SUCCESS_END );
        }
        else
        {
            /* Do Nothing */
        }
    }

    /**
     * Character set을 적용해야 함으로
     * position및 length를 character set적용하여 변경하고
     * BinaryString 함수로 처리한다.
     */
    
    if ( DTF_INTEGER( sValue2 ) == 0 )
    {
        /*
         * position이 0 인경우 1로 변경하여 처리한다.
         * oracle이 이렇게 하고 있음.
         */
        sTempPosition = 1;
    }
    else
    {
        sTempPosition = DTF_INTEGER( sValue2 );
    }

    sCharSetID = aVectorFunc->mGetCharSetIDFunc(aVectorArg);
    
    STL_TRY( dtfMbPosition2BinaryPosition( sCharSetID,
                                           DTF_VARCHAR( sValue1 ),
                                           sValue1->mLength,
                                           sTempPosition,
                                           &sPosition,
                                           &sIsOver,
                                           aVectorFunc,
                                           aVectorArg,
                                           sErrorStack )
             == STL_SUCCESS );

    /*
     * start_position > (str의 길이) 이면, 결과값은 NULL이다.
     *    예) substring( '봄여름'  from 5 )
     * (str의 길이 + start_position ) < 0 이면, 결과값은 NULL이다.
     *    예) substring( '봄여름' from  -5 )
     */    
    if ( sIsOver == STL_TRUE )
    {
        DTL_SET_NULL( sResultValue );

        STL_THROW( SUCCESS_END );
    }
    else
    {
        /* Do Nothing */
    }

    /**
     * FOR 의 Char length는 binary길이로 계산한다.
     */
    
    sString = DTF_VARCHAR( sValue1 ) + sPosition - 1;
    STL_TRY( dtfMbLength2BinaryLengthFuncList[ sCharSetID ]( sString,
                                                             sValue1->mLength - sPosition + 1,
                                                             sForLength,
                                                             &sLength,
                                                             &sIsOver,
                                                             aVectorFunc,
                                                             aVectorArg,
                                                             sErrorStack )
             == STL_SUCCESS );
    

    /*
     * binary length로 다 변경했음으로 binary함수 호출.
     */
    STL_TRY( dtfBinaryStringSubstringInternal( DTF_VARCHAR( sValue1 ),
                                               sValue1->mLength,
                                               sPosition,
                                               sLength,
                                               DTF_VARCHAR( sResultValue ),
                                               &sResultValue->mLength,
                                               sErrorStack )
             == STL_SUCCESS );

    STL_RAMP( SUCCESS_END );

    STL_DASSERT( sResultValue->mLength <= sResultValue->mBufferSize );    

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief substring(string from int [for int])
 *   <BR> binary string의 substring 추출
 *        <BR> substring(string from int [for int]) ,
 *        <BR> substr(string from int [for int]) ,
 *        <BR> substr( string, pos, len)
 *        <BR>   substring(string from int [for int]) => [ P, S, M ]
 *        <BR>   substr( string, pos, len)            => [ P, M, O ]
 *        <BR> ex) substring('Thomas' from 2 for 3)  =>  hom
 *        <BR> ex) substring('Thomas' from -2 for 3)  =>  as
 *        <BR> ex) substring('Thomas' from 2 for -3)  =>  null
 *        <BR> ex) substring('Thomas' from 0 for 3)  =>  Tho
 *        <BR> ex) substring('Thomas' from -6 for 3)  =>  null
 *        <BR> ex) substring('Thomas' from 6 for 3)  =>  null
 *        <BR> ex) substring(null from 2 for 3)  =>  null
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> substring(BINARY|VARBINARY, INTEGER, INTEGER)
 *                               <BR> substring(string, from, for)
 * @param[out] aResultValue      연산 결과 (결과타입 VARBINARY|LONGVARBINARY)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfBinaryStringSubstring( stlUInt16        aInputArgumentCnt,
                                    dtlValueEntry  * aInputArgument,
                                    void           * aResultValue,
                                    void           * aEnv )
{
    /*
     * dtfStringSubstring 과 동일한 정책 적용
     */
    dtlDataValue  * sValue1;
    dtlDataValue  * sPosition;
    dtlDataValue  * sLength;
    dtlDataValue  * sResultValue;

    stlInt64        sForLength;

    stlErrorStack * sErrorStack;

    /**
     *  substring(string from int [for int]) : Standard
     *  
     * 1. aInputArgumentCnt == 2
     *    substring( string from int )
     *    
     * 2. aInputArgumentCnt == 3
     *    substring( string from int for int )
     */

    DTL_PARAM_VALIDATE( (aInputArgumentCnt >= 2) && (aInputArgumentCnt <= 3),
                        (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sPosition = aInputArgument[1].mValue.mDataValue;
    sLength = NULL;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    DTF_CHECK_DTL_GROUP2( sValue1,
                          DTL_GROUP_BINARY_STRING,
                          DTL_GROUP_LONGVARBINARY_STRING,
                          sErrorStack );
    DTF_CHECK_DTL_VALUE( sPosition, DTL_TYPE_NATIVE_INTEGER, sErrorStack );
    
    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT2( sResultValue,
                           DTL_TYPE_VARBINARY,
                           DTL_TYPE_LONGVARBINARY,
                           sErrorStack );

    /**
     * Length 정보가 없는 경우 ( FOR phase 가 생략된 경우 )
     */
    
    if( aInputArgumentCnt == 2 )
    {
        /**
         * FOR phase 가 생략된 경우
         */
        sForLength = STL_INT64_MAX;
    }
    else
    {
        sLength = aInputArgument[2].mValue.mDataValue;
        DTF_CHECK_DTL_VALUE( sLength, DTL_TYPE_NATIVE_INTEGER, sErrorStack );
        
        sForLength = DTF_INTEGER( sLength );
        if ( sForLength <= 0 )
        {
            DTL_SET_NULL( sResultValue );
            
            STL_THROW( SUCCESS_END );
        }
        else
        {
            /* Do Nothing */
        }
    }
    
    STL_TRY( dtfBinaryStringSubstringInternal( DTF_VARBINARY( sValue1 ),
                                               sValue1->mLength,
                                               DTF_INTEGER( sPosition ),
                                               sForLength,
                                               DTF_VARBINARY( sResultValue ),
                                               &sResultValue->mLength,
                                               sErrorStack )
             == STL_SUCCESS );

    STL_RAMP( SUCCESS_END );

    STL_DASSERT( sResultValue->mLength <= sResultValue->mBufferSize );    

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/*******************************************************************************
 * upper(string)
 *   upper(string) => [ P, S, M, O ]
 * ex) upper('tom')  =>  TOM
 *******************************************************************************/

/**
 * @brief upper(string) 문자열을 대문자로 반환
 *        <BR> upper(string)
 *        <BR>   upper(string) => [ P, S, M, O ]
 *        <BR> ex) upper('tom')  =>  TOM
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> upper(CHAR or VARCHAR)
 * @param[out] aResultValue      연산 결과 (결과타입 CHAR or VARCHAR)
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfUpper( stlUInt16        aInputArgumentCnt,
                    dtlValueEntry  * aInputArgument,
                    void           * aResultValue,
                    dtlFuncVector  * aVectorFunc,
                    void           * aVectorArg,
                    void           * aEnv )
{
    dtlDataValue    * sValue1;
    dtlDataValue    * sResultValue;
    stlErrorStack   * sErrorStack;
    stlChar         * sInput;
    stlChar         * sResult;
    stlInt32          sGap;
    stlInt64          i;
    stlInt8           sLen;

    dtlCharacterSet   sCharSetID;

    /**
     * Parameter Validation
     */
    
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack = (stlErrorStack *)aEnv;

    /**
     * aInputArgument type은 DTL_TYPE_CHAR 또는 DTL_TYPE_VARCHAR 이어야 한다.
     * aResultValue type은 DTL_TYPE_CHAR 또는 DTL_TYPE_VARCHAR 이어야 한다.
     * 예) upper( DTL_TYPE_CHAR )    ==> result : DTL_TYPE_CHAR
     *     upper( DTL_TYPE_VARCHAR ) ==> result : DTL_TYPE_VARCHAR
     */
    DTF_CHECK_DTL_GROUP2( sValue1,
                          DTL_GROUP_CHAR_STRING,
                          DTL_GROUP_LONGVARCHAR_STRING,
                          sErrorStack );

    DTF_CHECK_DTL_RESULT3( sValue1,
                           DTL_TYPE_CHAR,
                           DTL_TYPE_VARCHAR,
                           DTL_TYPE_LONGVARCHAR,
                           sErrorStack );    
    
    sCharSetID = aVectorFunc->mGetCharSetIDFunc(aVectorArg);

    STL_DASSERT( sValue1->mLength <= sResultValue->mBufferSize );
    
    /* 대문자로 변경 한다. */
    sGap = 'A' - 'a';
    sInput = sValue1->mValue;
    sResult = sResultValue->mValue;
    i = 0;
    while( i < sValue1->mLength )
    {
        /*
         * 현재 character set의 1char의 길이를 구한다.
         * 1char이 1인 경우만 변경한다.
         * 현재는 ascii, utf8, ms949 뿐임으로 이 방식이 가능하다.
         * unicode등의 character가 추가되면 logic이 변경되어야 한다.
         */
        STL_TRY( dtsGetMbLengthFuncList[sCharSetID]( sInput,
                                                     &sLen,
                                                     sErrorStack )
                 == STL_SUCCESS );

        if ( sLen == 1)
        {
            if( ( *sInput >= 'a' ) && ( *sInput <= 'z' ) )
            {
                *sResult = *sInput + sGap;
            }
            else
            {
                *sResult = *sInput;
            }
            sInput++;
            sResult++;
            i++;
        }
        else
        {
            /*
             * 1char의 길이가 2이상임으로 다음 char로 넘어간다.
             */
            stlMemcpy( sResult,
                       sInput,
                       sLen );
            i += sLen;
            sInput += sLen;
            sResult += sLen;
        }
    }

    sResultValue->mLength = sValue1->mLength;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief binary substring(string from int [for int])을 stl 변수로 받아서 처리하는 함수.
 * @param[in]  aValue               data
 * @param[in]  aValueLength         data length
 * @param[in]  aPosition            position (1 base, 음수도 가능 )
 * @param[in]  aLength              substring의 length
 * @param[out] aResult              연산 결과 (결과타입 stlChar)
 * @param[out] aResultLen           연산 결과 (결과타입 stlInt64)
 * @param[out] aErrorStack          ErrorStack
 */
stlStatus dtfBinaryStringSubstringInternal( stlChar           * aValue,
                                            stlInt64            aValueLength,
                                            stlInt64            aPosition,
                                            stlInt64            aLength,
                                            stlChar           * aResult,
                                            stlInt64          * aResultLen,
                                            stlErrorStack     * aErrorStack )
{
    stlInt64        sCopyLen;
    stlInt64        sIndex; /* 복사 시작할 position : 0 based */

    /*
     * 복사되는 길이는 aLength 이나
     * 복사할 범위가 넘어가면 aLength보다 작아진다.
     * aLength만큼 복사하려면 strlen(aValue) >= aPosition + aLength - 1
     * => strlen(aValue) + 1 - sValue2 >= aLength
     * 그럼으로 MIN( aLength, strlen(sValue1) + 1 - aPosition )만큼 복사 가능하다.
     *
     * aPosition는 음수도 가능하다.
     * aPosition이 음수인 경우 오른쪽 부터 substring한다.
     * ex) substr('abcdefghi', -2, 3) => hi
     * aPosition이 0인 경우 1과 동일하게 처리한다.
     * aLength은 length임으로 0이상이어야 한다. (0은 가능)
     */

    DTL_PARAM_VALIDATE( aValueLength >= 0, aErrorStack );

    if ( aLength <= 0 )
    {
        /* length을 0으로 만들어 NULL로 설정한다. */
        *aResultLen = 0;

        STL_THROW( SUCCESS_END );
    }
    else
    {
        /* Do Nothing */
    }


    /*
     * sIndex는 실제 복사될 c언어의 index이다.
     * aPosition이 양수인 경우는 0부터 시작되도록 하고.
     * aPosition이 음수이면 오른쪽 부터 substring한다.
     * aPosition이 음수인 경우는 length에서 빼서 실제 index가 되도록 한다.
     *
     * 'abced' 에서 2 면 b부터 그러니 index는 1
     * 'abced' 에서 -2 면 e부터 그러니 index는 3
     * 'abced' 에서 1 면 a부터 그러니 index는 0
     * 'abced' 에서 0 면 a부터 그러니 index는 0 <= oracle이 이렇게 처리하고 있음.
     */
    if ( aPosition == 0 )
    {
        sIndex = 0;
    }
    else if ( aPosition > 0 )
    {
        sIndex = aPosition - 1;
    }
    else
    {
        sIndex = aValueLength + aPosition;
    }

    /*
     * input data범위를 넘어가면 null 리턴
     * aPosition이 음수인 경우도 index로 변환을 했음으로 같이 처리됨
     */
    if ( ( sIndex > aValueLength ) || ( sIndex < 0 ) )
    {
        *aResultLen = 0;
    }
    else
    {
        /*
         * 복사할 길이가 aValue보다 크면 최대 aValue 까지만 복사하도록 한다.
         */
        sCopyLen = STL_MIN( aLength, aValueLength - sIndex );
        stlMemcpy( aResult, aValue + sIndex, sCopyLen );
        *aResultLen = sCopyLen;
    }

    STL_RAMP( SUCCESS_END );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief position(substring in string)을 구하기 위한 내부 함수
 *        binary, string(character set적용) 모두 가능함. ( aIsBinary 로 구분 )
 *
 * @param[in]  aValue               varbinary data
 * @param[in]  aSubValue            substring data
 * @param[in]  aIsBinary            binary type여부
 * @param[out] aResultValue         연산 결과 (결과타입 dtlBigIntType)
 *                                  n: 1 base index
 *                                  0: not found
 *                                  null : 연산 불가능한 경우.
 * @param[in]  aVectorFunc          Function Vector
 * @param[in]  aVectorArg           Vector Argument
 * @param[out] aErrorStack          ErrorStack
 */
stlStatus dtfCommonPositionInternal( dtlDataValue     * aValue,
                                     dtlDataValue     * aSubValue,
                                     stlBool            aIsBinary,
                                     dtlDataValue     * aResultValue,
                                     dtlFuncVector    * aVectorFunc,
                                     void             * aVectorArg,
                                     stlErrorStack    * aErrorStack )
{
    /*
     * dtfStringPosition과 동일한 정책 적용
     */

    stlInt32        sCmpRtn;
    stlInt64        sIdx;
    stlInt64        sCharCnt;
    stlInt8         sLen;   /* 1char 의 length */

    dtlCharacterSet sCharSetID;

    /**
     * Parameter Validation
     */
    
/* DTL_TYPE_VARBINARY or DTL_TYPE_VARCHAR 두가지 type이 올수 있음으로 type검사 안함.
    DTF_CHECK_DTL_VALUE( aValue1, DTL_TYPE_VARBINARY, aErrorStack );
    DTF_CHECK_DTL_VALUE( aValue2, DTL_TYPE_VARBINARY, aErrorStack );
*/
    
    DTF_CHECK_DTL_RESULT( aResultValue, DTL_TYPE_NATIVE_BIGINT, aErrorStack );

    sCharSetID = aVectorFunc->mGetCharSetIDFunc(aVectorArg);
    
    STL_DASSERT( ( DTL_IS_NULL( aValue ) != STL_TRUE ) &&
                 ( DTL_IS_NULL( aSubValue ) != STL_TRUE ) );
    
    if( aValue->mLength < aSubValue->mLength )
    {
        /* sub string이 더 큼으로 not found */
        DTF_BIGINT( aResultValue ) = 0;
    }
    else
    {
        /* 일단 not found로 설정해 놓는다. */
        DTF_BIGINT( aResultValue ) = 0;

        /* sub string 찾는다. */
        sIdx = 0;
        sCharCnt = 0;
        while( sIdx < ( aValue->mLength - aSubValue->mLength + 1 ) )
        {
            /* 속도를 빠르게 하기 위해서 1byte만 먼저 비교한다. */
            if( DTF_VARBINARY( aValue )[sIdx] == DTF_VARBINARY( aSubValue )[0] )
            {
                sCmpRtn = stlMemcmp( DTF_VARBINARY( aValue ) + sIdx,
                                     DTF_VARBINARY( aSubValue ),
                                     aSubValue->mLength );
                if( sCmpRtn == 0 )
                {
                    /* found it.. */
                    /* C언어의 index를 몇번째로 변경해야 함으로 +1 한다. */
                    DTF_BIGINT( aResultValue ) = sCharCnt + 1;
                    break;
                }
                else
                {
                    /* not found.. continue.. */
                    /* Do Nothing */
                }

            }
            else
            {
                /* not found.. continue.. */
                /* Do Nothing */
            }

            /*
             * String type이면 Char 뒷byte에 ascii가 매치될수 있음으로
             * 1byte씩이 아닌 1char 단위로 skip 하면서 비교해야 함.
             */
            if ( aIsBinary == STL_FALSE )
            {
                /*
                 * character set에 따른 1 char에 대한 length를 구하고
                 * 그 만큼씩 넘어가면서 비교한다.
                 */
                STL_TRY( dtsGetMbLengthFuncList[sCharSetID]( DTF_VARBINARY( aValue ) + sIdx,
                                                             &sLen,
                                                             aErrorStack )
                         == STL_SUCCESS );
                sCharCnt++;
                sIdx += sLen;
            }
            else
            {
                /*
                 * binary type인 경우
                 */
                sCharCnt++;
                sIdx++;
            }
        }
    }
    aResultValue->mLength = DTL_NATIVE_BIGINT_SIZE;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
