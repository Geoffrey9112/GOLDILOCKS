/*******************************************************************************
 * dtcCast.c
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
 * @file dtcCast.c
 * @brief DataType Layer conversion함수
 */

#include <dtl.h>
#include <dtDef.h>

#include <dtcCast.h>


/**
 * @brief cast function에 대한 정보 얻기
 * @param[in]  aDataTypeArrayCount         aDataTypeArrary의 갯수
 * @param[in]  aIsConstantData             constant value인지 여부
 * @param[in]  aDataTypeArray              function을 수행할 data type
 * @param[in]  aDataTypeDefInfoArray       aDataTypeArray에 대응되는 dtlDataTypeDefInfo정보 
 * @param[in]  aFuncArgDataTypeArrayCount  function의 input argument count
 * @param[out] aFuncArgDataTypeArray       function 수행시 수행 argument type
 * @param[out] aFuncArgDataTypeDefInfoArray aFuncArgDataTypeArray에 대응되는 dtlDataTypeDefInfo정보
 * @param[out] aFuncResultDataType         function 수행시 return type
 * @param[out] aFuncResultDataTypeDefInfo  aFuncResultDataType에 대응하는 dtlDataTypeDefInfo정보
 * @param[out] aFuncPtrIdx                 함수 수행을 위한 function pointer 얻기 위한 식별 정보
 * @param[in]  aVectorFunc                 Function Vector
 * @param[in]  aVectorArg                  Vector Argument
 * @param[out] aErrorStack                 에러 스택
 */
stlStatus dtlGetFuncInfoCast( stlUInt16               aDataTypeArrayCount,
                              stlBool               * aIsConstantData,
                              dtlDataType           * aDataTypeArray,
                              dtlDataTypeDefInfo    * aDataTypeDefInfoArray,
                              stlUInt16               aFuncArgDataTypeArrayCount,
                              dtlDataType           * aFuncArgDataTypeArray,
                              dtlDataTypeDefInfo    * aFuncArgDataTypeDefInfoArray,
                              dtlDataType           * aFuncResultDataType,
                              dtlDataTypeDefInfo    * aFuncResultDataTypeDefInfo,
                              stlUInt32             * aFuncPtrIdx,
                              dtlFuncVector         * aVectorFunc,
                              void                  * aVectorArg,                               
                              stlErrorStack         * aErrorStack )
{
    dtlDataType     sType1;
    dtlDataType     sType2;

    STL_PARAM_VALIDATE( aDataTypeArrayCount == 2, aErrorStack );
    STL_PARAM_VALIDATE( aFuncArgDataTypeArrayCount == 1, aErrorStack );

    sType1 = aDataTypeArray[ 0 ];
    sType2 = aDataTypeArray[ 1 ];

    /*
     * cast 가능 여부
     */
    
    STL_TRY_THROW( dtlIsApplicableCast( sType1,
                                        aDataTypeDefInfoArray[0].mIntervalIndicator,
                                        sType2,
                                        aDataTypeDefInfoArray[1].mIntervalIndicator ) == STL_TRUE,
                   ERROR_NOT_APPLICABLE );
    
    /**
     * output 설정
     */
    
    aFuncArgDataTypeArray[0] = aDataTypeArray[0];

    aFuncArgDataTypeDefInfoArray[0].mArgNum1 = aDataTypeDefInfoArray[0].mArgNum1;
    aFuncArgDataTypeDefInfoArray[0].mArgNum2 = aDataTypeDefInfoArray[0].mArgNum2;
    aFuncArgDataTypeDefInfoArray[0].mStringLengthUnit = aDataTypeDefInfoArray[0].mStringLengthUnit;
    aFuncArgDataTypeDefInfoArray[0].mIntervalIndicator = aDataTypeDefInfoArray[0].mIntervalIndicator;
    
    *aFuncResultDataType = aDataTypeArray[1];

    aFuncResultDataTypeDefInfo->mArgNum1 = aDataTypeDefInfoArray[1].mArgNum1;
    aFuncResultDataTypeDefInfo->mArgNum2 = aDataTypeDefInfoArray[1].mArgNum2;
    aFuncResultDataTypeDefInfo->mStringLengthUnit = aDataTypeDefInfoArray[1].mStringLengthUnit;
    aFuncResultDataTypeDefInfo->mIntervalIndicator = aDataTypeDefInfoArray[1].mIntervalIndicator;

    /*
    * aFuncPtrIdx 에 Number 의 타입 정보를 추가 한다.
    * aFuncPtrIdx 값범위별 의미
    * 0     ~ 99     : sType2
    * 100   ~ 9900   : sType1
    */
//    *aFuncPtrIdx = ( sType1 * 100 ) + sType2;
    *aFuncPtrIdx = DTL_GET_CAST_FUNC_PTR_IDX( sType1, sType2 );
        
    return STL_SUCCESS;
    
    STL_CATCH( ERROR_NOT_APPLICABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_CONVERSION_NOT_APPLICABLE,
                      NULL,
                      aErrorStack,
                      dtlDataTypeName[sType1],
                      dtlDataTypeName[sType2] );
    }
        
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief cast function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus dtlGetFuncPtrCast( stlUInt32             aFuncPtrIdx,
                             dtlBuiltInFuncPtr   * aFuncPtr,
                             stlErrorStack       * aErrorStack )
{
    /**
     * output 설정
     */

    /*
     * aFuncPtrIdx 에서 Number 의 상세 타입에 따른
     * Cast 함수 포인터 assign
     * 
     */ 

    *aFuncPtr = dtlCastFunctionList[ aFuncPtrIdx / 100 ][ aFuncPtrIdx % 100 ];
    
    return STL_SUCCESS;
}


/**
 * @brief cast가 가능한지 여부 반환.
 * @param[in]  aSrcType                source datatype
 * @param[in]  aSrcIntervalIndicator   source type이 interval인 경우, interval indicator
 * @param[in]  aDstType                dest datatype
 * @param[in]  aDstIntervalIndicator   dest type이 interval인 경우, interval indicator
 * @return stlBool 
 */
stlBool dtlIsApplicableCast( dtlDataType           aSrcType,
                             dtlIntervalIndicator  aSrcIntervalIndicator,
                             dtlDataType           aDstType,
                             dtlIntervalIndicator  aDstIntervalIndicator )
{
    stlBool  sIsApplicable = STL_FALSE;
    
    /*
     * Numeric 의 경우 approximate 에 대한 함수 체크 추가
     */ 
    if( dtlCastFunctionList[ aSrcType ][ aDstType ] == NULL )
    {
        sIsApplicable = STL_FALSE;
    }
    else
    {
        sIsApplicable = STL_TRUE;
        
        if( (dtlDataTypeGroup[aSrcType] == DTL_GROUP_BINARY_INTEGER) ||
            (dtlDataTypeGroup[aSrcType] == DTL_GROUP_NUMBER) )
        {
            /**
             * exact numeric -> INTERVAL 으로 변경시
             * exact numeric -> INTERVAL single field 만 가능.
             */
        
            if( aDstType == DTL_TYPE_INTERVAL_YEAR_TO_MONTH )
            {
                if( (aDstIntervalIndicator == DTL_INTERVAL_INDICATOR_YEAR) ||
                    (aDstIntervalIndicator == DTL_INTERVAL_INDICATOR_MONTH) )
                {
                    /* Do Nothing */
                }
                else
                {
                    sIsApplicable = STL_FALSE;
                }
            }
            else if( aDstType == DTL_TYPE_INTERVAL_DAY_TO_SECOND )
            {
                if( (aDstIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY)    ||
                    (aDstIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR )  ||
                    (aDstIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE) ||
                    (aDstIntervalIndicator == DTL_INTERVAL_INDICATOR_SECOND) )
                {
                    /* Do Nothing */
                }
                else
                {
                    sIsApplicable = STL_FALSE;
                }
            }
            else
            {
                // Do Nothing
            }
        }
        else
        {
            /* Do Nothing */
        }
    
        if( (dtlDataTypeGroup[aDstType] == DTL_GROUP_BINARY_INTEGER) ||
            (dtlDataTypeGroup[aDstType] == DTL_GROUP_NUMBER) )
        {
            /**
             * INTERVAL -> exact numeric으로 변경시
             * INTERVAL single field -> exact numeric 만 가능.
             */
        
            if( aSrcType == DTL_TYPE_INTERVAL_YEAR_TO_MONTH )
            {
                if( (aSrcIntervalIndicator == DTL_INTERVAL_INDICATOR_YEAR) ||
                    (aSrcIntervalIndicator == DTL_INTERVAL_INDICATOR_MONTH) )
                {
                    /* Do Nothing */
                }
                else
                {
                    sIsApplicable = STL_FALSE;
                }
            }
            else if( aSrcType == DTL_TYPE_INTERVAL_DAY_TO_SECOND )
            {
                if( (aSrcIntervalIndicator == DTL_INTERVAL_INDICATOR_DAY)    ||
                    (aSrcIntervalIndicator == DTL_INTERVAL_INDICATOR_HOUR )  ||
                    (aSrcIntervalIndicator == DTL_INTERVAL_INDICATOR_MINUTE) ||
                    (aSrcIntervalIndicator == DTL_INTERVAL_INDICATOR_SECOND) )
                {
                    /* Do Nothing */
                }
                else
                {
                    sIsApplicable = STL_FALSE; 
                }
            }
            else
            {
                // Do Nothing
            }
        }
        else
        {
            // Do Nothing
        }
    }

    return sIsApplicable;
}


/** @} */
