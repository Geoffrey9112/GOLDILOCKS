/*******************************************************************************
 * dtmRowId.c
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
 * @file dtmRowId.c
 * @brief DataType Layer RowId type의 비교 연산
 */

#include <dtl.h>
#include <dtdDataType.h>
#include <dtdDef.h>


/**
 * @defgroup dtlRowId RowId Type간의 비교 연산
 * @ingroup dtlOperCompareType
 * @{
 *
 * @} dtlRowId */


/*******************************************************************************
 * TYPE : IS EQUAL
 ******************************************************************************/

/**
 * @addtogroup dtlRowId
 * @{
 */

/**
 * @brief RowId vs RowId의 IsEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsEqualRowIdToRowId( stlUInt16        aInputArgumentCnt,
                                      dtlValueEntry  * aInputArgument,
                                      void           * aResultValue,
                                      void           * aInfo,
                                      dtlFuncVector  * aVectorFunc,
                                      void           * aVectorArg,
                                      void           * aEnv )
{
    dtlRowIdType  * sLeftValue  = aInputArgument[0].mValue.mDataValue->mValue;
    dtlRowIdType  * sRightValue = aInputArgument[1].mValue.mDataValue->mValue;

    DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mDigits,
                                               sRightValue->mDigits,
                                               DTL_ROWID_SIZE )
                                    == 0 );

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;    
}

/** @} dtlRowId */


/*******************************************************************************
 * TYPE : IS NOT EQUAL
 ******************************************************************************/

/**
 * @addtogroup dtlRowId
 * @{
 */

/**
 * @brief RowId vs RowId의 IsNotEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsNotEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsNotEqualRowIdToRowId( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv )
{
    dtlRowIdType  * sLeftValue  = aInputArgument[0].mValue.mDataValue->mValue;
    dtlRowIdType  * sRightValue = aInputArgument[1].mValue.mDataValue->mValue;

    DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mDigits,
                                               sRightValue->mDigits,
                                               DTL_ROWID_SIZE )
                                    != 0 );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;    
}

/** @} dtlRowId */


/*******************************************************************************
 * TYPE : IS LESS THAN
 ******************************************************************************/

/**
 * @addtogroup dtlRowId
 * @{
 */

/**
 * @brief RowId vs RowId의 IsLessThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanRowIdToRowId( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv )
{
    dtlRowIdType  * sLeftValue  = aInputArgument[0].mValue.mDataValue->mValue;
    dtlRowIdType  * sRightValue = aInputArgument[1].mValue.mDataValue->mValue;

    DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mDigits,
                                               sRightValue->mDigits,
                                               DTL_ROWID_SIZE )
                                    < 0 );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;    
}

/** @} dtlRowId */


/*******************************************************************************
 * TYPE : IS LESS THAN EQUAL
 ******************************************************************************/

/**
 * @addtogroup dtlRowId
 * @{
 */

/**
 * @brief RowId vs RowId의 IsLessThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsLessThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsLessThanEqualRowIdToRowId( stlUInt16        aInputArgumentCnt,
                                              dtlValueEntry  * aInputArgument,
                                              void           * aResultValue,
                                              void           * aInfo,
                                              dtlFuncVector  * aVectorFunc,
                                              void           * aVectorArg,
                                              void           * aEnv )
{
    dtlRowIdType  * sLeftValue  = aInputArgument[0].mValue.mDataValue->mValue;
    dtlRowIdType  * sRightValue = aInputArgument[1].mValue.mDataValue->mValue;

    DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mDigits,
                                               sRightValue->mDigits,
                                               DTL_ROWID_SIZE )
                                    <= 0 );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;    
}

/** @} dtlRowId */


/*******************************************************************************
 * TYPE : IS GREATER THAN
 ******************************************************************************/

/**
 * @addtogroup dtlRowId
 * @{
 */

/**
 * @brief RowId vs RowId의 IsGreaterThan 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThan 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanRowIdToRowId( stlUInt16        aInputArgumentCnt,
                                            dtlValueEntry  * aInputArgument,
                                            void           * aResultValue,
                                            void           * aInfo,
                                            dtlFuncVector  * aVectorFunc,
                                            void           * aVectorArg,
                                            void           * aEnv )
{
    dtlRowIdType  * sLeftValue  = aInputArgument[0].mValue.mDataValue->mValue;
    dtlRowIdType  * sRightValue = aInputArgument[1].mValue.mDataValue->mValue;

    DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mDigits,
                                               sRightValue->mDigits,
                                               DTL_ROWID_SIZE )
                                    > 0 );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;    
}

/** @} dtlRowId */


/*******************************************************************************
 * TYPE : IS GREATER THAN EQUAL
 ******************************************************************************/

/**
 * @addtogroup dtlRowId
 * @{
 */

/**
 * @brief RowId vs RowId의 IsGreaterThanEqual 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      IsGreaterThanEqual 연산 결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtlOperIsGreaterThanEqualRowIdToRowId( stlUInt16        aInputArgumentCnt,
                                                 dtlValueEntry  * aInputArgument,
                                                 void           * aResultValue,
                                                 void           * aInfo,
                                                 dtlFuncVector  * aVectorFunc,
                                                 void           * aVectorArg,
                                                 void           * aEnv )
{
    dtlRowIdType  * sLeftValue  = aInputArgument[0].mValue.mDataValue->mValue;
    dtlRowIdType  * sRightValue = aInputArgument[1].mValue.mDataValue->mValue;

    DTL_BOOLEAN( aResultValue ) = ( stlMemcmp( sLeftValue->mDigits,
                                               sRightValue->mDigits,
                                               DTL_ROWID_SIZE )
                                    >= 0 );
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;    
}

/** @} dtlRowId */




