/*******************************************************************************
 * dtfGeneralComparison.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id:$
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file dtfGeneralComparison.c
 * @brief dtfGeneralComparison Function DataType Library Layer
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtfGeneralComparison.h>

/**
 * @addtogroup dtfGeneralComparison  Greatest Least
 * @ingroup dtf
 * @internal
 * @{
 */




/*******************************************************************************
 * LEAST
 *   LEAST( expr, ....., expr )
 *     LEAST( expr, ....., expr ) => [ O ]
 * ex) LEAST( 1, 2, 3  ) => 1
 *
 * GREATEST
 *   GREATEST( expr, ....., expr )
 *     GREATEST( expr, ....., expr ) => [ O ]
 * ex) GREATEST( 1, 2, 3  ) => 3
 *******************************************************************************/

/**
 * @brief      LEAST( expr, ....., expr ) : expr 들 중 가장 작은 값 반환.
 *        <BR> LEAST( expr, ....., expr )
 *        <BR>   LEAST( expr, ....., expr ) => [ O ]
 *        <BR> ex) LEAST( 1, 2, 3  ) => 1
 *        <BR>
 *        <BR> GREATEST( expr, ....., expr ) : expr 들 중 가장 큰 값 반환.
 *        <BR>   GREATEST( expr, ....., expr ) => [ O ]
 *        <BR> ex) ( 1, 2, 3  ) => 3
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      결과
 * @param[in]  aCompareResult    LEAST    : DTL_COMPARISON_LESS <BR>
 *                               GREATEST : DTL_COMPARISON_GREATER 
 * @param[in]  aInfo             function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       aVectorFunc
 * @param[in]  aVectorArg        aVectorArg
 * @param[in]  aEnv              environment (stlErrorStack)
 */
stlStatus dtfGeneralComparison( stlUInt16          aInputArgumentCnt,
                                dtlValueEntry    * aInputArgument,
                                void             * aResultValue,
                                dtlCompareResult   aCompareResult,
                                void             * aInfo,
                                dtlFuncVector    * aVectorFunc,
                                void             * aVectorArg,
                                void             * aEnv )
{
    dtlDataValue    * sValue;    
    dtlDataValue    * sResultValue;
    
    stlUInt16         i = 0;
    dtlGroup          sResultGroup = DTL_GROUP_MAX;
    dtlDataValue    * sCompareResultValue = NULL;
    
    dtlDataTypeInfo * sResultTypeInfo = NULL;
    dtlValueEntry     sCastInputArgument[5];

    dtlDataValue      sDestPrecisionValue;
    dtlDataValue      sDestScaleValue;
    dtlDataValue      sDestStringLengthUnitValue;
    dtlDataValue      sDestIntervalIndicatorValue;
    
    stlInt64          sDestPrecision = DTL_PRECISION_NA;
    stlInt64          sDestScale = DTL_SCALE_NA;
    stlInt32          sDestStringLengthUnit = DTL_STRING_LENGTH_UNIT_NA;
    stlInt32          sDestIntervalIndicator = DTL_INTERVAL_INDICATOR_NA;

    
    sDestPrecisionValue.mType       = DTL_TYPE_NATIVE_BIGINT;
    sDestPrecisionValue.mValue      = &sDestPrecision;
    sDestPrecisionValue.mBufferSize = DTL_NATIVE_BIGINT_SIZE;
    sDestPrecisionValue.mLength     = DTL_NATIVE_BIGINT_SIZE;
    
    sDestScaleValue.mType       = DTL_TYPE_NATIVE_BIGINT;
    sDestScaleValue.mValue      = &sDestScale;
    sDestScaleValue.mBufferSize = DTL_NATIVE_BIGINT_SIZE;
    sDestScaleValue.mLength     = DTL_NATIVE_BIGINT_SIZE;

    sDestStringLengthUnitValue.mType       = DTL_TYPE_NATIVE_INTEGER;
    sDestStringLengthUnitValue.mValue      = &sDestStringLengthUnit;
    sDestStringLengthUnitValue.mBufferSize = DTL_NATIVE_INTEGER_SIZE;
    sDestStringLengthUnitValue.mLength     = DTL_NATIVE_INTEGER_SIZE;

    sDestIntervalIndicatorValue.mType       = DTL_TYPE_NATIVE_INTEGER;
    sDestIntervalIndicatorValue.mValue      = &sDestIntervalIndicator;
    sDestIntervalIndicatorValue.mBufferSize = DTL_NATIVE_INTEGER_SIZE;
    sDestIntervalIndicatorValue.mLength     = DTL_NATIVE_INTEGER_SIZE;

    sResultTypeInfo = (dtlDataTypeInfo *)aInfo;

    sResultValue = (dtlDataValue *)aResultValue;
    
    STL_DASSERT( ( aInputArgumentCnt >= DTL_GENERAL_COMPARISON_FUN_INPUT_MIN_ARG_CNT ) &&
                 ( aInputArgumentCnt <= DTL_GENERAL_COMPARISON_FUN_INPUT_MAX_ARG_CNT ) );
    
    sValue = aInputArgument[0].mValue.mDataValue;
    sCompareResultValue = sValue;
    
    for( i = 1; i < aInputArgumentCnt; i++ )
    {
        sValue = aInputArgument[i].mValue.mDataValue;
        
        if( dtlPhysicalCompareFuncList[ sValue->mType ][ sCompareResultValue->mType ](
                sValue->mValue,
                sValue->mLength,
                sCompareResultValue->mValue,
                sCompareResultValue->mLength ) == aCompareResult )
        {
            sCompareResultValue = sValue;
        }
        else
        {
            /* Do Nothing */
        }
    }
    
    /*
     * result 결과값 생성.
     */
    
    sResultGroup = dtlDataTypeGroup[ sResultValue->mType ];
    STL_DASSERT( dtlConversionGroup[ dtlDataTypeGroup[sCompareResultValue->mType] ][ sResultGroup ]
                 == sResultGroup );
    
    sDestPrecision = sResultTypeInfo->mArgNum1;
    sDestScale     = sResultTypeInfo->mArgNum2;
    
    if( ( sResultGroup == DTL_GROUP_CHAR_STRING ) ||
        ( sResultGroup == DTL_GROUP_LONGVARCHAR_STRING ) )
    {
        STL_DASSERT( sResultTypeInfo->mArgNum3 >= 0 );
        STL_DASSERT( sResultTypeInfo->mArgNum3 < DTL_STRING_LENGTH_UNIT_MAX );
        
        sDestStringLengthUnit = sResultTypeInfo->mArgNum3;
    }
    else if( ( sResultGroup == DTL_GROUP_INTERVAL_YEAR_TO_MONTH ) ||
             ( sResultGroup == DTL_GROUP_INTERVAL_DAY_TO_SECOND ) )
    {
        STL_DASSERT( sResultTypeInfo->mArgNum3 >= 0 );
        STL_DASSERT( sResultTypeInfo->mArgNum3 < DTL_INTERVAL_INDICATOR_MAX );
        
        sDestIntervalIndicator = sResultTypeInfo->mArgNum3;
    }
    else
    {
        STL_DASSERT( sResultTypeInfo->mArgNum3 == 0 );
        
        /* Do Nothing */
    }
    
    sCastInputArgument[0].mValue.mDataValue = sCompareResultValue;
    sCastInputArgument[1].mValue.mDataValue = & sDestPrecisionValue;
    sCastInputArgument[2].mValue.mDataValue = & sDestScaleValue;
    sCastInputArgument[3].mValue.mDataValue = & sDestStringLengthUnitValue;
    sCastInputArgument[4].mValue.mDataValue = & sDestIntervalIndicatorValue;

    STL_DASSERT( dtlCastFunctionList[ sCompareResultValue->mType ][ sResultValue->mType ] != NULL );
    
    STL_TRY( dtlCastFunctionList[ sCompareResultValue->mType ][ sResultValue->mType ](
                 DTL_CAST_INPUT_ARG_CNT,
                 sCastInputArgument,
                 sResultValue,
                 NULL,
                 aVectorFunc,
                 aVectorArg,
                 aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;
       
    STL_FINISH;    
    
    return STL_FAILURE;    
}



/** @} */
