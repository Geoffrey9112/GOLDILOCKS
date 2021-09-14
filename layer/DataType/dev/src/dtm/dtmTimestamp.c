/*******************************************************************************
 * dtmTimestamp.c
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
 * @file dtmTimestamp.c
 * @brief DataType Layer Timestamp type의 비교 연산
 */

#include <dtl.h>
#include <dtdDataType.h>
#include <dtdDef.h>

/**
 * @defgroup dtlTimestamp Timestamp Type간의 비교 연산
 * @ingroup dtlOperCompareType
 * @{
 *
 * @} dtlTimestamp */


/*******************************************************************************
 * TYPE : IS EQUAL
 ******************************************************************************/

/**
 * @addtogroup dtlTimestamp
 * @{
 */

/**
 * @brief Timestamp vs Date의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualTimestampToDate( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlTimestampType *)aInputArgument[0].mValue.mDataValue->mValue) ==
        *((dtlDateType *)aInputArgument[1].mValue.mDataValue->mValue);
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;    
}

/**
 * @brief Timestamp vs Timestamp의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualTimestampToTimestamp( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlTimestampType *)aInputArgument[0].mValue.mDataValue->mValue) ==
        *((dtlTimestampType *)aInputArgument[1].mValue.mDataValue->mValue);

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;    
}

/** @} dtlTimestamp */


/*******************************************************************************
 * TYPE : IS NOT EQUAL
 ******************************************************************************/

/**
 * @addtogroup dtlTimestamp
 * @{
 */

/**
 * @brief Timestamp vs Date의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualTimestampToDate( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlTimestampType *)aInputArgument[0].mValue.mDataValue->mValue) !=
        *((dtlDateType *)aInputArgument[1].mValue.mDataValue->mValue);

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;    
}


/**
 * @brief Timestamp vs Timestamp의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualTimestampToTimestamp( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlTimestampType *)aInputArgument[0].mValue.mDataValue->mValue) !=
        *((dtlTimestampType *)aInputArgument[1].mValue.mDataValue->mValue);

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;    
}

/** @} dtlTimestamp */


/*******************************************************************************
 * TYPE : IS LESS THAN
 ******************************************************************************/

/**
 * @addtogroup dtlTimestamp
 * @{
 */

/**
 * @brief Timestamp vs Date의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanTimestampToDate( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlTimestampType *)aInputArgument[0].mValue.mDataValue->mValue) <
        *((dtlDateType *)aInputArgument[1].mValue.mDataValue->mValue);

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Timestamp vs Timestamp의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanTimestampToTimestamp( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlTimestampType *)aInputArgument[0].mValue.mDataValue->mValue) <
        *((dtlTimestampType *)aInputArgument[1].mValue.mDataValue->mValue);

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/** @} dtlTimestamp */


/*******************************************************************************
 * TYPE : IS LESS THAN EQUAL
 ******************************************************************************/

/**
 * @addtogroup dtlTimestamp
 * @{
 */

/**
 * @brief Timestamp vs Date의 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualTimestampToDate( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlTimestampType *)aInputArgument[0].mValue.mDataValue->mValue) <=
        *((dtlDateType *)aInputArgument[1].mValue.mDataValue->mValue);

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/**
 * @brief Timestamp vs Timestamp의 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualTimestampToTimestamp( stlUInt16        aInputArgumentCnt,
                                                      dtlValueEntry  * aInputArgument,
                                                      void           * aResultValue,
                                                      void           * aInfo,
                                                      dtlFuncVector  * aVectorFunc,
                                                      void           * aVectorArg,
                                                      void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlTimestampType *)aInputArgument[0].mValue.mDataValue->mValue) <=
        *((dtlTimestampType *)aInputArgument[1].mValue.mDataValue->mValue);

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;
}

/** @} dtlTimestamp */


/*******************************************************************************
 * TYPE : IS GREATER THAN
 ******************************************************************************/

/**
 * @addtogroup dtlTimestamp
 * @{
 */

/**
 * @brief Timestamp vs Date의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanTimestampToDate( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlTimestampType *)aInputArgument[0].mValue.mDataValue->mValue) >
        *((dtlDateType *)aInputArgument[1].mValue.mDataValue->mValue);

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;    
}

/**
 * @brief Timestamp vs Timestamp의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanTimestampToTimestamp( stlUInt16        aInputArgumentCnt,
                                                    dtlValueEntry  * aInputArgument,
                                                    void           * aResultValue,
                                                    void           * aInfo,
                                                    dtlFuncVector  * aVectorFunc,
                                                    void           * aVectorArg,
                                                    void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlTimestampType *)aInputArgument[0].mValue.mDataValue->mValue) >
        *((dtlTimestampType *)aInputArgument[1].mValue.mDataValue->mValue);

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;    
}

/** @} dtlTimestamp */


/*******************************************************************************
 * TYPE : IS GREATER THAN EQUAL
 ******************************************************************************/

/**
 * @addtogroup dtlTimestamp
 * @{
 */

/**
 * @brief Timestamp vs Date의 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualTimestampToDate( stlUInt16        aInputArgumentCnt,
                                                    dtlValueEntry  * aInputArgument,
                                                    void           * aResultValue,
                                                    void           * aInfo,
                                                    dtlFuncVector  * aVectorFunc,
                                                    void           * aVectorArg,
                                                    void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlTimestampType *)aInputArgument[0].mValue.mDataValue->mValue) >=
        *((dtlDateType *)aInputArgument[1].mValue.mDataValue->mValue);
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;    
}

/**
 * @brief Timestamp vs Timestamp의 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualTimestampToTimestamp( stlUInt16        aInputArgumentCnt,
                                                         dtlValueEntry  * aInputArgument,
                                                         void           * aResultValue,
                                                         void           * aInfo,
                                                         dtlFuncVector  * aVectorFunc,
                                                         void           * aVectorArg,
                                                         void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlTimestampType *)aInputArgument[0].mValue.mDataValue->mValue) >=
        *((dtlTimestampType *)aInputArgument[1].mValue.mDataValue->mValue);
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;    
}

/** @} dtlTimestamp */




