/*******************************************************************************
 * dtcFromBinary.c
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
 * @file dtcFromBinary.c
 * @brief DataType Layer Binary으로부터 다른 타입으로의 변환 
 */

#include <dtl.h>
#include <dtDef.h>

#include <dtcCast.h>

/**
 * @ingroup dtcFromBinary
 * @{
 */

/**
 * @brief Binary -> Binary 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastBinaryToBinary( stlUInt16        aInputArgumentCnt,
                                 dtlValueEntry  * aInputArgument,
                                 void           * aResultValue,
                                 void           * aInfo,
                                 dtlFuncVector  * aVectorFunc,
                                 void           * aVectorArg,
                                 void           * aEnv )
{
    dtlDataValue  * sSourceValue;                 // aInputArgument[0].mValue.mDataValue
    dtlDataValue  * sDestPrecisionValue;          // aInputArgument[1].mValue.mDataValue
//  dtlDataValue  * sDestScaleValue;              // aInputArgument[2].mValue.mDataValue
//  dtlDataValue  * sDestStringLengthUnitValue;   // aInputArgument[3].mValue.mDataValue
//  dtlDataValue  * sDestIntervalIndicatorValue;  // aInputArgument[4].mValue.mDataValue

    dtlDataValue  * sResultValue;

    stlInt64        sDestPrecision;
    stlInt64        sPaddNullLen;
    
    stlErrorStack * sErrorStack;

    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
    sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
//  sDestScaleValue              = aInputArgument[2].mValue.mDataValue;
//  sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue;
//  sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue;
    
    sResultValue = (dtlDataValue *)aResultValue;
    
    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_BINARY, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_BINARY, sErrorStack );

    sDestPrecision = *(stlInt64 *)(sDestPrecisionValue->mValue);

    STL_TRY_THROW( sDestPrecision >= sSourceValue->mLength, ERRCODE_STRING_DATA_RIGHT_TRUNCATION );

    stlMemcpy( sResultValue->mValue,
               sSourceValue->mValue,
               sSourceValue->mLength );
    
    if( sDestPrecision > sSourceValue->mLength )
    {        
        /* 차이나는 precision만큼 null padding */
        sPaddNullLen = sDestPrecision - sSourceValue->mLength;
        stlMemset( ((stlChar*)sResultValue->mValue + sSourceValue->mLength), 0, sPaddNullLen );

        /* null padding한 길이만큼 length계산 */
        sResultValue->mLength = sSourceValue->mLength + sPaddNullLen;
    }
    else
    {
        sResultValue->mLength = sSourceValue->mLength;
    }

    return STL_SUCCESS;

    STL_CATCH( ERRCODE_STRING_DATA_RIGHT_TRUNCATION )
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
 * @brief Binary -> Varbinary 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastBinaryToVarbinary( stlUInt16        aInputArgumentCnt,
                                    dtlValueEntry  * aInputArgument,
                                    void           * aResultValue,
                                    void           * aInfo,
                                    dtlFuncVector  * aVectorFunc,
                                    void           * aVectorArg,
                                    void           * aEnv )
{
    dtlDataValue  * sSourceValue;                 // aInputArgument[0].mValue.mDataValue
    dtlDataValue  * sDestPrecisionValue;          // aInputArgument[1].mValue.mDataValue
//  dtlDataValue  * sDestScaleValue;              // aInputArgument[2].mValue.mDataValue
//  dtlDataValue  * sDestStringLengthUnitValue;   // aInputArgument[3].mValue.mDataValue
//  dtlDataValue  * sDestIntervalIndicatorValue;  // aInputArgument[4].mValue.mDataValue

    dtlDataValue  * sResultValue;

    stlInt64        sDestPrecision;
    
    stlErrorStack * sErrorStack;

    sSourceValue                 = aInputArgument[0].mValue.mDataValue;
    sDestPrecisionValue          = aInputArgument[1].mValue.mDataValue;
//  sDestScaleValue              = aInputArgument[2].mValue.mDataValue;
//  sDestStringLengthUnitValue   = aInputArgument[3].mValue.mDataValue;
//  sDestIntervalIndicatorValue  = aInputArgument[4].mValue.mDataValue;
    
    sResultValue = (dtlDataValue *)aResultValue;
    
    sErrorStack = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_BINARY, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_VARBINARY, sErrorStack );

    sDestPrecision = *(stlInt64 *)(sDestPrecisionValue->mValue);

    STL_TRY_THROW( sDestPrecision >= sSourceValue->mLength, ERRCODE_STRING_DATA_RIGHT_TRUNCATION );

    stlMemcpy( sResultValue->mValue,
               sSourceValue->mValue,
               sSourceValue->mLength );
    
    sResultValue->mLength = sSourceValue->mLength;

    return STL_SUCCESS;

    STL_CATCH( ERRCODE_STRING_DATA_RIGHT_TRUNCATION )
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
 * @brief Binary -> Longvarbinary 으로의 type cast
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      cast연산결과
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack) 
 */
stlStatus dtcCastBinaryToLongvarbinary( stlUInt16        aInputArgumentCnt,
                                        dtlValueEntry  * aInputArgument,
                                        void           * aResultValue,
                                        void           * aInfo,
                                        dtlFuncVector  * aVectorFunc,
                                        void           * aVectorArg,
                                        void           * aEnv )
{
    dtlDataValue  * sSourceValue;                 // aInputArgument[0].mValue.mDataValue
    dtlDataValue  * sResultValue;
    stlErrorStack * sErrorStack;

    sSourceValue = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;
    sErrorStack  = (stlErrorStack *)aEnv;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == DTL_CAST_INPUT_ARG_CNT, sErrorStack );    
    DTF_CHECK_DTL_VALUE( sSourceValue, DTL_TYPE_BINARY, sErrorStack );
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_LONGVARBINARY, sErrorStack );

    STL_TRY_THROW( DTL_LONGVARBINARY_MAX_PRECISION >= sSourceValue->mLength,
                   ERRCODE_STRING_DATA_RIGHT_TRUNCATION );

    /**
     * 메모리 확장이 필요한지 검사한다.
     * ( Binary -> Long varbinary로 변환시,
     *   현재는 메모리 확장이 필요한 경우가 없으나, 코드 남겨둠. )
     */
    
    if( sSourceValue->mLength > sResultValue->mBufferSize )
    {
        STL_TRY( aVectorFunc->mReallocLongVaryingMemFunc( aVectorArg,
                                                          sSourceValue->mLength,
                                                          &sResultValue->mValue,
                                                          sErrorStack )
                 == STL_SUCCESS );

        sResultValue->mBufferSize = sSourceValue->mLength;
    }
    
    stlMemcpy( sResultValue->mValue,
               sSourceValue->mValue,
               sSourceValue->mLength );
    
    sResultValue->mLength = sSourceValue->mLength;

    return STL_SUCCESS;

    STL_CATCH( ERRCODE_STRING_DATA_RIGHT_TRUNCATION )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_C_BINARY_BYTE_LENGTH_GREATER_THAN_COLUMN_LENGTH,
                      NULL,
                      sErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
