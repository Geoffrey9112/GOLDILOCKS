/*******************************************************************************
 * elfCaseExpr.c
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
 * @file elfCaseExpr.c
 * @brief Case Expression Function Execution Library Layer
 */

#include <ell.h>

/**
 * @addtogroup elfCaseExpr Case Expression 
 * @ingroup elf
 * @internal
 * @{
 */

/*******************************************************************************
 * CASE WHEN condition THEN result [WHEN ...] [ELSE result] END
 *   CASE WHEN condition THEN result [WHEN ...] [ELSE result] END => [ P, S, M, O ]
 * ex) CASE WHEN a=1 THEN 'one'
 *     WHEN a=2 THEN 'two'
 *     ELSE 'other'
 *     END
 *     ==>
 *     1  one
 *     2  two
 *     3  other
 *******************************************************************************/

/**
 * @brief SIMPLE CASE
 *        <BR> CASE WHEN condition THEN result [WHEN ...] [ELSE result] END
 *        <BR>   CASE WHEN condition THEN result [WHEN ...] [ELSE result] END => [ P, S, M, O ]
 *        <BR> ex) CASE WHEN a=1 THEN 'one'
 *        <BR>    WHEN a=2 THEN 'two'
 *        <BR>    ELSE 'other'
 *        <BR>    END
 *        <BR>    ==>
 *        <BR>    1  one
 *        <BR>    2  two
 *        <BR>    3  other
 *        
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      연산 결과
 * @param[out] aEnv              environment
 */
stlStatus elfSimpleCase( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aEnv )
{
    
    return STL_SUCCESS;
}

/*******************************************************************************
 * CASE expression WHEN value THEN result [WHEN ...] [ELSE result] END
 *   CASE expression WHEN value THEN result [WHEN ...] [ELSE result] END => [ P, S, M ]
 * ex) CASE a WHEN 1 THEN 'one'
 *     WHEN 2 THEN 'two'
 *     ELSE 'other'
 *     END
 *     ==>
 *     1  one
 *     2  two
 *     3  other
 *******************************************************************************/

/**
 * @brief SEARCHED CASE
 *        <BR> CASE expression WHEN value THEN result [WHEN ...] [ELSE result] END
 *        <BR>   CASE expression WHEN value THEN result [WHEN ...] [ELSE result] END => [ P, S, M ]
 *        <BR> ex) CASE a WHEN 1 THEN 'one'
 *        <BR>     WHEN 2 THEN 'two'
 *        <BR>     ELSE 'other'
 *        <BR>     END
 *        <BR>     ==>
 *        <BR>     1  one
 *        <BR>     2  two
 *        <BR>     3  other
 *        
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      연산 결과
 * @param[out] aEnv              environment
 */
stlStatus elfSearchedCase( stlUInt16        aInputArgumentCnt,
                           dtlValueEntry  * aInputArgument,
                           void           * aResultValue,
                           void           * aEnv )
{
    
    return STL_SUCCESS;
}

/*******************************************************************************
 * COALESCE(value [, ...])
 *   COALESCE(value [, ...]) => [ P, S, O ]
 * ex) COALESCE( NULL, 1, 2, 3 )  =>  1
 * ex) COALESCE( description, short_description, '(none)') ...
 *******************************************************************************/

/**
 * @brief COALESCE(value [, ...])
 *        <BR> NULL이 아닌 최초의 값을 반환
 *        <BR> value가 모두 NULL인 경우, NULL 반환
 *        <BR>   COALESCE(value [, ...]) => [ P, S, O ]
 *        
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      연산 결과
 * @param[out] aEnv              environment
 */
stlStatus elfCoalesce( stlUInt16        aInputArgumentCnt,
                       dtlValueEntry  * aInputArgument,
                       void           * aResultValue,
                       void           * aEnv )
{
    
    return STL_SUCCESS;
}

/*******************************************************************************
 * NULLIF( value1, value2 )
 *   NULLIF( value1, value2 ) => [ P, S, M, O ]
 * ex) NULLIF( 1, 1 )  =>  NULL
 *******************************************************************************/

/**
 * @brief NULLIF( value1, value2 )
 *        <BR> value1과 value2가 같은 경우 NULL반환 
 *        <BR> 그 외의 경우 value1을 반환
 *        <BR>   NULLIF( value1, value2 ) => [ P, S, M, O ]
 *        
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      연산 결과
 * @param[out] aEnv              environment
 */
stlStatus elfNullIf( stlUInt16        aInputArgumentCnt,
                     dtlValueEntry  * aInputArgument,
                     void           * aResultValue,
                     void           * aEnv )
{
    
    return STL_SUCCESS;
}

/*******************************************************************************
 * GREATEST( value [, ...])
 *   GREATEST( value [, ...]) => [ P, O ]
 * ex) GREATEST( 1, 2, 5, 3, -1 )  =>  5
 *******************************************************************************/
/*******************************************************************************
 * LEAST( value [, ...])
 *   LEAST( value [, ...]) => [ P, O ]
 * ex) LEAST( 1, 2, 5, 3, -1 )  =>  -1
 *******************************************************************************/

/**
 * @brief GREATEST( value [, ...]) , LEAST( value [, ...])
 *
 *        <BR> GREATEST( value [, ...])
 *        <BR>   GREATEST( value [, ...]) => [ P, O ]
 *        <BR> ex) GREATEST( 1, 2, 5, 3, -1 )  =>  5
 *
 *        <BR> LEAST( value [, ...])
 *        <BR>   LEAST( value [, ...]) => [ P, O ]
 *        <BR> ex) LEAST( 1, 2, 5, 3, -1 )  =>  -1
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      연산 결과
 * @param[out] aEnv              environment
 */
stlStatus elfGreatestLeast( stlUInt16        aInputArgumentCnt,
                            dtlValueEntry  * aInputArgument,
                            void           * aResultValue,
                            void           * aEnv )
{
    return STL_SUCCESS;
}

/*******************************************************************************
 * NVL( value1, value2 )
 *   NVL( value1, value2 ) => [ P, O ]
 * ex) NVL( NULL, 'Not Applicable' )  => Not Applicable 
 *******************************************************************************/

/**
 * @brief NVL( value1, value2 )
 *        <BR> value1이 NULL인 경우, value2 반환
 *        <BR> value1이 NULL이 아닌 경우, value1 반환
 *        <BR>   NVL( value1, value2 ) => [ P, O ]
 *        <BR> ex) NVL( NULL, 'Not Applicable' )  => Not Applicable
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      연산 결과
 * @param[out] aEnv              environment
 */
stlStatus elfNvl( stlUInt16        aInputArgumentCnt,
                  dtlValueEntry  * aInputArgument,
                  void           * aResultValue,
                  void           * aEnv )
{
    
    return STL_SUCCESS;
}

/** @} */
