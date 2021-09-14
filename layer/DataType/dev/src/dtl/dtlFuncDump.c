/*******************************************************************************
 * dtlFuncDump.c
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
 * @file dtlFuncDump.c
 * @brief Data Type Layer DUMP Function 
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtfSystem.h>

/**
 * @addtogroup dtlFuncDump DUMP Function
 * @ingroup dtlOperLogical
 * @{
 */


/**
 * @brief DUMP function에 대한 정보 얻기
 * @param[in]  aDataTypeArrayCount         aDataTypeArray의 갯수
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
stlStatus dtlGetFuncInfoDump( stlUInt16               aDataTypeArrayCount,
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
    stlInt64  sPrecision = 0;
    
    /*
     * parameter validation
     */

    STL_PARAM_VALIDATE( aDataTypeArrayCount == 1, aErrorStack );
    STL_PARAM_VALIDATE( aFuncArgDataTypeArrayCount == 1, aErrorStack );

    
    /**
     * Dump String의 Precision 구하기
     * @remark Dump String 표현
     *         'type' + 'type info' + 'length' + '멤버별 byte string'
     *
     *         예제)
     *               "Type=NATIVE_SMALLINT Len=2 : Val=15,11"
     *               "Type=VARCHAR Len=3 : Str=65,66,67"
     *               "Type=NUMERIC Len=4 : Expo=193 Digit=36,67,09"
     *               "Type=TIMESTAMP_TZ Len=12 : Time=123,10,10,10,20,30,10,13 Tz=34,62,152,44"
     *               "Type=INTERVAL_YM(YM) Len=8 : Ind=0,0,0,1 Month=0,0,0,13"
     *               "Type=INTERVAL_DS(DH) Len=12 : Ind=0,0,0,2 Day=0,0,0,13 Time=34,62,152,44"
     */

    sPrecision = DTL_DUMP_FUNC_TYPE_INFO_SIZE;
    
    switch( aDataTypeArray[ 0 ] )
    {
        case DTL_TYPE_BOOLEAN :
            {
                sPrecision += DTL_BOOLEAN_SIZE * 4;
                break;
            }
        case DTL_TYPE_NATIVE_SMALLINT :
            {
                sPrecision += DTL_NATIVE_SMALLINT_SIZE * 4;
                break;
            }
        case DTL_TYPE_NATIVE_INTEGER :
            {
                sPrecision += DTL_NATIVE_INTEGER_SIZE * 4;
                break;
            }
        case DTL_TYPE_NATIVE_BIGINT :
            {
                sPrecision += DTL_NATIVE_BIGINT_SIZE * 4;
                break;
            }
        case DTL_TYPE_NATIVE_REAL :
            {
                sPrecision += DTL_NATIVE_REAL_SIZE * 4;
                break;
            }
        case DTL_TYPE_NATIVE_DOUBLE :
            {
                sPrecision += DTL_NATIVE_DOUBLE_SIZE * 4;
                break;
            }
        case DTL_TYPE_FLOAT :
            {
                sPrecision += DTL_NUMERIC_SIZE( dtlBinaryToDecimalPrecision[ aDataTypeDefInfoArray[ 0 ].mArgNum1 ] ) * 4;
                break;
            }
        case DTL_TYPE_NUMBER :
            {
                sPrecision += DTL_NUMERIC_SIZE( aDataTypeDefInfoArray[ 0 ].mArgNum1 ) * 4;
                break;
            }
        case DTL_TYPE_DECIMAL :
            {
                sPrecision += DTL_NUMERIC_SIZE( aDataTypeDefInfoArray[ 0 ].mArgNum1 ) * 4;
                break;
            }
        case DTL_TYPE_CHAR :
        case DTL_TYPE_VARCHAR :
            {
                if( aDataTypeDefInfoArray[ 0 ].mStringLengthUnit ==
                    DTL_STRING_LENGTH_UNIT_CHARACTERS )
                {
                    sPrecision += DTL_CHAR_STRING_COLUMN_SIZE( aDataTypeDefInfoArray[ 0 ].mArgNum1,
                                                               aVectorFunc,
                                                               aVectorArg ) * 4;
                }
                else
                {
                    sPrecision += aDataTypeDefInfoArray[ 0 ].mArgNum1 * 4;
                }
                break;
            }
        case DTL_TYPE_BINARY :
        case DTL_TYPE_VARBINARY :
            {
                sPrecision += aDataTypeDefInfoArray[ 0 ].mArgNum1 * 4;
                break;
            }
        case DTL_TYPE_LONGVARCHAR :
        case DTL_TYPE_LONGVARBINARY :
            {
                sPrecision = DTL_VARCHAR_MAX_PRECISION;
                break;
            }
        case DTL_TYPE_CLOB :
        case DTL_TYPE_BLOB :
            {
                /* invalid */
                break;
            }
        case DTL_TYPE_DATE :
            {
                sPrecision += DTL_DATE_SIZE * 4;
                break;
            }
        case DTL_TYPE_TIME :
            {
                sPrecision += DTL_TIME_SIZE * 4;
                break;
            }
        case DTL_TYPE_TIMESTAMP :
            {
                sPrecision += DTL_TIMESTAMP_SIZE * 4;
                break;
            }
        case DTL_TYPE_TIME_WITH_TIME_ZONE :
            {
                sPrecision += DTL_TIMETZ_SIZE * 4;
                break;
            }
        case DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE :
            {
                sPrecision += DTL_TIMESTAMPTZ_SIZE * 4;
                break;
            }
        case DTL_TYPE_INTERVAL_YEAR_TO_MONTH :
            {
                sPrecision += DTL_INTERVAL_YEAR_TO_MONTH_SIZE * 4;
                break;
            }
        case DTL_TYPE_INTERVAL_DAY_TO_SECOND :
            {
                sPrecision += DTL_INTERVAL_DAY_TO_SECOND_SIZE * 4;
                break;
            }
        case DTL_TYPE_ROWID :
            {
                sPrecision += DTL_ROWID_SIZE * 4;
                break;
            }
        default :
            {
                STL_DASSERT( 0 );
                break;
            }
    }

    STL_DASSERT( sPrecision <= DTL_DUMP_FUNC_TYPE_INFO_SIZE + (DTL_VARCHAR_MAX_PRECISION * 16) );
    
    if( sPrecision > DTL_VARCHAR_MAX_PRECISION )
    {
        sPrecision = DTL_VARCHAR_MAX_PRECISION;
    }
    else
    {
        /* Do Nothing */
    }
    

    /**
     * output 설정 
     */

    aFuncArgDataTypeArray[0] = aDataTypeArray[ 0 ];

    aFuncArgDataTypeDefInfoArray[ 0 ].mArgNum1 =
        aDataTypeDefInfoArray[ 0 ].mArgNum1;
    aFuncArgDataTypeDefInfoArray[ 0 ].mArgNum2 =
        aDataTypeDefInfoArray[ 0 ].mArgNum2;
    aFuncArgDataTypeDefInfoArray[ 0 ].mStringLengthUnit =
        aDataTypeDefInfoArray[ 0 ].mStringLengthUnit;
    aFuncArgDataTypeDefInfoArray[ 0 ].mIntervalIndicator =
        aDataTypeDefInfoArray[ 0 ].mIntervalIndicator;
    
    *aFuncResultDataType = DTL_TYPE_VARCHAR;
    
    aFuncResultDataTypeDefInfo->mArgNum1 = sPrecision;
    aFuncResultDataTypeDefInfo->mArgNum2 = gResultDataTypeDef[ DTL_TYPE_VARCHAR ].mArgNum2;
    aFuncResultDataTypeDefInfo->mStringLengthUnit =
        DTL_STRING_LENGTH_UNIT_OCTETS;
    aFuncResultDataTypeDefInfo->mIntervalIndicator =
        gResultDataTypeDef[ DTL_TYPE_VARCHAR ].mIntervalIndicator;
    
    *aFuncPtrIdx = 0;

    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;   
}

/**
 * @brief DUMP function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus dtlGetFuncPtrDump( stlUInt32             aFuncPtrIdx,
                             dtlBuiltInFuncPtr   * aFuncPtr,
                             stlErrorStack       * aErrorStack )
{
    /**
     * output 설정
     */

    *aFuncPtr = dtlDump;
    
    return STL_SUCCESS;
}


stlStatus dtlDump( stlUInt16        aInputArgumentCnt,
                   dtlValueEntry  * aInputArgument,
                   void           * aResultValue,
                   void           * aInfo,
                   dtlFuncVector  * aVectorFunc,
                   void           * aVectorArg,
                   void           * aEnv )
{
    STL_DASSERT( aInputArgument[0].mValue.mDataValue->mLength > 0 );
    
    return gDtfDumpFunc[ aInputArgument[0].mValue.mDataValue->mType ]( aInputArgumentCnt,
                                                                       aInputArgument,
                                                                       aResultValue,
                                                                       aEnv );
}

/** @} dtlFuncDump */
