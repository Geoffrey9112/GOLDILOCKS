/*******************************************************************************
 * elfAggregate.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file elfAggregate.c
 * @brief elfAggregate Function Execution Library Layer
 */

#include <ell.h>

/**
 * @addtogroup elfAggregate  Aggregate
 * @ingroup elf
 * @internal
 * @{
 */

/*******************************************************************************
 * avg(expression)
 * avg => [ P, S, M, O ]
 ******************************************************************************/

/**
 * @brief avg(expression)를 수행하기 위해 avg대상들의 count와 sum을 구함.
 * @param[in]     aValue     expression
 * @param[in,out] aCount     avg대상 수
 * @param[in,out] aSumValue  avg대상들의 합(double)
 * @param[out] aErrorStack   에러 스택
 */
stlStatus elfDoubleAvgAccum( dtlDataValue   * aValue,
                             dtlDataValue   * aCount,
                             dtlDataValue   * aSumValue,
                             stlErrorStack  * aErrorStack )
{
    return STL_SUCCESS;
}

/**
 * @brief avg(expression)를 수행하기 위해 avg대상들의 count와 sum을 구함.
 * @param[in]     aValue     expression
 * @param[in,out] aCount     avg대상 수
 * @param[in,out] aSumValue  avg대상들의 합(numeric)
 * @param[out] aErrorStack   에러 스택
 */
stlStatus elfNumericAvgAccum( dtlDataValue   * aValue,
                              dtlDataValue   * aCount,
                              dtlDataValue   * aSumValue,
                              stlErrorStack  * aErrorStack )
{
    return STL_SUCCESS;
}

/**
 * @brief avg(expression)를 수행하기 위해 avg대상들의 count와 sum을 구함.
 * @param[in]     aValue     expression
 * @param[in,out] aCount     avg대상 수
 * @param[in,out] aSumValue  avg대상들의 합(interval)
 * @param[out] aErrorStack   에러 스택
 */
stlStatus elfIntervalAvgAccum( dtlDataValue   * aValue,
                               dtlDataValue   * aCount,
                               dtlDataValue   * aSumValue,
                               stlErrorStack  * aErrorStack )
{
    return STL_SUCCESS;
}

/**
 * @brief avg(expression)
 * @param[in]  aCount          avg대상 수
 * @param[in]  aSumValue       avg대상들의 합
 * @param[out] aAvgResultValue 최종 avg 연산 결과(double)
 * @param[out] aErrorStack     에러 스택
 */
stlStatus elfDoubleAvg( dtlDataValue   * aCount,
                        dtlDataValue   * aSumValue,
                        dtlDataValue   * aAvgResultValue,
                        stlErrorStack  * aErrorStack )
{
    return STL_SUCCESS;
}

/**
 * @brief avg(expression)
 * @param[in]  aCount          avg대상 수
 * @param[in]  aSumValue       avg대상들의 합
 * @param[out] aAvgResultValue 최종 avg 연산 결과(numeric)
 * @param[out] aErrorStack     에러 스택
 */
stlStatus elfNumericAvg( dtlDataValue   * aCount,
                         dtlDataValue   * aSumValue,
                         dtlDataValue   * aAvgResultValue,
                         stlErrorStack  * aErrorStack )
{
    return STL_SUCCESS;
}

/**
 * @brief avg(expression)
 * @param[in]  aCount          avg대상 수
 * @param[in]  aSumValue       avg대상들의 합
 * @param[out] aAvgResultValue 최종 avg 연산 결과(interval)
 * @param[out] aErrorStack     에러 스택
 */
stlStatus elfIntervalAvg( dtlDataValue   * aCount,
                          dtlDataValue   * aSumValue,
                          dtlDataValue   * aAvgResultValue,
                          stlErrorStack  * aErrorStack )
{
    return STL_SUCCESS;
}

/*******************************************************************************
 * bit_and(expression)
 *   bit_and(expression) => [ P, M ]
 * 굳이 안 만들어도 elfBitwiseAnd함수 이용해서 
 ******************************************************************************/


/*******************************************************************************
 * bit_or(expression)
 *   bit_or(expression) => [ P, M ]
 * 굳이 안 만들어도 elfBitwiseOr함수 이용해서 
 ******************************************************************************/


/*******************************************************************************
 * bool_and(expression)
 *   bool_and(expression) => [ P ]
 ******************************************************************************/

/**
 * @brief bool_and(expression)
 * @param[in]     aValue       aValue(boolean)
 * @param[in,out] aResultValue 연산 결과(boolean)
 * @param[out]    aErrorStack  에러 스택
 */
stlStatus elfBooleanAnd( dtlDataValue   * aValue,
                         dtlDataValue   * aResultValue,
                         stlErrorStack  * aErrorStack )
{
    return STL_SUCCESS;
}

/*******************************************************************************
 * bool_or(expression)
 *   bool_or(expression) => [ P ]
 ******************************************************************************/

/**
 * @brief bool_or(expression)
 * @param[in]     aValue       aValue(boolean)
 * @param[in,out] aResultValue 연산 결과(boolean)
 * @param[out]    aErrorStack  에러 스택
 */
stlStatus elfBooleanOr( dtlDataValue   * aValue,
                        dtlDataValue   * aResultValue,
                        stlErrorStack  * aErrorStack )
{
    return STL_SUCCESS;
}

/*******************************************************************************
 * count(*)
 *   count(*) => [ P, S, M, O ]
 ******************************************************************************/

/**
 * @brief count(*)
 * @param[in,out] aResultValue 연산 결과(bigint)
 * @param[out]    aErrorStack  에러 스택
 */
stlStatus elfCountAsterisk(dtlDataValue   * aResultValue,
                           stlErrorStack  * aErrorStack )
{
    /*
     * SM에서 최적화 함수 제공????
     */ 
    return STL_SUCCESS;
}

/*******************************************************************************
 * count(expression)
 *   count(expression) => [ P, S, M, O ]
 ******************************************************************************/

/**
 * @brief count(expression)
 * @param[in,out] aResultValue 연산 결과(bigint)
 * @param[out]    aErrorStack  에러 스택
 */
stlStatus elfCount(dtlDataValue   * aResultValue,
                   stlErrorStack  * aErrorStack )
{
    /*
     * not null value count,
     * resultvalue ++ ;
     */ 
    return STL_SUCCESS;
}

/*******************************************************************************
 * every(expression)
 *   every(expression) => [ P, S ]
 * 모든 레코드의 평가가 true인 경우, true 반환   
 * elfBooleanAnd() 함수 이용 ???
 ******************************************************************************/


/*******************************************************************************
 * max(expression)
 *   max(expression) => [ P, S, M, O ]
 ******************************************************************************/

/**
 * @brief max(expression)
 * @param[in]     aValue       
 * @param[in,out] aResultValue 연산 결과(bigint)
 * @param[out]    aErrorStack  에러 스택
 */
stlStatus elfIntegerMax( dtlDataValue   * aValue,
                         dtlDataValue   * aResultValue,
                         stlErrorStack  * aErrorStack )
{
    /*
     * aresultevalue = avalue > aresultvalue ? avalue : aresultvalue
     */ 
    return STL_SUCCESS;
}

/**
 * @brief max(expression)
 * @param[in]     aValue       
 * @param[in,out] aResultValue 연산 결과(double)
 * @param[out]    aErrorStack  에러 스택
 */
stlStatus elfDoubleMax( dtlDataValue   * aValue,
                        dtlDataValue   * aResultValue,
                        stlErrorStack  * aErrorStack )
{
    /*
     * aresultevalue = avalue > aresultvalue ? avalue : aresultvalue
     */ 
    return STL_SUCCESS;
}

/**
 * @brief max(expression)
 * @param[in]     aValue       
 * @param[in,out] aResultValue 연산 결과(numeric)
 * @param[out]    aErrorStack  에러 스택
 */
stlStatus elfNumericMax( dtlDataValue   * aValue,
                         dtlDataValue   * aResultValue,
                         stlErrorStack  * aErrorStack )
{
    /*
     * aresultevalue = avalue > aresultvalue ? avalue : aresultvalue
     */ 
    return STL_SUCCESS;
}


/* date, time, timetz, timestamp, interval, string(char, varchar, longvarchar) */

/*******************************************************************************
 * min(expression)
 *   min(expression) => [ P, S, M, O ]
 ******************************************************************************/





/*******************************************************************************
 * string_agg(expression, delimiter)
 *   string_agg(expression, delimiter) => [ P ]
 ******************************************************************************/





/*******************************************************************************
 * sum( expression )   ?????
 *   sum(expression) => [ P, S, M, O ]
 *   
 * 굳이 안 만들어도 elfXXXAdd함수 이용해서
 ******************************************************************************/


/** @} */
