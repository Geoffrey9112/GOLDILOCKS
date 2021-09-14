/*******************************************************************************
 * dtmIntervalDayToSecond.c
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
 * @file dtmIntervalDayToSecond.c
 * @brief DataType Layer dtlIntervalDayToSecond type의 비교 연산
 */

#include <dtl.h>
#include <dtdDef.h>
#include <dtdDataType.h>

/**
 * @defgroup dtmIntervalDayToSecond dtlIntervalDayToSecond간의 비교 연산
 * @ingroup dtlOperCompareType
 * @{
 *
 * @} dtlIntervalDayToSecond */


/*******************************************************************************
 * TYPE : IS EQUAL
 ******************************************************************************/

/**
 * @addtogroup dtlIntervalDayToSecond
 * @{
 */

/**
 * @brief dtlIntervalDayToSecond vs dtlIntervalDayToSecond의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualIntervalDayToSecond( stlUInt16       aInputArgumentCnt,
                                             dtlValueEntry * aInputArgument,
                                             void          * aResultValue,
                                             void          * aInfo,
                                             dtlFuncVector * aVectorFunc,
                                             void          * aVectorArg,
                                             void          * aEnv )
{
    dtlIntervalDayToSecondType * sLeftValue =
        ((dtlIntervalDayToSecondType *)aInputArgument[0].mValue.mDataValue->mValue);
    dtlIntervalDayToSecondType * sRightValue =
        ((dtlIntervalDayToSecondType *)aInputArgument[1].mValue.mDataValue->mValue);

    dtlTimeOffset sLeftValueSpan = DTD_GET_INTERVAL_DAY_TO_SECOND_SPAN_VALUE(sLeftValue);
    dtlTimeOffset sRightValueSpan = DTD_GET_INTERVAL_DAY_TO_SECOND_SPAN_VALUE(sRightValue);

    DTL_BOOLEAN( aResultValue ) = (sLeftValueSpan == sRightValueSpan);

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;    
}

/** @} dtlIntervalDayToSecond */


/*******************************************************************************
 * TYPE : IS NOT EQUAL
 ******************************************************************************/

/**
 * @addtogroup dtlIntervalDayToSecond
 * @{
 */

/**
 * @brief dtlIntervalDayToSecond vs dtlIntervalDayToSecond의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualIntervalDayToSecond( stlUInt16       aInputArgumentCnt,
                                                dtlValueEntry * aInputArgument,
                                                void          * aResultValue,
                                                void          * aInfo,
                                                dtlFuncVector * aVectorFunc,
                                                void          * aVectorArg,
                                                void          * aEnv )
{
    dtlIntervalDayToSecondType * sLeftValue =
        ((dtlIntervalDayToSecondType *)aInputArgument[0].mValue.mDataValue->mValue);
    dtlIntervalDayToSecondType * sRightValue =
        ((dtlIntervalDayToSecondType *)aInputArgument[1].mValue.mDataValue->mValue);

    dtlTimeOffset sLeftValueSpan = DTD_GET_INTERVAL_DAY_TO_SECOND_SPAN_VALUE(sLeftValue);
    dtlTimeOffset sRightValueSpan = DTD_GET_INTERVAL_DAY_TO_SECOND_SPAN_VALUE(sRightValue);

    DTL_BOOLEAN( aResultValue ) = (sLeftValueSpan != sRightValueSpan);

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;    
}

/** @} dtlIntervalDayToSecond */


/*******************************************************************************
 * TYPE : IS LESS THAN
 ******************************************************************************/

/**
 * @addtogroup dtlIntervalDayToSecond
 * @{
 */

/**
 * @brief dtlIntervalDayToSecond vs dtlIntervalDayToSecond의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanIntervalDayToSecond( stlUInt16       aInputArgumentCnt,
                                                dtlValueEntry * aInputArgument,
                                                void          * aResultValue,
                                                void          * aInfo,
                                                dtlFuncVector * aVectorFunc,
                                                void          * aVectorArg,
                                                void          * aEnv )
{
    dtlIntervalDayToSecondType * sLeftValue =
        ((dtlIntervalDayToSecondType *)aInputArgument[0].mValue.mDataValue->mValue);
    dtlIntervalDayToSecondType * sRightValue =
        ((dtlIntervalDayToSecondType *)aInputArgument[1].mValue.mDataValue->mValue);

    dtlTimeOffset sLeftValueSpan = DTD_GET_INTERVAL_DAY_TO_SECOND_SPAN_VALUE(sLeftValue);
    dtlTimeOffset sRightValueSpan = DTD_GET_INTERVAL_DAY_TO_SECOND_SPAN_VALUE(sRightValue);

    DTL_BOOLEAN( aResultValue ) = (sLeftValueSpan < sRightValueSpan);    

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;    
}

/** @} dtlIntervalDayToSecond */


/*******************************************************************************
 * TYPE : IS LESS THAN EQUAL
 ******************************************************************************/

/**
 * @addtogroup dtlIntervalDayToSecond
 * @{
 */

/**
 * @brief dtlIntervalDayToSecond vs dtlIntervalDayToSecond의 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                                     dtlValueEntry  * aInputArgument,
                                                     void           * aResultValue,
                                                     void           * aInfo,
                                                     dtlFuncVector  * aVectorFunc,
                                                     void           * aVectorArg,
                                                     void           * aEnv )
{
    dtlIntervalDayToSecondType * sLeftValue =
        ((dtlIntervalDayToSecondType *)aInputArgument[0].mValue.mDataValue->mValue);
    dtlIntervalDayToSecondType * sRightValue =
        ((dtlIntervalDayToSecondType *)aInputArgument[1].mValue.mDataValue->mValue);

    dtlTimeOffset sLeftValueSpan = DTD_GET_INTERVAL_DAY_TO_SECOND_SPAN_VALUE(sLeftValue);
    dtlTimeOffset sRightValueSpan = DTD_GET_INTERVAL_DAY_TO_SECOND_SPAN_VALUE(sRightValue);

    DTL_BOOLEAN( aResultValue ) = (sLeftValueSpan <= sRightValueSpan);    

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;    
}

/** @} dtlIntervalDayToSecond */


/*******************************************************************************
 * TYPE : IS GREATER THAN
 ******************************************************************************/

/**
 * @addtogroup dtlIntervalDayToSecond
 * @{
 */

/**
 * @brief dtlIntervalDayToSecond vs dtlIntervalDayToSecond의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                                   dtlValueEntry  * aInputArgument,
                                                   void           * aResultValue,
                                                   void           * aInfo,
                                                   dtlFuncVector  * aVectorFunc,
                                                   void           * aVectorArg,
                                                   void           * aEnv )
{
    dtlIntervalDayToSecondType * sLeftValue =
        ((dtlIntervalDayToSecondType *)aInputArgument[0].mValue.mDataValue->mValue);
    dtlIntervalDayToSecondType * sRightValue =
        ((dtlIntervalDayToSecondType *)aInputArgument[1].mValue.mDataValue->mValue);

    dtlTimeOffset sLeftValueSpan = DTD_GET_INTERVAL_DAY_TO_SECOND_SPAN_VALUE(sLeftValue);
    dtlTimeOffset sRightValueSpan = DTD_GET_INTERVAL_DAY_TO_SECOND_SPAN_VALUE(sRightValue);
    
    DTL_BOOLEAN( aResultValue ) = (sLeftValueSpan > sRightValueSpan);

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;    
}

/** @} dtlIntervalDayToSecond */


/*******************************************************************************
 * TYPE : IS GREATER THAN EQUAL
 ******************************************************************************/

/**
 * @addtogroup dtlIntervalDayToSecond
 * @{
 */

/**
 * @brief dtlIntervalDayToSecond vs dtlIntervalDayToSecond의 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualIntervalDayToSecond( stlUInt16        aInputArgumentCnt,
                                                        dtlValueEntry  * aInputArgument,
                                                        void           * aResultValue,
                                                        void           * aInfo,
                                                        dtlFuncVector  * aVectorFunc,
                                                        void           * aVectorArg,
                                                        void           * aEnv )
{
    dtlIntervalDayToSecondType * sLeftValue =
        ((dtlIntervalDayToSecondType *)aInputArgument[0].mValue.mDataValue->mValue);
    dtlIntervalDayToSecondType * sRightValue =
        ((dtlIntervalDayToSecondType *)aInputArgument[1].mValue.mDataValue->mValue);

    dtlTimeOffset sLeftValueSpan = DTD_GET_INTERVAL_DAY_TO_SECOND_SPAN_VALUE(sLeftValue);
    dtlTimeOffset sRightValueSpan = DTD_GET_INTERVAL_DAY_TO_SECOND_SPAN_VALUE(sRightValue);

    DTL_BOOLEAN( aResultValue ) = (sLeftValueSpan >= sRightValueSpan);    

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;    
}

/** @} dtlIntervalDayToSecond */




