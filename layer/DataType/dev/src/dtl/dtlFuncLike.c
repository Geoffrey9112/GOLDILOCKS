/*******************************************************************************
 * dtlFuncLike.c
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
 * @file dtlFuncLike.c
 * @brief Data Type Layer Like 함수 
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtfString.h>


/**
 * @addtogroup dtlFuncLike
 * @{
 */

stlStatus dtlLike( stlUInt16        aInputArgumentCnt,
                   dtlValueEntry  * aInputArgument,
                   void           * aResultValue,
                   void           * aInfo,
                   dtlFuncVector  * aVectorFunc,
                   void           * aVectorArg,
                   void           * aEnv )
{
    return dtfLike( aInputArgumentCnt,
                    aInputArgument,
                    aResultValue,
                    aInfo,
                    aVectorFunc,
                    aVectorArg,
                    aEnv );
}

stlStatus dtlNotLike( stlUInt16        aInputArgumentCnt,
                      dtlValueEntry  * aInputArgument,
                      void           * aResultValue,
                      void           * aInfo,
                      dtlFuncVector  * aVectorFunc,
                      void           * aVectorArg,
                      void           * aEnv )
{
    return dtfNotLike( aInputArgumentCnt,
                       aInputArgument,
                       aResultValue,
                       aInfo,
                       aVectorFunc,
                       aVectorArg,
                       aEnv );
}

stlStatus dtlLikePattern( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aInfo,
                          dtlFuncVector  * aVectorFunc,
                          void           * aVectorArg,
                          void           * aEnv )
{
    return dtfLikePattern( aInputArgumentCnt,
                           aInputArgument,
                           aResultValue,
                           aInfo,
                           aVectorFunc,
                           aVectorArg,
                           aEnv );
}

/**
 * @brief Like function에 대한 정보 얻기
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
stlStatus dtlGetFuncInfoLike( stlUInt16               aDataTypeArrayCount,
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
    dtlDataType   sType;
    
    STL_PARAM_VALIDATE( ( aDataTypeArrayCount == 2 ), aErrorStack );
    STL_PARAM_VALIDATE( ( aFuncArgDataTypeArrayCount == 2 ), aErrorStack );

    /**
     * input data type이 character string으로 변환될 수 있는지 검사 후
     * input data type을 varchar로 변환
     */
    sType = aDataTypeArray[0];

    if( sType == DTL_TYPE_LONGVARCHAR )
    {
        aFuncArgDataTypeArray[0] = DTL_TYPE_LONGVARCHAR;

        aFuncArgDataTypeDefInfoArray[0] = aDataTypeDefInfoArray[0];
    }
    else
    {
        STL_TRY_THROW( dtlCastFunctionList[ sType ][ DTL_TYPE_VARCHAR ] != NULL,
                       ERROR_NOT_APPLICABLE);

        if( ( dtlDataTypeGroup[sType] == DTL_GROUP_CHAR_STRING ) ||
            ( dtlDataTypeGroup[sType] == DTL_GROUP_LONGVARCHAR_STRING ) )
        {
            /* string 간 conversion이 발생하지 않도록 한다. */
            
            aFuncArgDataTypeArray[0] = sType;

            aFuncArgDataTypeDefInfoArray[0] = aDataTypeDefInfoArray[0];            
        }
        else
        {
            aFuncArgDataTypeArray[0] = DTL_TYPE_VARCHAR;

            aFuncArgDataTypeDefInfoArray[0].mArgNum1 =
                gResultDataTypeDef[aFuncArgDataTypeArray[0]].mArgNum1;
            aFuncArgDataTypeDefInfoArray[0].mArgNum2 =
                gResultDataTypeDef[aFuncArgDataTypeArray[0]].mArgNum2;
            aFuncArgDataTypeDefInfoArray[0].mStringLengthUnit =
                gResultDataTypeDef[aFuncArgDataTypeArray[0]].mStringLengthUnit;
            aFuncArgDataTypeDefInfoArray[0].mIntervalIndicator =
                gResultDataTypeDef[aFuncArgDataTypeArray[0]].mIntervalIndicator;
        }
    }
    
    aFuncArgDataTypeArray[1] = aDataTypeArray[1];
    
    aFuncArgDataTypeDefInfoArray[1].mArgNum1 =
        gResultDataTypeDef[aFuncArgDataTypeArray[1]].mArgNum1;
    aFuncArgDataTypeDefInfoArray[1].mArgNum2 =
        gResultDataTypeDef[aFuncArgDataTypeArray[1]].mArgNum2;
    aFuncArgDataTypeDefInfoArray[1].mStringLengthUnit =
        gResultDataTypeDef[aFuncArgDataTypeArray[1]].mStringLengthUnit;
    aFuncArgDataTypeDefInfoArray[1].mIntervalIndicator =
        gResultDataTypeDef[aFuncArgDataTypeArray[1]].mIntervalIndicator;
    
    /**
     * @todo function의 result data type의 info가 저장되는 부분을 고민해야 함.
     */
    
    *aFuncResultDataType = DTL_TYPE_BOOLEAN;

    aFuncResultDataTypeDefInfo->mArgNum1 = gResultDataTypeDef[DTL_TYPE_BOOLEAN].mArgNum1;
    aFuncResultDataTypeDefInfo->mArgNum2 = gResultDataTypeDef[DTL_TYPE_BOOLEAN].mArgNum2;
    aFuncResultDataTypeDefInfo->mStringLengthUnit =
        gResultDataTypeDef[DTL_TYPE_BOOLEAN].mStringLengthUnit;
    aFuncResultDataTypeDefInfo->mIntervalIndicator =
        gResultDataTypeDef[DTL_TYPE_BOOLEAN].mIntervalIndicator;

    *aFuncPtrIdx = 0;

    return STL_SUCCESS;
    
    STL_CATCH( ERROR_NOT_APPLICABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_CONVERSION_NOT_APPLICABLE,
                      NULL,
                      aErrorStack,
                      dtlDataTypeName[sType],
                      dtlDataTypeName[DTL_TYPE_VARCHAR] );
    }
        
    STL_FINISH;
    
    return STL_FAILURE;

}

/**
 * @brief Like Pattern function에 대한 정보 얻기
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
stlStatus dtlGetFuncInfoLikePattern( stlUInt16               aDataTypeArrayCount,
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
    dtlDataType   sType;
    stlUInt8      i;

    STL_PARAM_VALIDATE( ( ( aDataTypeArrayCount == 1 ) || ( aDataTypeArrayCount == 2 ) ), aErrorStack );
    STL_PARAM_VALIDATE( ( ( aFuncArgDataTypeArrayCount == 1 ) || ( aFuncArgDataTypeArrayCount == 2 ) ), aErrorStack );

    /**
     * input data type이 character string으로 변환될 수 있는지 검사 후
     * input data type을 varchar로 변환
     */
    for( i = 0; i < aFuncArgDataTypeArrayCount; i++ )
    {
        sType = aDataTypeArray[i];

        if( sType == DTL_TYPE_LONGVARCHAR )
        {
            aFuncArgDataTypeArray[i] = DTL_TYPE_LONGVARCHAR;
        
            aFuncArgDataTypeDefInfoArray[i] = aDataTypeDefInfoArray[i];
        }
        else
        {
            STL_TRY_THROW( dtlCastFunctionList[ sType ][ DTL_TYPE_VARCHAR ] != NULL,
                           ERROR_NOT_APPLICABLE);
        
            if( ( dtlDataTypeGroup[sType] == DTL_GROUP_CHAR_STRING ) ||
                ( dtlDataTypeGroup[sType] == DTL_GROUP_LONGVARCHAR_STRING ) )
            {
                /* string 간 conversion이 발생하지 않도록 한다. */
                
                aFuncArgDataTypeArray[i] = sType;
            
                aFuncArgDataTypeDefInfoArray[i] = aDataTypeDefInfoArray[i];
            }
            else
            {
                aFuncArgDataTypeArray[i] = DTL_TYPE_VARCHAR;
            
                aFuncArgDataTypeDefInfoArray[i].mArgNum1 =
                    gResultDataTypeDef[aFuncArgDataTypeArray[i]].mArgNum1;
                aFuncArgDataTypeDefInfoArray[i].mArgNum2 =
                    gResultDataTypeDef[aFuncArgDataTypeArray[i]].mArgNum2;
                aFuncArgDataTypeDefInfoArray[i].mStringLengthUnit =
                    gResultDataTypeDef[aFuncArgDataTypeArray[i]].mStringLengthUnit;
                aFuncArgDataTypeDefInfoArray[i].mIntervalIndicator =
                    gResultDataTypeDef[aFuncArgDataTypeArray[i]].mIntervalIndicator;
            }
        }
    }
    
    *aFuncResultDataType = DTL_TYPE_LONGVARBINARY;

    aFuncResultDataTypeDefInfo->mArgNum1 = gResultDataTypeDef[DTL_TYPE_LONGVARBINARY].mArgNum1;
    aFuncResultDataTypeDefInfo->mArgNum2 = gResultDataTypeDef[DTL_TYPE_LONGVARBINARY].mArgNum2;
    aFuncResultDataTypeDefInfo->mStringLengthUnit =
        gResultDataTypeDef[DTL_TYPE_LONGVARBINARY].mStringLengthUnit;
    aFuncResultDataTypeDefInfo->mIntervalIndicator =
        gResultDataTypeDef[DTL_TYPE_LONGVARBINARY].mIntervalIndicator;

    *aFuncPtrIdx = 0;

    return STL_SUCCESS;

    STL_CATCH( ERROR_NOT_APPLICABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_CONVERSION_NOT_APPLICABLE,
                      NULL,
                      aErrorStack,
                      dtlDataTypeName[sType],
                      dtlDataTypeName[DTL_TYPE_VARCHAR] );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Like function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus dtlGetFuncPtrLike( stlUInt32             aFuncPtrIdx,
                             dtlBuiltInFuncPtr   * aFuncPtr,
                             stlErrorStack       * aErrorStack )
{
    /**
     * output 설정
     */
    *aFuncPtr = dtlLike;
    
    return STL_SUCCESS;
}

/**
 * @brief Not Like function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus dtlGetFuncPtrNotLike( stlUInt32             aFuncPtrIdx,
                                dtlBuiltInFuncPtr   * aFuncPtr,
                                stlErrorStack       * aErrorStack )
{
    /**
     * output 설정
     */
    *aFuncPtr = dtlNotLike;
    
    return STL_SUCCESS;
}

/**
 * @brief Like Pattern function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus dtlGetFuncPtrLikePattern( stlUInt32             aFuncPtrIdx,
                                    dtlBuiltInFuncPtr   * aFuncPtr,
                                    stlErrorStack       * aErrorStack )
{
    /**
     * output 설정
     */
    *aFuncPtr = dtlLikePattern;

    return STL_SUCCESS;
}

/** @} */
