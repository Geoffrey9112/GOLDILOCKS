/*******************************************************************************
 * dtlFuncToChar.c
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
 * @file dtlFuncToChar.c
 * @brief Data Type Layer To_Char 함수 
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtfFormatting.h>

/**
 * @ingroup dtlFunction Function관련 함수
 * @{
 */


/**
 * @brief To_Char  function에 대한 정보 얻기
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
stlStatus dtlGetFuncInfoToChar( stlUInt16               aDataTypeArrayCount,
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
    dtlGroup            sGroup = DTL_GROUP_MAX;
    dtlBuiltInFuncPtr   sToCharFunc = NULL;
    stlChar             sErrMsg[128];

    /**
     * function argument 
     */
    
    sToCharFunc = gDtfToCharFunc[ aDataTypeArray[0] ];    
    sGroup = dtlDataTypeGroup[ aDataTypeArray[0] ];
    
    if( sToCharFunc == NULL )
    {
        sGroup = dtlDataTypeGroup[ aDataTypeArray[0] ];
        
        if( sGroup == DTL_GROUP_BINARY_INTEGER )
        {
            /**
             * @todo smallint, integer, bigint에 대한 처리도 추가해야 함.
             * smallint, integer, bigint 에 대한 함수 추가전까지 임시 코드임.
             */
            
            aFuncArgDataTypeArray[0] = DTL_TYPE_NUMBER;
            
            aFuncArgDataTypeDefInfoArray[ 0 ].mArgNum1 =
                gResultDataTypeDef[ DTL_TYPE_NUMBER ].mArgNum1;
            aFuncArgDataTypeDefInfoArray[ 0 ].mArgNum2 =
                gResultDataTypeDef[ DTL_TYPE_NUMBER ].mArgNum2;
            aFuncArgDataTypeDefInfoArray[ 0 ].mStringLengthUnit =
                gResultDataTypeDef[ DTL_TYPE_NUMBER ].mStringLengthUnit;
            aFuncArgDataTypeDefInfoArray[ 0 ].mIntervalIndicator =
                gResultDataTypeDef[ DTL_TYPE_NUMBER ].mIntervalIndicator;
            
        }
        else
        {
            STL_THROW( ERROR_NOT_APPLICABLE );
        }
    }
    else
    {
        aFuncArgDataTypeArray[0] = aDataTypeArray[ 0 ];
        
        aFuncArgDataTypeDefInfoArray[ 0 ].mArgNum1 =
            aDataTypeDefInfoArray[ 0 ].mArgNum1;
        aFuncArgDataTypeDefInfoArray[ 0 ].mArgNum2 =
            aDataTypeDefInfoArray[ 0 ].mArgNum2;
        aFuncArgDataTypeDefInfoArray[ 0 ].mStringLengthUnit =
            aDataTypeDefInfoArray[ 0 ].mStringLengthUnit;
        aFuncArgDataTypeDefInfoArray[ 0 ].mIntervalIndicator =
            aDataTypeDefInfoArray[ 0 ].mIntervalIndicator;
    }

    aFuncArgDataTypeArray[ 1 ] = DTL_TYPE_VARBINARY;
    
    aFuncArgDataTypeDefInfoArray[ 1 ].mArgNum1 =
        gResultDataTypeDef[ DTL_TYPE_VARBINARY ].mArgNum1;
    aFuncArgDataTypeDefInfoArray[ 1 ].mArgNum2 =
        gResultDataTypeDef[ DTL_TYPE_VARBINARY ].mArgNum2;
    aFuncArgDataTypeDefInfoArray[ 1 ].mStringLengthUnit =
        gResultDataTypeDef[ DTL_TYPE_VARBINARY ].mStringLengthUnit;
    aFuncArgDataTypeDefInfoArray[ 1 ].mIntervalIndicator =
        gResultDataTypeDef[ DTL_TYPE_VARBINARY ].mIntervalIndicator;
    
    
    /**
     * result type의 기본정보 설정.
     *
     * 1. char         => result type : char
     * 2. varchar      => result type : varchar
     * 3. long varchar => result type : long varchar
     * 4, 그외 타입들  => result type : varchar
     */
    
    *aFuncResultDataType = DTL_TYPE_VARCHAR;

    aFuncResultDataTypeDefInfo->mArgNum1 =
        gResultDataTypeDef[ *aFuncResultDataType ].mArgNum1;
    aFuncResultDataTypeDefInfo->mArgNum2 =
        gResultDataTypeDef[ *aFuncResultDataType ].mArgNum2;
    aFuncResultDataTypeDefInfo->mStringLengthUnit =
        gResultDataTypeDef[ *aFuncResultDataType ].mStringLengthUnit;
    aFuncResultDataTypeDefInfo->mIntervalIndicator =
        gResultDataTypeDef[ *aFuncResultDataType ].mIntervalIndicator;
    
    sGroup = dtlDataTypeGroup[ aFuncArgDataTypeArray[0] ];

    switch( sGroup )
    {
        case DTL_GROUP_BINARY_INTEGER:
        case DTL_GROUP_NUMBER:
        case DTL_GROUP_BINARY_REAL:
            {
                /**
                 * @todo to_char( number ) 에 대한 처리.
                 *       to_char( number, fmt ) 인 경우만 아래코드가 처리될 수 있도록.
                 */
                aFuncResultDataTypeDefInfo->mArgNum1 = DTF_NUMBER_TO_CHAR_RESULT_STRING_MAX_SIZE;
                
                break;
            }
        case DTL_GROUP_CHAR_STRING:
        case DTL_GROUP_LONGVARCHAR_STRING:
            {
                *aFuncResultDataType = aFuncArgDataTypeArray[0];
                
                aFuncResultDataTypeDefInfo->mArgNum1 =
                    aFuncArgDataTypeDefInfoArray[ 0 ].mArgNum1;
                aFuncResultDataTypeDefInfo->mArgNum2 =
                    aFuncArgDataTypeDefInfoArray[ 0 ].mArgNum2;
                aFuncResultDataTypeDefInfo->mStringLengthUnit =
                    aFuncArgDataTypeDefInfoArray[ 0 ].mStringLengthUnit;
                aFuncResultDataTypeDefInfo->mIntervalIndicator =
                    aFuncArgDataTypeDefInfoArray[ 0 ].mIntervalIndicator;
                
                break;
            }
        case DTL_GROUP_DATE:
        case DTL_GROUP_TIME:
        case DTL_GROUP_TIMESTAMP:
        case DTL_GROUP_TIME_WITH_TIME_ZONE:
        case DTL_GROUP_TIMESTAMP_WITH_TIME_ZONE:
        case DTL_GROUP_INTERVAL_YEAR_TO_MONTH:
        case DTL_GROUP_INTERVAL_DAY_TO_SECOND:
            {
                aFuncResultDataTypeDefInfo->mArgNum1 =
                    (gResultDataTypeDef[aFuncArgDataTypeArray[0]].mStringBufferSize - 1);
                
                break;
            }
        case DTL_GROUP_BOOLEAN:
        case DTL_GROUP_BINARY_STRING:
        case DTL_GROUP_LONGVARBINARY_STRING:
        case DTL_GROUP_ROWID:
        default:
            STL_DASSERT( STL_FALSE );
            break;
    }
    
    *aFuncPtrIdx = 0;
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_NOT_APPLICABLE )
    {
        sErrMsg[0] = 0x00;

        stlSnprintf( sErrMsg + stlStrlen( sErrMsg ),
                     STL_SIZEOF( sErrMsg ) - stlStrlen( sErrMsg ),
                     "%s, ",
                     dtlDataTypeName[aDataTypeArray[0]]);
        
        stlSnprintf( sErrMsg + stlStrlen( sErrMsg ),
                     STL_SIZEOF( sErrMsg ) - stlStrlen( sErrMsg ),
                     "%s",
                     dtlDataTypeName[aDataTypeArray[1]]);
        
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INCONSISTENT_DATATYPE_FUNCTION_PARAMETERS,
                      NULL,
                      aErrorStack,
                      sErrMsg );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief To_Char function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus dtlGetFuncPtrToChar( stlUInt32             aFuncPtrIdx,
                               dtlBuiltInFuncPtr   * aFuncPtr,
                               stlErrorStack       * aErrorStack )
{
    /**
     * output 설정
     */

    *aFuncPtr = dtlToChar;
    
    return STL_SUCCESS;
}


stlStatus dtlToChar( stlUInt16        aInputArgumentCnt,
                     dtlValueEntry  * aInputArgument,
                     void           * aResultValue,
                     void           * aInfo,
                     dtlFuncVector  * aVectorFunc,
                     void           * aVectorArg,
                     void           * aEnv )
{
    stlChar     sErrMsg[128];
    
    STL_TRY_THROW( gDtfToCharFunc[ aInputArgument[0].mValue.mDataValue->mType ] != NULL,
                   ERROR_NOT_APPLICABLE );
    
    STL_TRY( gDtfToCharFunc[ aInputArgument[0].mValue.mDataValue->mType ](
                 aInputArgumentCnt,
                 aInputArgument,
                 aResultValue,
                 aInfo,
                 aVectorFunc,
                 aVectorArg,
                 (stlErrorStack *)aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_CATCH( ERROR_NOT_APPLICABLE )
    {
        sErrMsg[0] = 0x00;

        stlSnprintf( sErrMsg + stlStrlen( sErrMsg ),
                     STL_SIZEOF( sErrMsg ) - stlStrlen( sErrMsg ),
                     "%s, ",
                     dtlDataTypeName[ aInputArgument[0].mValue.mDataValue->mType ]);
        
        stlSnprintf( sErrMsg + stlStrlen( sErrMsg ),
                     STL_SIZEOF( sErrMsg ) - stlStrlen( sErrMsg ),
                     "%s",
                     dtlDataTypeName[ aInputArgument[1].mValue.mDataValue->mType ]);
        
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INCONSISTENT_DATATYPE_FUNCTION_PARAMETERS,
                      NULL,
                      (stlErrorStack *)aEnv,
                      sErrMsg );
    }
    
    STL_FINISH;
    
    return STL_FAILURE;
    
}

/** @} */
