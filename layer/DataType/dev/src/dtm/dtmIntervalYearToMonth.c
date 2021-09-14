/*******************************************************************************
 * dtmIntervalYearToMonth.c
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
 * @file dtmIntervalYearToMonth.c
 * @brief DataType Layer dtlIntervalYearToMonth type의 비교 연산
 */

#include <dtl.h>
#include <dtdDataType.h>

/**
 * @defgroup dtlIntervalYearToMonth dtlIntervalYearToMonth 간의 비교 연산
 * @ingroup dtlOperCompareType
 * @{
 *
 * @} dtlIntervalYearToMonth */


/*******************************************************************************
 * TYPE : IS EQUAL
 ******************************************************************************/

/**
 * @addtogroup dtlIntervalYearToMonth
 * @{
 */

/**
 * @brief dtlIntervalYearToMonth vs dtlIntervalYearToMonth의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualIntervalYearToMonth( stlUInt16       aInputArgumentCnt,
                                             dtlValueEntry * aInputArgument,
                                             void          * aResultValue,
                                             void          * aInfo,
                                             dtlFuncVector * aVectorFunc,
                                             void          * aVectorArg,
                                             void          * aEnv )
{
    dtlIntervalYearToMonthType * sLeftValue =
        ((dtlIntervalYearToMonthType *)aInputArgument[0].mValue.mDataValue->mValue);
    dtlIntervalYearToMonthType * sRightValue =
        ((dtlIntervalYearToMonthType *)aInputArgument[1].mValue.mDataValue->mValue);

    DTL_BOOLEAN( aResultValue ) = (sLeftValue->mMonth == sRightValue->mMonth);

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;    
}

/** @} dtlIntervalYearToMonth */


/*******************************************************************************
 * TYPE : IS NOT EQUAL
 ******************************************************************************/

/**
 * @addtogroup dtlIntervalYearToMonth
 * @{
 */

/**
 * @brief dtlIntervalYearToMonth vs dtlIntervalYearToMonth의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualIntervalYearToMonth( stlUInt16       aInputArgumentCnt,
                                                dtlValueEntry * aInputArgument,
                                                void          * aResultValue,
                                                void          * aInfo,
                                                dtlFuncVector * aVectorFunc,
                                                void          * aVectorArg,
                                                void          * aEnv )
{
    dtlIntervalYearToMonthType * sLeftValue =
        ((dtlIntervalYearToMonthType *)aInputArgument[0].mValue.mDataValue->mValue);
    dtlIntervalYearToMonthType * sRightValue =
        ((dtlIntervalYearToMonthType *)aInputArgument[1].mValue.mDataValue->mValue);

    DTL_BOOLEAN( aResultValue ) = (sLeftValue->mMonth != sRightValue->mMonth);

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;    
}

/** @} dtlIntervalYearToMonth */


/*******************************************************************************
 * TYPE : IS LESS THAN
 ******************************************************************************/

/**
 * @addtogroup dtlIntervalYearToMonth
 * @{
 */

/**
 * @brief dtlIntervalYearToMonth vs dtlIntervalYearToMonth의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanIntervalYearToMonth( stlUInt16       aInputArgumentCnt,
                                                dtlValueEntry * aInputArgument,
                                                void          * aResultValue,
                                                void          * aInfo,
                                                dtlFuncVector * aVectorFunc,
                                                void          * aVectorArg,
                                                void          * aEnv )
{
    dtlIntervalYearToMonthType * sLeftValue =
        ((dtlIntervalYearToMonthType *)aInputArgument[0].mValue.mDataValue->mValue);
    dtlIntervalYearToMonthType * sRightValue =
        ((dtlIntervalYearToMonthType *)aInputArgument[1].mValue.mDataValue->mValue);
    
    DTL_BOOLEAN( aResultValue ) = (sLeftValue->mMonth < sRightValue->mMonth);

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;    
}

/** @} dtlIntervalYearToMonth */


/*******************************************************************************
 * TYPE : IS LESS THAN EQUAL
 ******************************************************************************/

/**
 * @addtogroup dtlIntervalYearToMonth
 * @{
 */

/**
 * @brief dtlIntervalYearToMonth vs dtlIntervalYearToMonth의 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                                     dtlValueEntry  * aInputArgument,
                                                     void           * aResultValue,
                                                     void           * aInfo,
                                                     dtlFuncVector  * aVectorFunc,
                                                     void           * aVectorArg,
                                                     void           * aEnv )
{
    dtlIntervalYearToMonthType * sLeftValue =
        ((dtlIntervalYearToMonthType *)aInputArgument[0].mValue.mDataValue->mValue);
    dtlIntervalYearToMonthType * sRightValue =
        ((dtlIntervalYearToMonthType *)aInputArgument[1].mValue.mDataValue->mValue);

    DTL_BOOLEAN( aResultValue ) = (sLeftValue->mMonth <= sRightValue->mMonth);

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;    
}

/** @} dtlIntervalYearToMonth */


/*******************************************************************************
 * TYPE : IS GREATER THAN
 ******************************************************************************/

/**
 * @addtogroup dtlIntervalYearToMonth
 * @{
 */

/**
 * @brief dtlIntervalYearToMonth vs dtlIntervalYearToMonth의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                                   dtlValueEntry  * aInputArgument,
                                                   void           * aResultValue,
                                                   void           * aInfo,
                                                   dtlFuncVector  * aVectorFunc,
                                                   void           * aVectorArg,
                                                   void           * aEnv )
{
    dtlIntervalYearToMonthType * sLeftValue =
        ((dtlIntervalYearToMonthType *)aInputArgument[0].mValue.mDataValue->mValue);
    dtlIntervalYearToMonthType * sRightValue =
        ((dtlIntervalYearToMonthType *)aInputArgument[1].mValue.mDataValue->mValue);

    DTL_BOOLEAN( aResultValue ) = (sLeftValue->mMonth > sRightValue->mMonth);

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;    
}

/** @} dtlIntervalYearToMonth */


/*******************************************************************************
 * TYPE : IS GREATER THAN EQUAL
 ******************************************************************************/

/**
 * @addtogroup dtlIntervalYearToMonth
 * @{
 */

/**
 * @brief dtlIntervalYearToMonth vs dtlIntervalYearToMonth의 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualIntervalYearToMonth( stlUInt16        aInputArgumentCnt,
                                                        dtlValueEntry  * aInputArgument,
                                                        void           * aResultValue,
                                                        void           * aInfo,
                                                        dtlFuncVector  * aVectorFunc,
                                                        void           * aVectorArg,
                                                        void           * aEnv )
{
    dtlIntervalYearToMonthType * sLeftValue =
        ((dtlIntervalYearToMonthType *)aInputArgument[0].mValue.mDataValue->mValue);
    dtlIntervalYearToMonthType * sRightValue =
        ((dtlIntervalYearToMonthType *)aInputArgument[1].mValue.mDataValue->mValue);

    DTL_BOOLEAN( aResultValue ) = (sLeftValue->mMonth >= sRightValue->mMonth);

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;    
}

/** @} dtlIntervalYearToMonth */




