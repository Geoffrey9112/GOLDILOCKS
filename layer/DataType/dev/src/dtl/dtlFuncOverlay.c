/*******************************************************************************
 * dtlFuncOverlay.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtlFuncOverlay.c 1841 2011-09-07 07:14:06Z jhkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file dtlFuncOverlay.c
 * @brief DataType Layer Function관련 함수 
 */

#include <dtl.h>
#include <dtDef.h>
#include <dtlFuncCommon.h>
#include <dtfString.h>

/**
 * @ingroup dtlFunction Function관련 함수
 * @{
 */


/*
 * 대표 함수의 정보를 정의한다.
 * mValidArgList는 함수를 결정하는데 사용되는 arg index의 list이다.
 * 함수를 결정하는데 사용되지 않는 arg는 무조건 함수의 원형 type으로
 * 리턴한다.
 */
const dtlFuncInfo dtlFuncOverlayInfo =
{
    { STL_TRUE, STL_TRUE, }      /* 함수를 결정하는데 판단이 되는 argument index list */
};


/*
 * 각 함수별 arg type, result type을 정의한다.
 * 주의: 빠르게 처리 가능한 함수를 먼저 정의해야 한다.
 */
const dtlFuncArgInfo dtlFuncOverlayList[] =
{
     { dtlStringOverlay,
       { DTL_TYPE_VARCHAR,
         DTL_TYPE_VARCHAR,
         DTL_TYPE_NATIVE_INTEGER,
         DTL_TYPE_NATIVE_INTEGER,
         DTL_TYPE_NATIVE_INTEGER },
       DTL_TYPE_VARCHAR
     },
     { dtlLongvarcharStringOverlay,
       { DTL_TYPE_LONGVARCHAR,
         DTL_TYPE_LONGVARCHAR,
         DTL_TYPE_NATIVE_INTEGER,
         DTL_TYPE_NATIVE_INTEGER,
         DTL_TYPE_NATIVE_INTEGER },
       DTL_TYPE_LONGVARCHAR
     },
     { dtlBinaryStringOverlay,
       { DTL_TYPE_VARBINARY,
         DTL_TYPE_VARBINARY,
         DTL_TYPE_NATIVE_INTEGER,
         DTL_TYPE_NATIVE_INTEGER,
         DTL_TYPE_NATIVE_INTEGER },
       DTL_TYPE_VARBINARY
     },
     { dtlLongvarbinaryStringOverlay,
       { DTL_TYPE_LONGVARBINARY,
         DTL_TYPE_LONGVARBINARY,
         DTL_TYPE_NATIVE_INTEGER,
         DTL_TYPE_NATIVE_INTEGER,
         DTL_TYPE_NATIVE_INTEGER },
       DTL_TYPE_LONGVARBINARY
     },


     { NULL,
       { DTL_TYPE_NA, },
       DTL_TYPE_NA
     }
};


stlStatus dtlStringOverlay( stlUInt16        aInputArgumentCnt,
                            dtlValueEntry  * aInputArgument,
                            void           * aResultValue,
                            void           * aInfo,
                            dtlFuncVector  * aVectorFunc,
                            void           * aVectorArg,
                            void           * aEnv )
{
    return dtfStringOverlay( aInputArgumentCnt,
                             aInputArgument,
                             aResultValue,
                             aVectorFunc,
                             aVectorArg,
                             aEnv );
}

stlStatus dtlLongvarcharStringOverlay( stlUInt16        aInputArgumentCnt,
                                       dtlValueEntry  * aInputArgument,
                                       void           * aResultValue,
                                       void           * aInfo,
                                       dtlFuncVector  * aVectorFunc,
                                       void           * aVectorArg,
                                       void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sValue2;
    dtlDataValue  * sResultValue;
    stlInt64        sResultLength;
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    /*
     * overlay 결과의 최대 길이는 다음과 같다.
     *
     * 예) create table t1( i1 char(5), i2 char(10) );
     *     insert into t1 values ( 'abcde', '1234567890' );
     *     select overlay( I1 placing i2 from length(i1) for (-length(i1)) ) from t1;
     */
    sResultLength = sValue1->mLength + sValue2->mLength + sValue1->mLength;
    sResultLength = STL_MIN( sResultLength, DTL_LONGVARCHAR_MAX_PRECISION );
    
    if( sResultLength > sResultValue->mBufferSize )
    {
        STL_TRY( aVectorFunc->mReallocLongVaryingMemFunc( aVectorArg,
                                                          sResultLength,
                                                          &sResultValue->mValue,
                                                          (stlErrorStack *)aEnv )
                 == STL_SUCCESS );
        
        sResultValue->mBufferSize = sResultLength;
    }
    
    STL_TRY( dtfLongvarcharStringOverlay( aInputArgumentCnt,
                                          aInputArgument,
                                          aResultValue,
                                          aVectorFunc,
                                          aVectorArg,
                                          aEnv )
             == STL_SUCCESS );

    STL_DASSERT( sResultValue->mLength <= sResultValue->mBufferSize );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus dtlBinaryStringOverlay( stlUInt16        aInputArgumentCnt,
                                  dtlValueEntry  * aInputArgument,
                                  void           * aResultValue,
                                  void           * aInfo,
                                  dtlFuncVector  * aVectorFunc,
                                  void           * aVectorArg,
                                  void           * aEnv )
{
    return dtfBinaryStringOverlay( aInputArgumentCnt,
                                   aInputArgument,
                                   DTL_VARBINARY_MAX_PRECISION,
                                   aResultValue,
                                   aEnv );
}

stlStatus dtlLongvarbinaryStringOverlay( stlUInt16        aInputArgumentCnt,
                                         dtlValueEntry  * aInputArgument,
                                         void           * aResultValue,
                                         void           * aInfo,
                                         dtlFuncVector  * aVectorFunc,
                                         void           * aVectorArg,
                                         void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sValue2;
    dtlDataValue  * sResultValue;
    stlInt64        sResultLength;
    
    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    /*
     * overlay 결과의 최대 길이는 다음과 같다.
     *
     * 예) create table t1( i1 binary(5), i2 binary(10) );
     *     insert into t1 values ( X'AABBCCDDEE', '11223344556677889900' );
     *     select overlay( I1 placing i2 from lengthb(i1) for (-lengthb(i1)) ) from t1;
     */
    sResultLength = sValue1->mLength + sValue2->mLength + sValue1->mLength;    
    sResultLength = STL_MIN( sResultLength, DTL_LONGVARBINARY_MAX_PRECISION );
    
    if( sResultLength > sResultValue->mBufferSize )
    {
        STL_TRY( aVectorFunc->mReallocLongVaryingMemFunc( aVectorArg,
                                                          sResultLength,
                                                          &sResultValue->mValue,
                                                          (stlErrorStack *)aEnv )
                 == STL_SUCCESS );
        
        sResultValue->mBufferSize = sResultLength;
    }
    
    STL_TRY( dtfBinaryStringOverlay( aInputArgumentCnt,
                                     aInputArgument,
                                     DTL_LONGVARBINARY_MAX_PRECISION,
                                     aResultValue,
                                     aEnv )
             == STL_SUCCESS );

    STL_DASSERT( sResultValue->mLength <= sResultValue->mBufferSize );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Overlay function에 대한 정보 얻기
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
stlStatus dtlGetFuncInfoOverlay( stlUInt16               aDataTypeArrayCount,
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
    stlInt64   sMaxPrecision = 0;
    
    STL_TRY( dtlGetFuncInfoCommon( &dtlFuncOverlayInfo,
                                   dtlFuncOverlayList,
                                   aDataTypeArrayCount,
                                   aIsConstantData,
                                   aDataTypeArray,
                                   aDataTypeDefInfoArray,
                                   aFuncArgDataTypeArrayCount,
                                   aFuncArgDataTypeArray,
                                   aFuncArgDataTypeDefInfoArray,
                                   aFuncResultDataType,
                                   aFuncResultDataTypeDefInfo,
                                   aFuncPtrIdx,
                                   aErrorStack )
             == STL_SUCCESS );
    
    /*
     * result type 에 대한 info 정보 수정.
     */
    
    /*
     * OVERLAY( str1 PLACING str2 FROM start_position [ FOR string_length ] )
     *         ^^^^^         ^^^^^
     *
     * result type의 최대 precision = (str1 precision) + (str2 precision)
     */
    
    if( *aFuncResultDataType == DTL_TYPE_VARCHAR )
    {
        /*
         * string length unit 결정
         */
        
        if( ( aFuncArgDataTypeDefInfoArray[0].mStringLengthUnit == DTL_STRING_LENGTH_UNIT_CHARACTERS ) ||
            ( aFuncArgDataTypeDefInfoArray[1].mStringLengthUnit == DTL_STRING_LENGTH_UNIT_CHARACTERS ) )
        {
            aFuncResultDataTypeDefInfo->mStringLengthUnit = DTL_STRING_LENGTH_UNIT_CHARACTERS;
        }
        else
        {
            aFuncResultDataTypeDefInfo->mStringLengthUnit = DTL_STRING_LENGTH_UNIT_OCTETS;
        }
    }
    else
    {
        /* Do Nothing */
    }

    /*
     * mArgNum1 ( precision ) 결정
     */

    /*
     * overlay 결과의 최대 길이는 다음과 같다.
     *
     * 예) create table t1( i1 char(5), i2 char(10) );
     *     insert into t1 values ( 'abcde', '1234567890' );
     *     select overlay( I1 placing i2 from length(i1) for (-length(i1)) ) from t1;
     */
    aFuncResultDataTypeDefInfo->mArgNum1 =
        aFuncArgDataTypeDefInfoArray[0].mArgNum1
        + aFuncArgDataTypeDefInfoArray[1].mArgNum1 + aFuncArgDataTypeDefInfoArray[0].mArgNum1;
    
    /*
     * 결과 precision 조정.
     *
     * : precision 초과되는 경우 , max precision 으로 조정.
     */

    if( *aFuncResultDataType == DTL_TYPE_VARCHAR )
    {
        sMaxPrecision = DTL_VARCHAR_MAX_PRECISION;
    }
    else if( *aFuncResultDataType == DTL_TYPE_LONGVARCHAR )
    {
        sMaxPrecision = DTL_LONGVARCHAR_MAX_PRECISION;
    }
    else if( *aFuncResultDataType == DTL_TYPE_VARBINARY )
    {
        sMaxPrecision = DTL_VARBINARY_MAX_PRECISION;
    }
    else if( *aFuncResultDataType == DTL_TYPE_LONGVARBINARY )
    {
        sMaxPrecision = DTL_LONGVARBINARY_MAX_PRECISION;
    }
    else
    {
        STL_DASSERT( STL_FALSE );
    }
    
    if( aFuncResultDataTypeDefInfo->mArgNum1 > sMaxPrecision )
    {
        aFuncResultDataTypeDefInfo->mArgNum1 = sMaxPrecision;
    }
    else
    {
        /* Do Nothing */
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Overlay function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus dtlGetFuncPtrOverlay( stlUInt32             aFuncPtrIdx,
                                dtlBuiltInFuncPtr   * aFuncPtr,
                                stlErrorStack       * aErrorStack )
{
    /**
     * output 설정
     */

    /*
     * dtlOverlay가 n차원 배열임으로 1차원 배열로 typecast해서
     * aFuncPtrIdx를 적용한다.
     */
    *aFuncPtr = dtlFuncOverlayList[aFuncPtrIdx].mFuncPtr;

    return STL_SUCCESS;
}



/** @} */
