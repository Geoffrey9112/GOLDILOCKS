/*******************************************************************************
 * dtmTimestampTz.c
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
 * @file dtmTimestampTz.c
 * @brief DataType Layer TimestampTz type의 비교 연산
 */

#include <dtl.h>
#include <dtdDataType.h>
#include <dtdDef.h>

/**
 * @defgroup dtlTimestampTz TimestampTz Type간의 비교 연산
 * @ingroup dtlOperCompareType
 * @{
 *
 * @} dtlTimestampTz */


/*******************************************************************************
 * TYPE : IS EQUAL
 ******************************************************************************/

/**
 * @addtogroup dtlTimestampTz
 * @{
 */

/**
 * @brief TimestampTz vs TimestampTz의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualTimestampTzToTimestampTz( stlUInt16        aInputArgumentCnt,
                                                  dtlValueEntry  * aInputArgument,
                                                  void           * aResultValue,
                                                  void           * aInfo,
                                                  dtlFuncVector  * aVectorFunc,
                                                  void           * aVectorArg,
                                                  void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        ((dtlTimestampTzType *)aInputArgument[0].mValue.mDataValue->mValue)->mTimestamp ==
        ((dtlTimestampTzType *)aInputArgument[1].mValue.mDataValue->mValue)->mTimestamp;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;    
}

/** @} dtlTimestampTz */


/*******************************************************************************
 * TYPE : IS NOT EQUAL
 ******************************************************************************/

/**
 * @addtogroup dtlTimestampTz
 * @{
 */

/**
 * @brief TimestampTz vs TimestampTz의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualTimestampTzToTimestampTz( stlUInt16        aInputArgumentCnt,
                                                     dtlValueEntry  * aInputArgument,
                                                     void           * aResultValue,
                                                     void           * aInfo,
                                                     dtlFuncVector  * aVectorFunc,
                                                     void           * aVectorArg,
                                                     void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        ((dtlTimestampTzType *)aInputArgument[0].mValue.mDataValue->mValue)->mTimestamp !=
        ((dtlTimestampTzType *)aInputArgument[1].mValue.mDataValue->mValue)->mTimestamp;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;    
}

/** @} dtlTimestampTz */


/*******************************************************************************
 * TYPE : IS LESS THAN
 ******************************************************************************/

/**
 * @addtogroup dtlTimestampTz
 * @{
 */

/**
 * @brief TimestampTz vs TimestampTz의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanTimestampTzToTimestampTz( stlUInt16        aInputArgumentCnt,
                                                     dtlValueEntry  * aInputArgument,
                                                     void           * aResultValue,
                                                     void           * aInfo,
                                                     dtlFuncVector  * aVectorFunc,
                                                     void           * aVectorArg,
                                                     void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        ((dtlTimestampTzType *)aInputArgument[0].mValue.mDataValue->mValue)->mTimestamp <
        ((dtlTimestampTzType *)aInputArgument[1].mValue.mDataValue->mValue)->mTimestamp;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;    
}

/** @} dtlTimestampTz */


/*******************************************************************************
 * TYPE : IS LESS THAN EQUAL
 ******************************************************************************/

/**
 * @addtogroup dtlTimestampTz
 * @{
 */

/**
 * @brief TimestampTz vs TimestampTz의 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualTimestampTzToTimestampTz( stlUInt16        aInputArgumentCnt,
                                                          dtlValueEntry  * aInputArgument,
                                                          void           * aResultValue,
                                                          void           * aInfo,
                                                          dtlFuncVector  * aVectorFunc,
                                                          void           * aVectorArg,
                                                          void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        ((dtlTimestampTzType *)aInputArgument[0].mValue.mDataValue->mValue)->mTimestamp <=
        ((dtlTimestampTzType *)aInputArgument[1].mValue.mDataValue->mValue)->mTimestamp;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;    
}

/** @} dtlTimestampTz */


/*******************************************************************************
 * TYPE : IS GREATER THAN
 ******************************************************************************/

/**
 * @addtogroup dtlTimestampTz
 * @{
 */

/**
 * @brief TimestampTz vs TimestampTz의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanTimestampTzToTimestampTz( stlUInt16        aInputArgumentCnt,
                                                        dtlValueEntry  * aInputArgument,
                                                        void           * aResultValue,
                                                        void           * aInfo,
                                                        dtlFuncVector  * aVectorFunc,
                                                        void           * aVectorArg,
                                                        void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        ((dtlTimestampTzType *)aInputArgument[0].mValue.mDataValue->mValue)->mTimestamp >
        ((dtlTimestampTzType *)aInputArgument[1].mValue.mDataValue->mValue)->mTimestamp;

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;    
}

/** @} dtlTimestampTz */


/*******************************************************************************
 * TYPE : IS GREATER THAN EQUAL
 ******************************************************************************/

/**
 * @addtogroup dtlTimestampTz
 * @{
 */

/**
 * @brief TimestampTz vs TimestampTz의 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualTimestampTzToTimestampTz( stlUInt16        aInputArgumentCnt,
                                                             dtlValueEntry  * aInputArgument,
                                                             void           * aResultValue,
                                                             void           * aInfo,
                                                             dtlFuncVector  * aVectorFunc,
                                                             void           * aVectorArg,
                                                             void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        ((dtlTimestampTzType *)aInputArgument[0].mValue.mDataValue->mValue)->mTimestamp >=
        ((dtlTimestampTzType *)aInputArgument[1].mValue.mDataValue->mValue)->mTimestamp;
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;    
}

/** @} dtlTimestampTz */




