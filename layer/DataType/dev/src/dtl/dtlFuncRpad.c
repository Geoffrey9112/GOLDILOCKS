/*******************************************************************************
 * dtlFuncRpad.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *
 ******************************************************************************/

/**
 * @file dtlFuncRpad.c
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
const dtlFuncInfo dtlFuncRpadInfo =
{
    { STL_TRUE } /* 함수를 결정하는데 판단이 되는 argument cnt */
};


/*
 * 각 함수별 arg type, result type을 정의한다.
 * 주의: 빠르게 처리 가능한 함수를 먼저 정의해야 한다.
 */
const dtlFuncArgInfo dtlFuncRpadList[] =
{
     { dtlStringRpad,
       { DTL_TYPE_VARCHAR, DTL_TYPE_NATIVE_INTEGER, DTL_TYPE_VARCHAR },
       DTL_TYPE_VARCHAR
     },
     { dtlLongvarcharStringRpad,
       { DTL_TYPE_LONGVARCHAR, DTL_TYPE_NATIVE_INTEGER, DTL_TYPE_LONGVARCHAR },
       DTL_TYPE_LONGVARCHAR
     },
     { dtlBinaryRpad,
       { DTL_TYPE_VARBINARY, DTL_TYPE_NATIVE_INTEGER, DTL_TYPE_VARBINARY },
       DTL_TYPE_VARBINARY
     },
     { dtlLongvarbinaryRpad,
       { DTL_TYPE_LONGVARBINARY, DTL_TYPE_NATIVE_INTEGER, DTL_TYPE_LONGVARBINARY },
       DTL_TYPE_LONGVARBINARY
     },
     
     { NULL,
       { DTL_TYPE_NA, },
       DTL_TYPE_NA
     }
};


stlStatus dtlStringRpad( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aInfo,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv )
{
    return dtfStringRpad( aInputArgumentCnt,
                          aInputArgument,
                          aResultValue,
                          aVectorFunc,
                          aVectorArg,
                          aEnv );
}

stlStatus dtlLongvarcharStringRpad( stlUInt16        aInputArgumentCnt,
                                    dtlValueEntry  * aInputArgument,
                                    void           * aResultValue,
                                    void           * aInfo,
                                    dtlFuncVector  * aVectorFunc,
                                    void           * aVectorArg,
                                    void           * aEnv )
{
    dtlDataValue  * sValue;
    dtlDataValue  * sResultValue;
    stlInt64        sTotalLen;
    
    sValue = aInputArgument[1].mValue.mDataValue;
    
    sTotalLen    = DTF_INTEGER( sValue );
    sResultValue = (dtlDataValue *)aResultValue;

    sTotalLen *= dtlGetMbMaxLength((aVectorFunc)->mGetCharSetIDFunc(aVectorArg));
    sTotalLen = STL_MIN( sTotalLen, DTL_LONGVARCHAR_MAX_PRECISION );
    
    if( sTotalLen > sResultValue->mBufferSize )
    {
        STL_TRY( aVectorFunc->mReallocLongVaryingMemFunc( aVectorArg,
                                                          sTotalLen,
                                                          &sResultValue->mValue,
                                                          (stlErrorStack *)aEnv )
                 == STL_SUCCESS );
        
        sResultValue->mBufferSize = sTotalLen;
    }
    
    STL_TRY( dtfLongvarcharStringRpad( aInputArgumentCnt,
                                       aInputArgument,
                                       aResultValue,
                                       aVectorFunc,
                                       aVectorArg,
                                       aEnv )
             == STL_SUCCESS );
    
    STL_DASSERT( sResultValue->mBufferSize >= sResultValue->mLength );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus dtlBinaryRpad( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aInfo,
                         dtlFuncVector  * aVectorFunc,
                         void           * aVectorArg,
                         void           * aEnv )
{
    return dtfBinaryRpad( aInputArgumentCnt,
                          aInputArgument,
                          DTL_VARBINARY_MAX_PRECISION,
                          aResultValue,
                          aVectorFunc,
                          aVectorArg,
                          aEnv );
}

stlStatus dtlLongvarbinaryRpad( stlUInt16        aInputArgumentCnt,
                                dtlValueEntry  * aInputArgument,
                                void           * aResultValue,
                                void           * aInfo,
                                dtlFuncVector  * aVectorFunc,
                                void           * aVectorArg,
                                void           * aEnv )
{
    dtlDataValue * sValue;
    dtlDataValue * sResultValue;
    stlInt64       sTotalLen;
    
    sValue = aInputArgument[1].mValue.mDataValue;
    
    sTotalLen    = DTF_INTEGER( sValue );
    sResultValue = (dtlDataValue *)aResultValue;

    sTotalLen = STL_MIN( sTotalLen, DTL_LONGVARBINARY_MAX_PRECISION );

    if( sTotalLen > sResultValue->mBufferSize )
    {
        STL_TRY( aVectorFunc->mReallocLongVaryingMemFunc( aVectorArg,
                                                          sTotalLen,
                                                          &sResultValue->mValue,
                                                          (stlErrorStack *)aEnv )
                 == STL_SUCCESS );
        
        sResultValue->mBufferSize = sTotalLen;
    }
    
    STL_TRY( dtfBinaryRpad( aInputArgumentCnt,
                            aInputArgument,
                            DTL_LONGVARBINARY_MAX_PRECISION,
                            aResultValue,
                            aVectorFunc,
                            aVectorArg,
                            aEnv )
             == STL_SUCCESS );
    
    STL_DASSERT( sResultValue->mBufferSize >= sResultValue->mLength );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Rpad function에 대한 정보 얻기
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
stlStatus dtlGetFuncInfoRpad( stlUInt16               aDataTypeArrayCount,
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
    STL_TRY( dtlGetFuncInfoCommon( &dtlFuncRpadInfo,
                                   dtlFuncRpadList,
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

    /**
     * @todo  
     * result type 에 대한 info 정보 수정.
     *
     * RPAD( str, length int, [, fill] )
     *           ^^^^^^^^^^^^
     *           
     * ex) Rpad( 'aa',5,'b' )  =>  aabbb
     *
     * 두번째 인자인 length int 값을 알 수 있어야 가능하다.
     */    

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Rpad function pointer 얻기
 * @param[in]  aFuncPtrIdx       function pointer 얻기 위한 식별자
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus dtlGetFuncPtrRpad( stlUInt32             aFuncPtrIdx,
                             dtlBuiltInFuncPtr   * aFuncPtr,
                             stlErrorStack       * aErrorStack )
{
    /**
     * output 설정
     */

    /*
     * dtlRpad가 n차원 배열임으로 1차원 배열로 typecast해서
     * aFuncPtrIdx를 적용한다.
     */
    *aFuncPtr = dtlFuncRpadList[aFuncPtrIdx].mFuncPtr;

    return STL_SUCCESS;
}



/** @} */
