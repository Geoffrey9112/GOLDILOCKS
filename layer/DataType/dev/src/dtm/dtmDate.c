/*******************************************************************************
 * dtmDate.c
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
 * @file dtmDate.c
 * @brief DataType Layer Date type의 비교 연산
 */

#include <dtl.h>
#include <dtdDataType.h>
#include <dtdDef.h>


/**
 * @defgroup dtlDate Date Type간의 비교 연산
 * @ingroup dtlOperCompareType
 * @{
 *
 * @} dtlDate */


/*******************************************************************************
 * TYPE : IS EQUAL
 ******************************************************************************/

/**
 * @addtogroup dtlDate
 * @{
 */

/**
 * @brief Date vs Date의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualDateToDate( stlUInt16        aInputArgumentCnt,
                                    dtlValueEntry  * aInputArgument,
                                    void           * aResultValue,
                                    void           * aInfo,
                                    dtlFuncVector  * aVectorFunc,
                                    void           * aVectorArg,
                                    void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
    *((dtlDateType *)aInputArgument[0].mValue.mDataValue->mValue) ==
        *((dtlDateType *)aInputArgument[1].mValue.mDataValue->mValue);

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;    
}

/**
 * @brief Date vs Timestamp의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualDateToTimestamp( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
    *((dtlDateType *)aInputArgument[0].mValue.mDataValue->mValue) ==
        *((dtlTimestampType *)aInputArgument[1].mValue.mDataValue->mValue);

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;    
}


/** @} dtlDate */


/*******************************************************************************
 * TYPE : IS NOT EQUAL
 ******************************************************************************/

/**
 * @addtogroup dtlDate
 * @{
 */

/**
 * @brief Date vs Date의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualDateToDate( stlUInt16        aInputArgumentCnt,
                                       dtlValueEntry  * aInputArgument,
                                       void           * aResultValue,
                                       void           * aInfo,
                                       dtlFuncVector  * aVectorFunc,
                                       void           * aVectorArg,
                                       void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlDateType *)aInputArgument[0].mValue.mDataValue->mValue) !=
        *((dtlDateType *)aInputArgument[1].mValue.mDataValue->mValue);

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;    
}

/**
 * @brief Date vs Timestamp의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualDateToTimestamp( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlDateType *)aInputArgument[0].mValue.mDataValue->mValue) !=
        *((dtlTimestampType *)aInputArgument[1].mValue.mDataValue->mValue);

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;    
}

/** @} dtlDate */


/*******************************************************************************
 * TYPE : IS LESS THAN
 ******************************************************************************/

/**
 * @addtogroup dtlDate
 * @{
 */

/**
 * @brief Date vs Date의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanDateToDate( stlUInt16        aInputArgumentCnt,
                                       dtlValueEntry  * aInputArgument,
                                       void           * aResultValue,
                                       void           * aInfo,
                                       dtlFuncVector  * aVectorFunc,
                                       void           * aVectorArg,
                                       void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlDateType *)aInputArgument[0].mValue.mDataValue->mValue) <
        *((dtlDateType *)aInputArgument[1].mValue.mDataValue->mValue);

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;    
}

/**
 * @brief Date vs Timestamp의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanDateToTimestamp( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlDateType *)aInputArgument[0].mValue.mDataValue->mValue) <
        *((dtlTimestampType *)aInputArgument[1].mValue.mDataValue->mValue);

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;    
}

/** @} dtlDate */


/*******************************************************************************
 * TYPE : IS LESS THAN EQUAL
 ******************************************************************************/

/**
 * @addtogroup dtlDate
 * @{
 */

/**
 * @brief Date vs Date의 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualDateToDate( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlDateType *)aInputArgument[0].mValue.mDataValue->mValue) <=
        *((dtlDateType *)aInputArgument[1].mValue.mDataValue->mValue);

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;    
}

/**
 * @brief Date vs Timestamp의 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualDateToTimestamp( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlDateType *)aInputArgument[0].mValue.mDataValue->mValue) <=
        *((dtlTimestampType *)aInputArgument[1].mValue.mDataValue->mValue);

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;    
}

/** @} dtlDate */


/*******************************************************************************
 * TYPE : IS GREATER THAN
 ******************************************************************************/

/**
 * @addtogroup dtlDate
 * @{
 */

/**
 * @brief Date vs Date의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanDateToDate( stlUInt16        aInputArgumentCnt,
                                          dtlValueEntry  * aInputArgument,
                                          void           * aResultValue,
                                          void           * aInfo,
                                          dtlFuncVector  * aVectorFunc,
                                          void           * aVectorArg,
                                          void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlDateType *)aInputArgument[0].mValue.mDataValue->mValue) >
        *((dtlDateType *)aInputArgument[1].mValue.mDataValue->mValue);

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;    
}


/**
 * @brief Date vs Timestamp의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanDateToTimestamp( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlDateType *)aInputArgument[0].mValue.mDataValue->mValue) >
        *((dtlTimestampType *)aInputArgument[1].mValue.mDataValue->mValue);

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;    
}

/** @} dtlDate */


/*******************************************************************************
 * TYPE : IS GREATER THAN EQUAL
 ******************************************************************************/

/**
 * @addtogroup dtlDate
 * @{
 */

/**
 * @brief Date vs Date의 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualDateToDate( stlUInt16        aInputArgumentCnt,
                                               dtlValueEntry  * aInputArgument,
                                               void           * aResultValue,
                                               void           * aInfo,
                                               dtlFuncVector  * aVectorFunc,
                                               void           * aVectorArg,
                                               void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlDateType *)aInputArgument[0].mValue.mDataValue->mValue) >=
        *((dtlDateType *)aInputArgument[1].mValue.mDataValue->mValue);

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;    
}

/**
 * @brief Date vs Timestamp의 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualDateToTimestamp( stlUInt16        aInputArgumentCnt,
                                                    dtlValueEntry  * aInputArgument,
                                                    void           * aResultValue,
                                                    void           * aInfo,
                                                    dtlFuncVector  * aVectorFunc,
                                                    void           * aVectorArg,
                                                    void           * aEnv )
{
    DTL_BOOLEAN( aResultValue ) =
        *((dtlDateType *)aInputArgument[0].mValue.mDataValue->mValue) >=
        *((dtlTimestampType *)aInputArgument[1].mValue.mDataValue->mValue);

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;    
}

/** @} dtlDate */




