/*******************************************************************************
 * knlFunction.c
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
 * @file knlFunction.c
 * @brief Kernel Library Layer Function 
 */

#include <knl.h>


/**
 * @ingroup knlFunction
 * @{
 */

/*******************************************************************************
 * GET FUNCTION POINTER 
 ******************************************************************************/

stlStatus knlGetListFuncPtrInvalid( knlBuiltInListFuncPtr   * aFuncPtr,
                                    stlErrorStack           * aErrorStack )
{
    /**
     * output 설정
     */

    *aFuncPtr = knlInvalidListFunc;

    
    return STL_SUCCESS;
}

stlStatus knlGetListFuncPtrNotSupport( knlBuiltInListFuncPtr   * aFuncPtr,
                                       stlErrorStack           * aErrorStack )
{
    /**
     * output 설정
     */

    *aFuncPtr = knlNotSupportListFunc;

    
    return STL_SUCCESS;
}



/*************************************************
 * ASSIGN FUNCTION
 ************************************************/

/**
 * @brief ASSIGN function          
 * @param[in]  aDataTypeArrayCount          aDataTypeArray    
 * @param[in]  aIsConstantData              constant value     
 * @param[in]  aDataTypeArray               function data type
 * @param[in]  aDataTypeDefInfoArray        aDataTypeArray (dtlDataTypeDefInfo)
 * @param[in]  aFuncArgDataTypeArrayCount   function input argument count
 * @param[out] aFuncArgDataTypeArray        function argument type
 * @param[out] aFuncArgDataTypeDefInfoArray aFuncArgDataTypeArray (dtlDataTypeDefInfo)
 * @param[out] aFuncResultDataType          function return type
 * @param[out] aFuncResultDataTypeDefInfo   aFuncResultDataType (dtlDataTypeDefInfo)
 * @param[out] aFuncPtrIdx                  function pointer            
 * @param[in]  aVectorFunc                  Function Vector
 * @param[in]  aVectorArg                   Vector Argument
 * @param[out] aErrorStack
 */
stlStatus knlGetFuncInfoAssign( stlUInt16               aDataTypeArrayCount,
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
    /*
     * parameter validation
     */

    STL_PARAM_VALIDATE( aDataTypeArrayCount == 2, aErrorStack );
    STL_PARAM_VALIDATE( aFuncArgDataTypeArrayCount == 2, aErrorStack );
    

    /**
     * output    
     */

    aFuncArgDataTypeArray[ 0 ] = aDataTypeArray[ 1 ];

    aFuncArgDataTypeDefInfoArray[ 0 ].mArgNum1 =
        aDataTypeDefInfoArray[ 1 ].mArgNum1;
    aFuncArgDataTypeDefInfoArray[ 0 ].mArgNum2 =
        aDataTypeDefInfoArray[ 1 ].mArgNum2;
    aFuncArgDataTypeDefInfoArray[ 0 ].mStringLengthUnit =
        aDataTypeDefInfoArray[ 1 ].mStringLengthUnit;
    aFuncArgDataTypeDefInfoArray[ 0 ].mIntervalIndicator =
        aDataTypeDefInfoArray[ 1 ].mIntervalIndicator;
    
    aFuncArgDataTypeArray[ 1 ] = aDataTypeArray[ 1 ];

    aFuncArgDataTypeDefInfoArray[ 1 ].mArgNum1 =
        aDataTypeDefInfoArray[ 1 ].mArgNum1;
    aFuncArgDataTypeDefInfoArray[ 1 ].mArgNum2 =
        aDataTypeDefInfoArray[ 1 ].mArgNum2;
    aFuncArgDataTypeDefInfoArray[ 1 ].mStringLengthUnit =
        aDataTypeDefInfoArray[ 1 ].mStringLengthUnit;
    aFuncArgDataTypeDefInfoArray[ 1 ].mIntervalIndicator =
        aDataTypeDefInfoArray[ 1 ].mIntervalIndicator;

    *aFuncResultDataType = aDataTypeArray[ 1 ];
    
    aFuncResultDataTypeDefInfo->mArgNum1 = aDataTypeDefInfoArray[ 1 ].mArgNum1;
    aFuncResultDataTypeDefInfo->mArgNum2 = aDataTypeDefInfoArray[ 1 ].mArgNum2;
    aFuncResultDataTypeDefInfo->mStringLengthUnit =
        aDataTypeDefInfoArray[ 1 ].mStringLengthUnit;
    aFuncResultDataTypeDefInfo->mIntervalIndicator =
        aDataTypeDefInfoArray[ 1 ].mIntervalIndicator;
    
    *aFuncPtrIdx = 0;

    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;   
}


/**
 * @brief ASSIGN function pointer   
 * @param[in]  aFuncPtrIdx       function pointer          
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack            
 */
stlStatus knlGetFuncPtrAssign( stlUInt32             aFuncPtrIdx,
                               dtlBuiltInFuncPtr   * aFuncPtr,
                               stlErrorStack       * aErrorStack )
{
    /**
     * output   
     */

    *aFuncPtr = knlAssign;
    
    return STL_SUCCESS;
}


stlStatus knlAssign( stlUInt16        aInputArgumentCnt,
                     dtlValueEntry  * aInputArgument,
                     void           * aResultValue,
                     void           * aInfo,
                     dtlFuncVector  * aVectorFunc,
                     void           * aVectorArg,
                     void           * aEnv )
{
    dtlDataValue  * sResultValue;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );

    sResultValue = (dtlDataValue *)aResultValue;

    if( aInputArgument[0].mValue.mDataValue->mLength == 0 )
    {
        aInputArgument[1].mValue.mDataValue->mLength = 0;
        sResultValue->mLength = 0;
    }
    else
    {
        STL_TRY( knlCopyDataValue( aInputArgument[0].mValue.mDataValue,
                                   aInputArgument[1].mValue.mDataValue,
                                   aEnv )
                 == STL_SUCCESS );

        if( ( sResultValue->mType == DTL_TYPE_LONGVARCHAR ) ||
            ( sResultValue->mType == DTL_TYPE_LONGVARBINARY ) )
        {
            STL_TRY( knlCopyDataValue( aInputArgument[1].mValue.mDataValue,
                                       sResultValue,
                                       aEnv )
                     == STL_SUCCESS );
        }
        else
        {
            sResultValue->mLength = aInputArgument[1].mValue.mDataValue->mLength;
            sResultValue->mValue = aInputArgument[1].mValue.mDataValue->mValue;
        }
    }

    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/*************************************************
 * LIST FUNCTION
 ************************************************/
/*******************************************************************************
 * INVALID FUNCTION 
 ******************************************************************************/

stlStatus knlInvalidListFunc( stlUInt16              aInputArgumentCnt,
                              stlUInt16              aRowValueCnt,
                              knlListEntry         * aInputArgument,
                              knlExprContextInfo   * aContextInfo,
                              stlInt32               aBlockIdx,
                              stlBool              * aIsResult,
                              void                 * aResultValue,
                              void                 * aEnv )
{
    *aIsResult = STL_FALSE;
    
    DTL_BOOLEAN( aResultValue ) = STL_FALSE;
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    
    return STL_SUCCESS;
}

stlStatus knlNotSupportListFunc( stlUInt16              aInputArgumentCnt,
                                 stlUInt16              aRowValueCnt,
                                 knlListEntry         * aInputArgument,
                                 knlExprContextInfo   * aContextInfo,
                                 stlInt32               aBlockIdx,
                                 stlBool              * aIsResult,
                                 void                 * aResultValue,
                                 void                 * aEnv )
{
    *aIsResult = STL_FALSE;

    STL_THROW( RAMP_ERROR_NOT_IMPLEMENTED );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERROR_NOT_IMPLEMENTED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_NOT_IMPLEMENTED,
                      NULL,
                      aEnv );
    }

    STL_FINISH;
    return STL_FAILURE;
}

/************************************************
 * IN FUNCTION
 ************************************************/
/*******************************************************************************************
 * IN FUNCTION
 * GET FUNCTION POINTER 
 ******************************************************************************************/

/**
 * @brief Single Row Only Value In function pointer 얻기
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus knlGetListFuncPtrSingleInOnlyValue( knlBuiltInListFuncPtr   * aFuncPtr,
                                              stlErrorStack           * aErrorStack )
{
    /**
     * output 설정
     */
    
     *aFuncPtr = knlSingleInOnlyValue;
    
    return STL_SUCCESS;
}


/**
 * @brief Single Row All Expr In function pointer 얻기
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus knlGetListFuncPtrSingleIn( knlBuiltInListFuncPtr   * aFuncPtr,
                                     stlErrorStack           * aErrorStack )
{
    /**
     * output 설정
     */
    
     *aFuncPtr = knlSingleIn;
    
    return STL_SUCCESS;
}

/**
 * @brief Multi Row Only Value In function pointer 얻기
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus knlGetListFuncPtrMultiInOnlyValue( knlBuiltInListFuncPtr   * aFuncPtr,
                                             stlErrorStack           * aErrorStack )
{
    /**
     * output 설정
     */
    
     *aFuncPtr = knlMultiInOnlyValue;
    
    return STL_SUCCESS;
}

/**
 * @brief Multi Row All Expr In function pointer 얻기
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus knlGetListFuncPtrMultiIn( knlBuiltInListFuncPtr   * aFuncPtr,
                                    stlErrorStack           * aErrorStack )
{
    /**
     * output 설정
     */
    
     *aFuncPtr = knlMultiIn;
    
    return STL_SUCCESS;
}


/*******************************************************************************************
 * IN FUNCTION
 * FUNCTION  
 ******************************************************************************************/
/**
 * @brief In
 *        <BR> expr In ( expr, expr )
 *        <BR>   In => [ P, O, M, S ]
 *        <BR> ex) 10 In (10, 11, 'a' )  =>  true
 *                                      <BR> EXPR IN ( VALUE, VALUE, VALUE ... )
 * @param[in]  aInputArgumentCnt        input argument count
 * @param[in]  aRowValueCnt             Row Value Count
 * @param[in]  aInputArgument           input argument
 * @param[in]  aContextInfo             Context info
 * @param[in]  aBlockIdx                EvalInfo Block Idx
 * @param[out] aIsResult                List Function Result
 * @param[out] aResultValue             연산 결과 (결과타입 BOOLEAN)
 * @param[out] aEnv                     environment (stlErrorStack)
 */
stlStatus knlSingleInOnlyValue( stlUInt16              aInputArgumentCnt,
                                stlUInt16              aRowValueCnt,
                                knlListEntry         * aInputArgument,
                                knlExprContextInfo   * aContextInfo,
                                stlInt32               aBlockIdx,
                                stlBool              * aIsResult,
                                void                 * aResultValue,
                                void                 * aEnv )
{
    dtlDataValue       * sLeft;
    dtlDataValue       * sRight;
    knlExprContext     * sLeftContext  = NULL;
    knlExprContext     * sRightContext = NULL;
    knlExprContext     * sLeftCast     = NULL;
    stlBool              sIsNull       = STL_FALSE;
    stlInt32             sIdx          = 0;
    stlBool              sResultValue  = STL_FALSE;
    
    STL_PARAM_VALIDATE( aInputArgumentCnt <= KNL_LIST_FUNC_MAX_VALUE_CNT, KNL_ERROR_STACK( aEnv ) );

    while( sIdx < aInputArgumentCnt )
    {
        sLeftContext =
            & aContextInfo->mContexts[ aInputArgument[sIdx].mOffset ];
        
        if( sLeftContext->mCast == NULL )
        {
            sLeft =
                KNL_GET_VALUE_BLOCK_DATA_VALUE( sLeftContext->mInfo.mValueInfo,
                                                aBlockIdx );
        }
        else
        {
            sLeftContext->mCast->mArgs[0].mValue.mDataValue =
                KNL_GET_VALUE_BLOCK_DATA_VALUE( sLeftContext->mInfo.mValueInfo,
                                                aBlockIdx );
                
            if( sLeftCast != NULL )
            {
                if( sLeftContext->mCast->mCastFuncPtr == sLeftCast->mCast->mCastFuncPtr )
                {
                    sLeft = & sLeftCast->mCast->mCastResultBuf;
                    STL_THROW( RAMP_EXIT_CAST );
                }
            }

            sLeftCast = sLeftContext;                
                
            if( DTL_IS_NULL( sLeftContext->mCast->mArgs[0].mValue.mDataValue ) )
            {
                DTL_SET_NULL( & sLeftContext->mCast->mCastResultBuf );
            }
            else
            {
                STL_TRY( sLeftContext->mCast->mCastFuncPtr(
                             DTL_CAST_INPUT_ARG_CNT,
                             sLeftContext->mCast->mArgs,
                             (void *) & sLeftContext->mCast->mCastResultBuf,
                             (void *)(sLeftContext->mCast->mSrcTypeInfo),
                             KNL_DT_VECTOR(aEnv),
                             aEnv,
                             KNL_ERROR_STACK( aEnv ) )
                         == STL_SUCCESS );
            }
            
            sLeft =
                & sLeftContext->mCast->mCastResultBuf;
        }
        
        STL_RAMP( RAMP_EXIT_CAST );
        sIdx ++;
        
        sRightContext =
            & aContextInfo->mContexts[ aInputArgument[sIdx].mOffset ];

        sRight =
            KNL_GET_VALUE_BLOCK_DATA_VALUE( sRightContext->mInfo.mValueInfo,
                                            aBlockIdx );
                             
        if( sRightContext->mCast == NULL )
        {
            /* Do Nothing */
        }
        else
        {
            if( DTL_IS_NULL( sRight ) )
            {
                DTL_SET_NULL( & sRightContext->mCast->mCastResultBuf );
            }
            else
            {
                sRightContext->mCast->mArgs[0].mValue.mDataValue =
                    sRight;

                STL_TRY( sRightContext->mCast->mCastFuncPtr(
                             DTL_CAST_INPUT_ARG_CNT,
                             sRightContext->mCast->mArgs,
                             (void *) & sRightContext->mCast->mCastResultBuf,
                             (void *)(sRightContext->mCast->mSrcTypeInfo),
                             KNL_DT_VECTOR(aEnv),
                             aEnv,
                             KNL_ERROR_STACK( aEnv ) )
                         == STL_SUCCESS );
            }
            
            sRight =
                & sRightContext->mCast->mCastResultBuf;
        }

        sIdx ++;
        
        if( DTL_IS_NULL( sLeft ) || DTL_IS_NULL( sRight ) )
        {
            sIsNull = STL_TRUE;
        }
        else
        {
            sResultValue =
                ( dtlPhysicalFuncArg2FuncList[ sLeft->mType ][ sRight->mType ]
                  [ DTL_PHYSICAL_FUNC_IS_EQUAL ]
                  ( sLeft->mValue,
                    sLeft->mLength,
                    sRight->mValue,
                    sRight->mLength ) );

            if( sResultValue == STL_TRUE )
            {
                *aIsResult = STL_TRUE;
                DTL_BOOLEAN( aResultValue ) = STL_TRUE;
                DTL_SET_BOOLEAN_LENGTH( aResultValue );

                STL_THROW( RAMP_LIST_EXIT ); 
            }      
        }
     }
    *aIsResult = STL_FALSE;
     if( sIsNull == STL_TRUE )
     {
         DTL_SET_NULL( (dtlBoolean*)aResultValue );
     }
     else
     {
         DTL_BOOLEAN( aResultValue ) = STL_FALSE;
         DTL_SET_BOOLEAN_LENGTH( aResultValue );
     }

     STL_RAMP( RAMP_LIST_EXIT );
     
     return STL_SUCCESS;

     STL_FINISH;

     return STL_FAILURE;
}

/**
 * @brief In
 *        <BR> expr In ( expr, expr )
 *        <BR>   In => [ P, O, M, S ]
 *        <BR> ex) 10 In (10, 11, 'a' )  =>  true
 *                                      <BR> EXPR IN ( VALUE, VALUE, VALUE ... )
 * @param[in]  aInputArgumentCnt        input argument count
 * @param[in]  aRowValueCnt             Row Value Count
 * @param[in]  aInputArgument           input argument
 * @param[in]  aContextInfo             Context info
 * @param[in]  aBlockIdx                EvalInfo Block Idx
 * @param[out] aIsResult                List Function Result
 * @param[out] aResultValue             연산 결과 (결과타입 BOOLEAN)
 * @param[out] aEnv                     environment (stlErrorStack)
 */
stlStatus knlSingleIn( stlUInt16              aInputArgumentCnt,
                       stlUInt16              aRowValueCnt,
                       knlListEntry         * aInputArgument,
                       knlExprContextInfo   * aContextInfo,
                       stlInt32               aBlockIdx,
                       stlBool              * aIsResult,
                       void                 * aResultValue,
                       void                 * aEnv )
{
    dtlDataValue       * sLeft;
    dtlDataValue       * sRight;
    knlExprContext     * sListContext = NULL;
    knlExprContext     * sLeftCast    = NULL;
    stlBool              sIsNull;
    stlInt32             sIdx         = 0;
    stlBool              sResultValue = STL_FALSE;
    

    STL_PARAM_VALIDATE( aInputArgumentCnt <= KNL_LIST_FUNC_MAX_VALUE_CNT, KNL_ERROR_STACK( aEnv ) );

    sIsNull = (  DTL_IS_NULL( (dtlBoolean*)aResultValue ) ) ? STL_TRUE : STL_FALSE;

    while( sIdx < aInputArgumentCnt )
    {
        sListContext =
            & aContextInfo->mContexts[ aInputArgument[sIdx].mOffset ];

        KNL_LIST_FUNC_GET_LEFT_VALUE( sLeft,
                                      aInputArgument[sIdx].mType,
                                      sListContext,
                                      sLeftCast,
                                      aBlockIdx,
                                      aEnv );
        sListContext =
            & aContextInfo->mContexts[ aInputArgument[sIdx + 1].mOffset ];

        KNL_LIST_FUNC_GET_RIGHT_VALUE( sRight,
                                       aInputArgument[sIdx + 1].mType,
                                       sListContext,
                                       aBlockIdx,
                                       aEnv );               
        
        sIdx += 2;
        
        if( DTL_IS_NULL( sLeft ) || DTL_IS_NULL( sRight ) )
        {
            sIsNull = STL_TRUE;
        }
        else
        {
            sResultValue =
                ( dtlPhysicalFuncArg2FuncList[ sLeft->mType ][ sRight->mType ]
                  [ DTL_PHYSICAL_FUNC_IS_EQUAL ]
                  ( sLeft->mValue,
                    sLeft->mLength,
                    sRight->mValue,
                    sRight->mLength ) );

            if( sResultValue == STL_TRUE )
            {
                *aIsResult = STL_TRUE;
                DTL_BOOLEAN( aResultValue ) = STL_TRUE;
                DTL_SET_BOOLEAN_LENGTH( aResultValue );

                STL_THROW( RAMP_LIST_EXIT ); 
            }
        }
    }
    *aIsResult = STL_FALSE;
    if( sIsNull == STL_TRUE )
    {
        DTL_SET_NULL( (dtlBoolean*)aResultValue );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = STL_FALSE;
        DTL_SET_BOOLEAN_LENGTH( aResultValue );
    }
    
    STL_RAMP( RAMP_LIST_EXIT );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief In
 *        <BR> (expr, expr) In ( (expr, expr), (expr, expr) )
 *        <BR>   In => [ P, O, M, S ]
 *        <BR> ex) (1, 2) In ( (1, 2), (3, 4) )  =>  true
 *                                      <BR> (EXPR,EXPR) IN ( (VALUE, VALUE), (VALUE ... ) )
 * @param[in]  aInputArgumentCnt        input argument count
 * @param[in]  aRowValueCnt             Row Value Count
 * @param[in]  aInputArgument           input argument
 * @param[in]  aContextInfo             Context info
 * @param[in]  aBlockIdx                EvalInfo Block Idx
 * @param[out] aIsResult                List Function Result
 * @param[out] aResultValue             연산 결과 (결과타입 BOOLEAN)
 * @param[out] aEnv                     environment (stlErrorStack)
 */
stlStatus knlMultiInOnlyValue( stlUInt16              aInputArgumentCnt,
                               stlUInt16              aRowValueCnt,
                               knlListEntry         * aInputArgument,
                               knlExprContextInfo   * aContextInfo,
                               stlInt32               aBlockIdx,
                               stlBool              * aIsResult,
                               void                 * aResultValue,
                               void                 * aEnv )
{
    dtlDataValue       * sLeft;
    dtlDataValue       * sRight;
    knlExprContext     * sLeftContext  = NULL;
    knlExprContext     * sRightContext = NULL;
    stlInt32             sIdx          = 0;
    stlUInt16            sValueCount;
    stlBool              sIsNull       = STL_FALSE;
    stlBool              sIsFalse;
    stlUInt16            sTrueCnt;
    stlBool              sResultValue = STL_FALSE;
    

    STL_PARAM_VALIDATE( aInputArgumentCnt <= KNL_LIST_FUNC_MAX_VALUE_CNT, KNL_ERROR_STACK( aEnv ) );

    *aIsResult = STL_FALSE;
    while( sIdx < aInputArgumentCnt )
    {
        sIsFalse = STL_FALSE;
        sTrueCnt = 0;
        for( sValueCount = 0 ; sValueCount < aRowValueCnt ; sValueCount ++ )
        {
            sLeftContext =
                & aContextInfo->mContexts[ aInputArgument[sIdx].mOffset ];
        
            sLeft =
                KNL_GET_VALUE_BLOCK_DATA_VALUE( sLeftContext->mInfo.mValueInfo,
                                                aBlockIdx );
            if( sLeftContext->mCast == NULL )
            {
                /* Do Nothing */
            }
            else
            {
                if( DTL_IS_NULL( sLeft ) )
                {
                    DTL_SET_NULL( & sLeftContext->mCast->mCastResultBuf );
                }
                else
                {
                    sLeftContext->mCast->mArgs[0].mValue.mDataValue =
                        sLeft;

                    STL_TRY( sLeftContext->mCast->mCastFuncPtr(
                                 DTL_CAST_INPUT_ARG_CNT,
                                 sLeftContext->mCast->mArgs,
                                 (void *) & sLeftContext->mCast->mCastResultBuf,
                                 (void *)(sLeftContext->mCast->mSrcTypeInfo),
                                 KNL_DT_VECTOR(aEnv),
                                 aEnv,
                                 KNL_ERROR_STACK( aEnv ) )
                             == STL_SUCCESS );
                                
                    sLeft =
                        & sLeftContext->mCast->mCastResultBuf;
                }
            }

            sIdx ++;
            
            sRightContext =
                & aContextInfo->mContexts[ aInputArgument[sIdx].mOffset ];

            sRight =
                KNL_GET_VALUE_BLOCK_DATA_VALUE( sRightContext->mInfo.mValueInfo,
                                                aBlockIdx );
                             
            if( sRightContext->mCast == NULL )
            {
                /* Do Nothing */
            }
            else
            {
                if( DTL_IS_NULL( sRight ) )
                {
                    DTL_SET_NULL( & sRightContext->mCast->mCastResultBuf );
                }
                else
                {
                    sRightContext->mCast->mArgs[0].mValue.mDataValue =
                        sRight;

                    STL_TRY( sRightContext->mCast->mCastFuncPtr(
                                 DTL_CAST_INPUT_ARG_CNT,
                                 sRightContext->mCast->mArgs,
                                 (void *) & sRightContext->mCast->mCastResultBuf,
                                 (void *)(sRightContext->mCast->mSrcTypeInfo),
                                 KNL_DT_VECTOR(aEnv),
                                 aEnv,
                                 KNL_ERROR_STACK( aEnv ) )
                             == STL_SUCCESS );
                }
                
                sRight =
                    & sRightContext->mCast->mCastResultBuf;
            }

            sIdx  ++;
        
            if( DTL_IS_NULL( sLeft ) || DTL_IS_NULL( sRight ) )
            {
                /*
                 * ( (TRUE, TRUE ) IN ( ( TRUE, NULL ) ) )  == NULL
                 * ( (TRUE, TRUE, TRUE ) IN ( ( TRUE, TRUE, NULL )  ) )  == NULL
                 * ( (TRUE, TRUE, TRUE ) IN ( ( TRUE, FALSE, NULL ) ) )  == FALSE
                 */
                
                if( sIsFalse == STL_TRUE )
                {
                    /* Do Nothing */
                }
                else
                {
                    DTL_SET_NULL( (dtlBoolean*)aResultValue );
                }
            }
            else
            {
                sResultValue =
                    ( dtlPhysicalFuncArg2FuncList[ sLeft->mType ][ sRight->mType ]
                      [ DTL_PHYSICAL_FUNC_IS_EQUAL ]
                      ( sLeft->mValue,
                        sLeft->mLength,
                        sRight->mValue,
                        sRight->mLength ) );

                if( sResultValue == STL_TRUE )
                {
                    sTrueCnt ++;
                    if( aRowValueCnt == sTrueCnt )
                    {
                        /*
                         * aRowValueCnt == sTrueCnt 는
                         * Row안의 Value들이
                         * true들로만 구성되있음을 의미한다.
                         * 하나의 Row만 True(같은 값)이면 Result를 True로 내보낸다. (Jump)
                         */ 
                        *aIsResult = STL_TRUE;
                        DTL_BOOLEAN( aResultValue ) = STL_TRUE;
                        DTL_SET_BOOLEAN_LENGTH( aResultValue );
                        STL_THROW( RAMP_LIST_EXIT );
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                }   
                else
                {
                    if( sIsNull == STL_FALSE )
                    {
                        sIsFalse = STL_TRUE;
                        DTL_SET_BOOLEAN_LENGTH( aResultValue );
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                }   
            }
        }
        sIsNull = (  DTL_IS_NULL( (dtlBoolean*)aResultValue ) == STL_TRUE ) ?
            STL_TRUE : STL_FALSE;   
    }
    DTL_BOOLEAN( aResultValue ) = STL_FALSE;
    STL_RAMP( RAMP_LIST_EXIT );
 
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief In
 *        <BR> (expr, expr) In ( (expr, expr), ( expr,expr) )
 *        <BR>   In => [ P, O, M, S ]
 *        <BR> ex) (10,11) In ( (10,11), ('11',12) )  =>  true
 *                                      <BR> (EXPR, EXPR) IN ( (VALUE, VALUE), (VALUE ...) )
 * @param[in]  aInputArgumentCnt        input argument count
 * @param[in]  aRowValueCnt             Row Value Count
 * @param[in]  aInputArgument           input argument
 * @param[in]  aContextInfo             Context info
 * @param[in]  aBlockIdx                EvalInfo Block Idx
 * @param[out] aIsResult                List Function Result
 * @param[out] aResultValue             연산 결과 (결과타입 BOOLEAN)
 * @param[out] aEnv                     environment (stlErrorStack)
 */
stlStatus knlMultiIn( stlUInt16              aInputArgumentCnt,
                      stlUInt16              aRowValueCnt,
                      knlListEntry         * aInputArgument,
                      knlExprContextInfo   * aContextInfo,
                      stlInt32               aBlockIdx,
                      stlBool              * aIsResult,
                      void                 * aResultValue,
                      void                 * aEnv )
{
    dtlDataValue       * sLeft;
    dtlDataValue       * sRight;
    knlExprContext     * sListContext = NULL;
    stlInt32             sIdx         = 0;
    stlUInt16            sValueCount;
    stlBool              sIsNull;
    stlBool              sIsFalse;
    stlUInt16            sTrueCnt;
    stlBool              sResultValue = STL_FALSE;

    STL_PARAM_VALIDATE( aInputArgumentCnt <= KNL_LIST_FUNC_MAX_VALUE_CNT, KNL_ERROR_STACK( aEnv ) );

    sIsNull = (  DTL_IS_NULL( (dtlBoolean*)aResultValue ) ) ? STL_TRUE : STL_FALSE;

    *aIsResult = STL_FALSE;
    while( sIdx < aInputArgumentCnt )
    {
        sIsFalse = STL_FALSE;
        sTrueCnt = 0;
        for( sValueCount = 0 ; sValueCount < aRowValueCnt ; sValueCount ++ )
        {           
            sListContext =
                & aContextInfo->mContexts[ aInputArgument[sIdx].mOffset ];

            KNL_LIST_FUNC_GET_MULTI_VALUE( sLeft,
                                           aInputArgument[sIdx].mType,
                                           sListContext,
                                           aBlockIdx,
                                           aEnv  );
            sListContext =
                & aContextInfo->mContexts[ aInputArgument[sIdx + 1].mOffset ];

            KNL_LIST_FUNC_GET_MULTI_VALUE( sRight,
                                           aInputArgument[sIdx + 1].mType,
                                           sListContext,
                                           aBlockIdx,
                                           aEnv  );               
        
            sIdx += 2;
            if( DTL_IS_NULL( sLeft ) || DTL_IS_NULL( sRight ) )
            {
                /*
                 * ( (TRUE, TRUE ) IN ( ( TRUE, NULL ) ) )  == NULL
                 * ( (TRUE, TRUE, TRUE ) IN ( ( TRUE, TRUE, NULL )  ) )  == NULL
                 * ( (TRUE, TRUE, TRUE ) IN ( ( TRUE, FALSE, NULL ) ) )  == FALSE
                 */
                
                if( sIsFalse == STL_TRUE )
                {
                    /* Do Nothing */
                }
                else
                {
                    DTL_SET_NULL( (dtlBoolean*)aResultValue );
                }
            }
            else
            {
                sResultValue =
                    ( dtlPhysicalFuncArg2FuncList[ sLeft->mType ][ sRight->mType ]
                      [ DTL_PHYSICAL_FUNC_IS_EQUAL ]
                      ( sLeft->mValue,
                        sLeft->mLength,
                        sRight->mValue,
                        sRight->mLength ) );

                if( sResultValue == STL_TRUE )
                {
                    sTrueCnt ++;
                    if( aRowValueCnt == sTrueCnt )
                    {
                         /*
                         * aRowValueCnt == sTrueCnt 는
                         * Row안의 Value들이
                         * true들로만 구성되있음을 의미한다.
                         * 하나의 Row만 True(같은 값)이면 Result를 True로 내보낸다. (Jump)
                         */ 
                        *aIsResult = STL_TRUE;
                        DTL_BOOLEAN( aResultValue ) = STL_TRUE;
                        DTL_SET_BOOLEAN_LENGTH( aResultValue );
                        STL_THROW( RAMP_LIST_EXIT );
                    }
                    else
                    {
                        /* Do Nohitng */
                    }
                }   
                else
                {
                    if( sIsNull == STL_FALSE )
                    {
                        sIsFalse = STL_TRUE;
                        DTL_SET_BOOLEAN_LENGTH( aResultValue );
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                }   
            }
        }
        sIsNull = (  DTL_IS_NULL( (dtlBoolean*)aResultValue ) == STL_TRUE ) ?
            STL_TRUE : STL_FALSE;   
    }
    DTL_BOOLEAN( aResultValue ) = STL_FALSE;
    STL_RAMP( RAMP_LIST_EXIT );
 
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/************************************************
 * NOT IN FUNCTION
 ************************************************/
/*******************************************************************************************
 * NOT IN FUNCTION
 * GET FUNCTION POINTER 
 ******************************************************************************************/

/**
 * @brief Single Row Only Value Not In function pointer 얻기
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus knlGetListFuncPtrSingleNotInOnlyValue( knlBuiltInListFuncPtr   * aFuncPtr,
                                                 stlErrorStack           * aErrorStack )
{
    /**
     * output 설정
     */
    
     *aFuncPtr = knlSingleNotInOnlyValue;
    
    return STL_SUCCESS;
}


/**
 * @brief Single Row All Expr Not In function pointer 얻기
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus knlGetListFuncPtrSingleNotIn( knlBuiltInListFuncPtr   * aFuncPtr,
                                        stlErrorStack           * aErrorStack )
{
    /**
     * output 설정
     */
    
     *aFuncPtr = knlSingleNotIn;
    
    return STL_SUCCESS;
}

/**
 * @brief Multi Row Only Value Not In function pointer 얻기
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus knlGetListFuncPtrMultiNotInOnlyValue( knlBuiltInListFuncPtr   * aFuncPtr,
                                                stlErrorStack           * aErrorStack )
{
    /**
     * output 설정
     */
    
     *aFuncPtr = knlMultiNotInOnlyValue;
    
    return STL_SUCCESS;
}

/**
 * @brief Multi Row All Expr Not In function pointer 얻기
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus knlGetListFuncPtrMultiNotIn( knlBuiltInListFuncPtr   * aFuncPtr,
                                       stlErrorStack           * aErrorStack )
{
    /**
     * output 설정
     */
    
     *aFuncPtr = knlMultiNotIn;
    
    return STL_SUCCESS;
}


/*******************************************************************************************
 * IN FUNCTION
 * FUNCTION  
 ******************************************************************************************/

/**
 * @brief Not In
 *        <BR> expr Not In ( expr, expr )
 *        <BR>   Not In => [ P, O, M, S ]
 *        <BR> ex) 10 Not In ( 3, 11, '1' )  =>  true
 *                                      <BR> EXPR NOT IN ( VALUE, VALUE, VALUE ... )
 * @param[in]  aInputArgumentCnt        input argument count
 * @param[in]  aRowValueCnt             Row Value Count
 * @param[in]  aInputArgument           input argument
 * @param[in]  aContextInfo             Context info
 * @param[in]  aBlockIdx                EvalInfo Block Idx
 * @param[out] aIsResult                List Function Result
 * @param[out] aResultValue             연산 결과 (결과타입 BOOLEAN)
 * @param[out] aEnv                     environment (stlErrorStack)
 */
stlStatus knlSingleNotInOnlyValue( stlUInt16              aInputArgumentCnt,
                                   stlUInt16              aRowValueCnt,
                                   knlListEntry         * aInputArgument,
                                   knlExprContextInfo   * aContextInfo,
                                   stlInt32               aBlockIdx,
                                   stlBool              * aIsResult,
                                   void                 * aResultValue,
                                   void                 * aEnv )
{
    STL_TRY( knlSingleInOnlyValue( aInputArgumentCnt,
                                   aRowValueCnt,
                                   aInputArgument,
                                   aContextInfo,
                                   aBlockIdx,
                                   aIsResult,
                                   aResultValue,
                                   aEnv ) == STL_SUCCESS );

    if( DTL_IS_NULL( (dtlBoolean*)aResultValue ) == STL_TRUE )
    {
        /* Do Nothing */
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( ( DTL_BOOLEAN( aResultValue ) == STL_TRUE ) ?
                                        STL_FALSE : STL_TRUE );
    }

    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Not In
 *        <BR> expr Not In ( expr, expr )
 *        <BR>  Not In => [ P, O, M, S ]
 *        <BR> ex) 10 In ( 9, 11, '12' )  =>  true
 *                                      <BR> EXPR NOT IN ( VALUE, VALUE, VALUE ... )
 * @param[in]  aInputArgumentCnt        input argument count
 * @param[in]  aRowValueCnt             Row Value Count
 * @param[in]  aInputArgument           input argument
 * @param[in]  aContextInfo             Context info
 * @param[in]  aBlockIdx                EvalInfo Block Idx
 * @param[out] aIsResult                List Function Result
 * @param[out] aResultValue             연산 결과 (결과타입 BOOLEAN)
 * @param[out] aEnv                     environment (stlErrorStack)
 */
stlStatus knlSingleNotIn( stlUInt16              aInputArgumentCnt,
                          stlUInt16              aRowValueCnt,
                          knlListEntry         * aInputArgument,
                          knlExprContextInfo   * aContextInfo,
                          stlInt32               aBlockIdx,
                          stlBool              * aIsResult,
                          void                 * aResultValue,
                          void                 * aEnv )
{
    STL_TRY( knlSingleIn( aInputArgumentCnt,
                          aRowValueCnt,
                          aInputArgument,
                          aContextInfo,
                          aBlockIdx,
                          aIsResult,
                          aResultValue,
                          aEnv ) == STL_SUCCESS );

    if( DTL_IS_NULL( (dtlBoolean*)aResultValue ) == STL_TRUE )
    {
        /* Do Nothing */
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( ( DTL_BOOLEAN( aResultValue ) == STL_TRUE ) ?
                                        STL_FALSE : STL_TRUE );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Not In
 *        <BR> expr Not In ( expr, expr )
 *        <BR>   Not In => [ P, O, M, S ]
 *        <BR> ex) 10 Not In ( 3, 11, '1' )  =>  true
 *                                      <BR> EXPR NOT IN ( VALUE, VALUE, VALUE ... )
 * @param[in]  aInputArgumentCnt        input argument count
 * @param[in]  aRowValueCnt             Row Value Count
 * @param[in]  aInputArgument           input argument
 * @param[in]  aContextInfo             Context info
 * @param[in]  aBlockIdx                EvalInfo Block Idx
 * @param[out] aIsResult                List Function Result
 * @param[out] aResultValue             연산 결과 (결과타입 BOOLEAN)
 * @param[out] aEnv                     environment (stlErrorStack)
 */
stlStatus knlMultiNotInOnlyValue( stlUInt16              aInputArgumentCnt,
                                  stlUInt16              aRowValueCnt,
                                  knlListEntry         * aInputArgument,
                                  knlExprContextInfo   * aContextInfo,
                                  stlInt32               aBlockIdx,
                                  stlBool              * aIsResult,
                                  void                 * aResultValue,
                                  void                 * aEnv )
{
    STL_TRY( knlMultiInOnlyValue( aInputArgumentCnt,
                                  aRowValueCnt,
                                  aInputArgument,
                                  aContextInfo,
                                  aBlockIdx,
                                  aIsResult,
                                  aResultValue,
                                  aEnv ) == STL_SUCCESS );

    if( DTL_IS_NULL( (dtlBoolean*)aResultValue ) == STL_TRUE )
    {
        /* Do Nothing */
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( ( DTL_BOOLEAN( aResultValue ) == STL_TRUE ) ?
                                        STL_FALSE : STL_TRUE );
    }

    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Not In
 *        <BR> expr Not In ( expr, expr )
 *        <BR>  Not In => [ P, O, M, S ]
 *        <BR> ex) 10 Not In ( 9, 11, '12' )  =>  true
 *                                      <BR> EXPR NOT IN ( VALUE, VALUE, VALUE ... )
 * @param[in]  aInputArgumentCnt        input argument count
 * @param[in]  aRowValueCnt             Row Value Count
 * @param[in]  aInputArgument           input argument
 * @param[in]  aContextInfo             Context info
 * @param[in]  aBlockIdx                EvalInfo Block Idx
 * @param[out] aIsResult                List Function Result
 * @param[out] aResultValue             연산 결과 (결과타입 BOOLEAN)
 * @param[out] aEnv                     environment (stlErrorStack)
 */
stlStatus knlMultiNotIn( stlUInt16              aInputArgumentCnt,
                         stlUInt16              aRowValueCnt,
                         knlListEntry         * aInputArgument,
                         knlExprContextInfo   * aContextInfo,
                         stlInt32               aBlockIdx,
                         stlBool              * aIsResult,
                         void                 * aResultValue,
                         void                 * aEnv )
{
    STL_TRY( knlMultiIn( aInputArgumentCnt,
                         aRowValueCnt,
                         aInputArgument,
                         aContextInfo,
                         aBlockIdx,
                         aIsResult,
                         aResultValue,
                         aEnv ) == STL_SUCCESS );

    if( DTL_IS_NULL( (dtlBoolean*)aResultValue ) == STL_TRUE )
    {
        /* Do Nothing */
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( ( DTL_BOOLEAN( aResultValue ) == STL_TRUE ) ?
                                        STL_FALSE : STL_TRUE );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/************************************************
 * LIST OPERATION
 ************************************************/

/*******************************************************************************************
 * LIST OPERATION ( EQUAL )
 * GET FUNCTION POINTER 
 ******************************************************************************************/

/**
 * @brief Multi Row Equal pointer 얻기
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus knlGetListFuncPtrEqualRow( knlBuiltInListFuncPtr   * aFuncPtr,
                                     stlErrorStack           * aErrorStack )
{
    /**
     * output 설정
     */
    
     *aFuncPtr = knlEqualRow;
    
    return STL_SUCCESS;
}


/**
 * @brief Is Equal
 *        <BR> (expr, expr) = (expr, expr)
 *        <BR> ( = ) Equal => [ P, M, S ]
 *        <BR> ex) (10,11) = (10,11)  =>  true
 *                                      <BR> (EXPR, EXPR) = (VALUE, VALUE)
 * @param[in]  aInputArgumentCnt        input argument count
 * @param[in]  aRowValueCnt             Row Value Count
 * @param[in]  aInputArgument           input argument
 * @param[in]  aContextInfo             Context info
 * @param[in]  aBlockIdx                EvalInfo Block Idx
 * @param[out] aIsResult                List Function Result
 * @param[out] aResultValue             연산 결과 (결과타입 BOOLEAN)
 * @param[out] aEnv                     environment (stlErrorStack)
 */
stlStatus knlEqualRow( stlUInt16              aInputArgumentCnt,
                       stlUInt16              aRowValueCnt,
                       knlListEntry         * aInputArgument,
                       knlExprContextInfo   * aContextInfo,
                       stlInt32               aBlockIdx,
                       stlBool              * aIsResult,
                       void                 * aResultValue,
                       void                 * aEnv )
{
    dtlDataValue       * sLeft;
    dtlDataValue       * sRight;
    knlExprContext     * sListContext = NULL;
    knlExprContext     * sLeftCast    = NULL;
    stlInt32             sIdx         = 0;
    stlBool              sResultValue = STL_FALSE;
    stlBool              sIsNull      = STL_FALSE;
    
    STL_PARAM_VALIDATE( aInputArgumentCnt <= KNL_LIST_FUNC_MAX_VALUE_CNT, KNL_ERROR_STACK( aEnv ) );

    while( sIdx < aInputArgumentCnt )
    {
        sListContext =
            & aContextInfo->mContexts[ aInputArgument[sIdx].mOffset ];

        KNL_LIST_FUNC_GET_LEFT_VALUE( sLeft,
                                      aInputArgument[sIdx].mType,
                                      sListContext,
                                      sLeftCast,
                                      aBlockIdx,
                                      aEnv  );
        sListContext =
            & aContextInfo->mContexts[ aInputArgument[sIdx + 1].mOffset ];

        KNL_LIST_FUNC_GET_RIGHT_VALUE( sRight,
                                       aInputArgument[sIdx + 1].mType,
                                       sListContext,
                                       aBlockIdx,
                                       aEnv  );               
        
        sIdx += 2;
        
        if( DTL_IS_NULL( sLeft ) || DTL_IS_NULL( sRight ) )
        {
            sIsNull = STL_TRUE;
        }
        else
        {
            sResultValue =
                ( dtlPhysicalFuncArg2FuncList[ sLeft->mType ][ sRight->mType ]
                  [ DTL_PHYSICAL_FUNC_IS_EQUAL ]
                  ( sLeft->mValue,
                    sLeft->mLength,
                    sRight->mValue,
                    sRight->mLength ) );

            if( sResultValue == STL_FALSE )
            {
                *aIsResult = STL_TRUE;
                DTL_BOOLEAN( aResultValue ) = STL_FALSE;
                DTL_SET_BOOLEAN_LENGTH( aResultValue );
                
                STL_THROW( RAMP_LIST_EXIT );
            }
            else
            {
                /* Do Nothing */
            }
        }
    }
    *aIsResult = STL_FALSE;
    if( sIsNull == STL_TRUE )
    {
        DTL_SET_NULL( (dtlBoolean*)aResultValue );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = STL_TRUE;
        DTL_SET_BOOLEAN_LENGTH( aResultValue );
    }
    STL_RAMP( RAMP_LIST_EXIT );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/*******************************************************************************************
 * LIST OPERATION ( NOT EQUAL )
 * GET FUNCTION POINTER 
 ******************************************************************************************/

/**
 * @brief Multi Row Not Equal pointer 얻기
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus knlGetListFuncPtrNotEqualRow( knlBuiltInListFuncPtr   * aFuncPtr,
                                        stlErrorStack           * aErrorStack )
{
    /**
     * output 설정
     */
    
     *aFuncPtr = knlNotEqualRow;
    
    return STL_SUCCESS;
}


/**
 * @brief Is Not Equal
 *        <BR> (expr, expr) != (expr, expr)
 *        <BR> ( != ) Not Equal => [ P, M, S ]
 *        <BR> ex) (10,11) != (9,10)  =>  true
 *                                      <BR> (EXPR, EXPR) != (VALUE, VALUE)
 * @param[in]  aInputArgumentCnt        input argument count
 * @param[in]  aRowValueCnt             Row Value Count
 * @param[in]  aInputArgument           input argument
 * @param[in]  aContextInfo             Context info
 * @param[in]  aBlockIdx                EvalInfo Block Idx
 * @param[out] aIsResult                List Function Result
 * @param[out] aResultValue             연산 결과 (결과타입 BOOLEAN)
 * @param[out] aEnv                     environment (stlErrorStack)
 */
stlStatus knlNotEqualRow( stlUInt16              aInputArgumentCnt,
                          stlUInt16              aRowValueCnt,
                          knlListEntry         * aInputArgument,
                          knlExprContextInfo   * aContextInfo,
                          stlInt32               aBlockIdx,
                          stlBool              * aIsResult,
                          void                 * aResultValue,
                          void                 * aEnv )
{
    STL_TRY( knlEqualRow( aInputArgumentCnt,
                          aRowValueCnt,
                          aInputArgument,
                          aContextInfo,
                          aBlockIdx,
                          aIsResult,
                          aResultValue,
                          aEnv ) == STL_SUCCESS );

    if( DTL_IS_NULL( (dtlBoolean*)aResultValue ) == STL_TRUE )
    {
        /* Do Nothing */
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( ( DTL_BOOLEAN( aResultValue ) == STL_TRUE ) ?
                                        STL_FALSE : STL_TRUE );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*******************************************************************************************
 * LIST OPERATION ( LESS THAN )
 * GET FUNCTION POINTER 
 ******************************************************************************************/

/**
 * @brief Multi Row Less Than pointer 얻기
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus knlGetListFuncPtrLessThanRow( knlBuiltInListFuncPtr   * aFuncPtr,
                                        stlErrorStack           * aErrorStack )
{
    /**
     * output 설정
     */
    
     *aFuncPtr = knlLessThanRow;
    
    return STL_SUCCESS;
}


/**
 * @brief Is Less Than
 *        <BR> (expr, expr) < (expr, expr)
 *        <BR> ( < ) Less Than => [ P, M, S ]
 *        <BR> ex) (9,10) < (10,11)  =>  true
 *                                      <BR> (EXPR, EXPR) < (VALUE, VALUE)
 * @param[in]  aInputArgumentCnt        input argument count
 * @param[in]  aRowValueCnt             Row Value Count
 * @param[in]  aInputArgument           input argument
 * @param[in]  aContextInfo             Context info
 * @param[in]  aBlockIdx                EvalInfo Block Idx
 * @param[out] aIsResult                List Function Result
 * @param[out] aResultValue             연산 결과 (결과타입 BOOLEAN)
 * @param[out] aEnv                     environment (stlErrorStack)
 */
stlStatus knlLessThanRow( stlUInt16              aInputArgumentCnt,
                          stlUInt16              aRowValueCnt,
                          knlListEntry         * aInputArgument,
                          knlExprContextInfo   * aContextInfo,
                          stlInt32               aBlockIdx,
                          stlBool              * aIsResult,
                          void                 * aResultValue,
                          void                 * aEnv )
{
    dtlDataValue       * sLeft;
    dtlDataValue       * sRight;
    knlExprContext     * sListContext     = NULL;
    knlExprContext     * sLeftCast        = NULL;
    stlInt32             sIdx             = aInputArgumentCnt;
    dtlCompareResult     sCompareResult   = DTL_COMPARISON_EQUAL;
    
    STL_PARAM_VALIDATE( aInputArgumentCnt <= KNL_LIST_FUNC_MAX_VALUE_CNT, KNL_ERROR_STACK( aEnv ) );

    *aIsResult = STL_TRUE;
    DTL_BOOLEAN( aResultValue ) = STL_FALSE;
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    while( 0 < sIdx )
    {
        sIdx--;
        sListContext =
            & aContextInfo->mContexts[ aInputArgument[sIdx].mOffset ];

        KNL_LIST_FUNC_GET_LEFT_VALUE( sRight,
                                      aInputArgument[sIdx].mType,
                                      sListContext,
                                      sLeftCast,
                                      aBlockIdx,
                                      aEnv  );
        sIdx--;
        sListContext =
            & aContextInfo->mContexts[ aInputArgument[sIdx].mOffset ];

        KNL_LIST_FUNC_GET_RIGHT_VALUE( sLeft,
                                       aInputArgument[sIdx].mType,
                                       sListContext,
                                       aBlockIdx,
                                       aEnv  );               
        
        if( DTL_IS_NULL( sLeft ) || DTL_IS_NULL( sRight ) )
        {
            DTL_SET_NULL( (dtlBoolean*)aResultValue );
            STL_THROW( RAMP_LIST_EXIT );
        }
        else
        {
            sCompareResult =
                ( dtlPhysicalCompareFuncList[ sLeft->mType ][ sRight->mType ]
                  ( sLeft->mValue,
                    sLeft->mLength,
                    sRight->mValue,
                    sRight->mLength ) );

            if( sCompareResult == DTL_COMPARISON_LESS )
            {
                /*
                 * Left가 작은경우로 TRUE
                 */
                DTL_BOOLEAN( aResultValue ) = STL_TRUE;                
                break;
            }
            else if( sCompareResult == DTL_COMPARISON_GREATER )
            {
                /*
                 * Right가 큰경우로 FALSE
                 */
                break;
            }
            else
            {
                /*
                 * DTL_COMPARISON_EQUAL
                 *
                 * Do Nothing
                 */
            }
        }
    }    
    STL_RAMP( RAMP_LIST_EXIT );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*******************************************************************************************
 * LIST OPERATION ( LESS THAN EQUAL )
 * GET FUNCTION POINTER 
 ******************************************************************************************/

/**
 * @brief Multi Row Less Than Equal Pointer 얻기
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus knlGetListFuncPtrLessThanEqualRow( knlBuiltInListFuncPtr   * aFuncPtr,
                                             stlErrorStack           * aErrorStack )
{
    /**
     * output 설정
     */
    
     *aFuncPtr = knlLessThanEqualRow;
    
    return STL_SUCCESS;
}


/**
 * @brief Is Less Than Equal
 *        <BR> (expr, expr) <= (expr, expr)
 *        <BR> ( <= ) Less Than Equal => [ P, M, S ]
 *        <BR> ex) (9,10) <= (10,10)  =>  true
 *                                      <BR> (EXPR, EXPR) <= (VALUE, VALUE)
 * @param[in]  aInputArgumentCnt        input argument count
 * @param[in]  aRowValueCnt             Row Value Count
 * @param[in]  aInputArgument           input argument
 * @param[in]  aContextInfo             Context info
 * @param[in]  aBlockIdx                EvalInfo Block Idx
 * @param[out] aIsResult                List Function Result
 * @param[out] aResultValue             연산 결과 (결과타입 BOOLEAN)
 * @param[out] aEnv                     environment (stlErrorStack)
 */
stlStatus knlLessThanEqualRow( stlUInt16              aInputArgumentCnt,
                               stlUInt16              aRowValueCnt,
                               knlListEntry         * aInputArgument,
                               knlExprContextInfo   * aContextInfo,
                               stlInt32               aBlockIdx,
                               stlBool              * aIsResult,
                               void                 * aResultValue,
                               void                 * aEnv )
{
    dtlDataValue       * sLeft;
    dtlDataValue       * sRight;
    knlExprContext     * sListContext     = NULL;
    knlExprContext     * sLeftCast        = NULL;
    stlInt32             sIdx             = aInputArgumentCnt;
    dtlCompareResult     sCompareResult   = DTL_COMPARISON_EQUAL;
    
    STL_PARAM_VALIDATE( aInputArgumentCnt <= KNL_LIST_FUNC_MAX_VALUE_CNT, KNL_ERROR_STACK( aEnv ) );

    *aIsResult = STL_TRUE;
    DTL_BOOLEAN( aResultValue ) = STL_TRUE;
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    while( 0 < sIdx )
    {
        sIdx--;
        sListContext =
            & aContextInfo->mContexts[ aInputArgument[sIdx].mOffset ];

        KNL_LIST_FUNC_GET_LEFT_VALUE( sRight,
                                      aInputArgument[sIdx].mType,
                                      sListContext,
                                      sLeftCast,
                                      aBlockIdx,
                                      aEnv  );
        sIdx--;
        sListContext =
            & aContextInfo->mContexts[ aInputArgument[sIdx].mOffset ];

        KNL_LIST_FUNC_GET_RIGHT_VALUE( sLeft,
                                       aInputArgument[sIdx].mType,
                                       sListContext,
                                       aBlockIdx,
                                       aEnv  );               
        
        
        if( DTL_IS_NULL( sLeft ) || DTL_IS_NULL( sRight ) )
        {
            DTL_SET_NULL( (dtlBoolean*)aResultValue );
            STL_THROW( RAMP_LIST_EXIT );
        }
        else
        {
            sCompareResult =
                ( dtlPhysicalCompareFuncList[ sLeft->mType ][ sRight->mType ]
                  ( sLeft->mValue,
                    sLeft->mLength,
                    sRight->mValue,
                    sRight->mLength ) );

            if( sCompareResult == DTL_COMPARISON_LESS )
            {
                /*
                 * Left가 작은경우로 TRUE
                 */
                break;                
            }
            else if( sCompareResult == DTL_COMPARISON_GREATER )
            {
                /*
                 * Right가 큰경우로 FALSE
                 */
                DTL_BOOLEAN( aResultValue ) = STL_FALSE;
                break;
            }
            else
            {
                /*
                 * DTL_COMPARISON_EQUAL
                 *
                 * Do Nothing
                 */
            }
        }
    }
    STL_RAMP( RAMP_LIST_EXIT );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*******************************************************************************************
 * LIST OPERATION ( IS GREATER THAN )
 * GET FUNCTION POINTER 
 ******************************************************************************************/

/**
 * @brief Multi Row Greater Than pointer 얻기
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus knlGetListFuncPtrGreaterThanRow( knlBuiltInListFuncPtr   * aFuncPtr,
                                           stlErrorStack           * aErrorStack )
{
    /**
     * output 설정
     */
    
     *aFuncPtr = knlGreaterThanRow;
    
    return STL_SUCCESS;
}


/**
 * @brief Is Greater Than
 *        <BR> (expr, expr) > (expr, expr)
 *        <BR> ( > ) Greater Than => [ P, M, S ]
 *        <BR> ex) (12,15) > (10,11)  =>  true
 *                                      <BR> (EXPR, EXPR) > (VALUE, VALUE)
 * @param[in]  aInputArgumentCnt        input argument count
 * @param[in]  aRowValueCnt             Row Value Count
 * @param[in]  aInputArgument           input argument
 * @param[in]  aContextInfo             Context info
 * @param[in]  aBlockIdx                EvalInfo Block Idx
 * @param[out] aIsResult                List Function Result
 * @param[out] aResultValue             연산 결과 (결과타입 BOOLEAN)
 * @param[out] aEnv                     environment (stlErrorStack)
 */
stlStatus knlGreaterThanRow( stlUInt16              aInputArgumentCnt,
                             stlUInt16              aRowValueCnt,
                             knlListEntry         * aInputArgument,
                             knlExprContextInfo   * aContextInfo,
                             stlInt32               aBlockIdx,
                             stlBool              * aIsResult,
                             void                 * aResultValue,
                             void                 * aEnv )
{
    dtlDataValue       * sLeft;
    dtlDataValue       * sRight;
    knlExprContext     * sListContext     = NULL;
    knlExprContext     * sLeftCast        = NULL;
    stlInt32             sIdx             = aInputArgumentCnt;
    dtlCompareResult     sCompareResult   = DTL_COMPARISON_EQUAL;
    
    STL_PARAM_VALIDATE( aInputArgumentCnt <= KNL_LIST_FUNC_MAX_VALUE_CNT, KNL_ERROR_STACK( aEnv ) );

    *aIsResult = STL_TRUE;
    DTL_BOOLEAN( aResultValue ) = STL_FALSE;
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    while( 0 < sIdx )
    {
        sIdx--;
        sListContext =
            & aContextInfo->mContexts[ aInputArgument[sIdx].mOffset ];

        KNL_LIST_FUNC_GET_LEFT_VALUE( sRight,
                                      aInputArgument[sIdx].mType,
                                      sListContext,
                                      sLeftCast,
                                      aBlockIdx,
                                      aEnv  );
        sIdx--;
        sListContext =
            & aContextInfo->mContexts[ aInputArgument[sIdx].mOffset ];

        KNL_LIST_FUNC_GET_RIGHT_VALUE( sLeft,
                                       aInputArgument[sIdx].mType,
                                       sListContext,
                                       aBlockIdx,
                                       aEnv  );               
        
        if( DTL_IS_NULL( sLeft ) || DTL_IS_NULL( sRight ) )
        {
            DTL_SET_NULL( (dtlBoolean*)aResultValue );
            STL_THROW( RAMP_LIST_EXIT );
        }
        else
        {
            sCompareResult =
                ( dtlPhysicalCompareFuncList[ sLeft->mType ][ sRight->mType ]
                  ( sLeft->mValue,
                    sLeft->mLength,
                    sRight->mValue,
                    sRight->mLength ) );

            if( sCompareResult == DTL_COMPARISON_GREATER )
            {
                /*
                 * Left가 큰경우로 TRUE
                 */
                DTL_BOOLEAN( aResultValue ) = STL_TRUE;
                break;
            }
            else if( sCompareResult == DTL_COMPARISON_LESS )
            {
                /*
                 * Right가 작은경우로 FALSE
                 */
                break;
            }
            else
            {
                /*
                 * DTL_COMPARISON_EQUAL
                 *
                 * Do Nothing
                 */
            }
        }
    }
    
    STL_RAMP( RAMP_LIST_EXIT );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/*******************************************************************************************
 * LIST OPERATION ( IS GREATER THAN EQUAL )
 * GET FUNCTION POINTER 
 ******************************************************************************************/

/**
 * @brief Multi Row Less Than pointer 얻기
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus knlGetListFuncPtrGreaterThanEqualRow( knlBuiltInListFuncPtr   * aFuncPtr,
                                                stlErrorStack           * aErrorStack )
{
    /**
     * output 설정
     */
    
     *aFuncPtr = knlGreaterThanEqualRow;
    
    return STL_SUCCESS;
}


/**
 * @brief Is Greater Than Equal
 *        <BR> (expr, expr) >= (expr, expr)
 *        <BR> ( >= ) Greater Than Equal => [ P, M, S ]
 *        <BR> ex) (12,15) >= (10,11)  =>  true
 *                                      <BR> (EXPR, EXPR) >= (VALUE, VALUE)
 * @param[in]  aInputArgumentCnt        input argument count
 * @param[in]  aRowValueCnt             Row Value Count
 * @param[in]  aInputArgument           input argument
 * @param[in]  aContextInfo             Context info
 * @param[in]  aBlockIdx                EvalInfo Block Idx
 * @param[out] aIsResult                List Function Result
 * @param[out] aResultValue             연산 결과 (결과타입 BOOLEAN)
 * @param[out] aEnv                     environment (stlErrorStack)
 */
stlStatus knlGreaterThanEqualRow( stlUInt16              aInputArgumentCnt,
                                  stlUInt16              aRowValueCnt,
                                  knlListEntry         * aInputArgument,
                                  knlExprContextInfo   * aContextInfo,
                                  stlInt32               aBlockIdx,
                                  stlBool              * aIsResult,
                                  void                 * aResultValue,
                                  void                 * aEnv )
{
    dtlDataValue       * sLeft;
    dtlDataValue       * sRight;
    knlExprContext     * sListContext     = NULL;
    knlExprContext     * sLeftCast        = NULL;
    stlInt32             sIdx             = aInputArgumentCnt;
    dtlCompareResult     sCompareResult   = DTL_COMPARISON_EQUAL;
    
    STL_PARAM_VALIDATE( aInputArgumentCnt <= KNL_LIST_FUNC_MAX_VALUE_CNT, KNL_ERROR_STACK( aEnv ) );

    *aIsResult = STL_TRUE;
    DTL_BOOLEAN( aResultValue ) = STL_TRUE;
    DTL_SET_BOOLEAN_LENGTH( aResultValue );
    while( 0 < sIdx )
    {
        sIdx--;

        sListContext =
            & aContextInfo->mContexts[ aInputArgument[sIdx].mOffset ];

        KNL_LIST_FUNC_GET_LEFT_VALUE( sRight,
                                      aInputArgument[sIdx].mType,
                                      sListContext,
                                      sLeftCast,
                                      aBlockIdx,
                                      aEnv  );
        sIdx--;
        sListContext =
            & aContextInfo->mContexts[ aInputArgument[sIdx].mOffset ];

        KNL_LIST_FUNC_GET_RIGHT_VALUE( sLeft,
                                       aInputArgument[sIdx].mType,
                                       sListContext,
                                       aBlockIdx,
                                       aEnv  );               
        
        if( DTL_IS_NULL( sLeft ) || DTL_IS_NULL( sRight ) )
        {
            DTL_SET_NULL( (dtlBoolean*)aResultValue );
            STL_THROW( RAMP_LIST_EXIT );
        }
        else
        {
            sCompareResult =
                ( dtlPhysicalCompareFuncList[ sLeft->mType ][ sRight->mType ]
                  ( sLeft->mValue,
                    sLeft->mLength,
                    sRight->mValue,
                    sRight->mLength ) );

            if( sCompareResult == DTL_COMPARISON_GREATER )
            {
                /*
                 * Left가 큰경우로 TRUE
                 */
                break;
            }
            else if( sCompareResult == DTL_COMPARISON_LESS )
            {
                /*
                 * Right가 작은경우로 FALSE
                 */
                DTL_BOOLEAN( aResultValue ) = STL_FALSE;
                break;
            }
            else
            {
                /*
                 * DTL_COMPARISON_EQUAL
                 *
                 * Do Nothing
                 */
            }
        }
    }
    
    STL_RAMP( RAMP_LIST_EXIT );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/************************************************
 * LIST OPERATION ( ANY )
 ************************************************/

/*******************************************************************************************
 * LIST OPERATION
 * GET FUNCTION POINTER 
 ******************************************************************************************/

/**
 * @brief Not Equal Any pointer 얻기
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus knlGetListFuncPtrNotEqualAny( knlBuiltInListFuncPtr   * aFuncPtr,
                                        stlErrorStack           * aErrorStack )
{
    /**
     * output 설정
     */
    
     *aFuncPtr = knlNotEqualAny;
    
    return STL_SUCCESS;
}

/**
 * @brief LessThanAny pointer 얻기
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus knlGetListFuncPtrLessThanAny( knlBuiltInListFuncPtr   * aFuncPtr,
                                        stlErrorStack           * aErrorStack )
{
    /**
     * output 설정
     */
    
     *aFuncPtr = knlLessThanAny;
    
    return STL_SUCCESS;
}

/**
 * @brief LessThanEqualAny pointer 얻기
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus knlGetListFuncPtrLessThanEqualAny( knlBuiltInListFuncPtr   * aFuncPtr,
                                             stlErrorStack           * aErrorStack )
{
    /**
     * output 설정
     */
    
     *aFuncPtr = knlLessThanEqualAny;
    
    return STL_SUCCESS;
}

/**
 * @brief GreaterThanAny pointer 얻기
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus knlGetListFuncPtrGreaterThanAny( knlBuiltInListFuncPtr   * aFuncPtr,
                                           stlErrorStack           * aErrorStack )
{
    /**
     * output 설정
     */
    
     *aFuncPtr = knlGreaterThanAny;
    
    return STL_SUCCESS;
}

/**
 * @brief GreaterThanEqualAny pointer 얻기
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus knlGetListFuncPtrGreaterThanEqualAny( knlBuiltInListFuncPtr   * aFuncPtr,
                                                stlErrorStack           * aErrorStack )
{
    /**
     * output 설정
     */
    
     *aFuncPtr = knlGreaterThanEqualAny;
    
    return STL_SUCCESS;
}

/**
 * MULTI
 */

/**
 * @brief Multi Not Equal Any pointer 얻기
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus knlGetListFuncPtrMultiNotEqualAny( knlBuiltInListFuncPtr   * aFuncPtr,
                                             stlErrorStack           * aErrorStack )
{
    /**
     * output 설정
     */
    
     *aFuncPtr = knlMultiNotEqualAny;
    
    return STL_SUCCESS;
}


/**
 * @brief Multi LessThanAny pointer 얻기
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus knlGetListFuncPtrMultiLessThanAny( knlBuiltInListFuncPtr   * aFuncPtr,
                                             stlErrorStack           * aErrorStack )
{
    /**
     * output 설정
     */
    
     *aFuncPtr = knlMultiLessThanAny;
    
    return STL_SUCCESS;
}

/**
 * @brief Multi LessThanEqualAny pointer 얻기
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus knlGetListFuncPtrMultiLessThanEqualAny( knlBuiltInListFuncPtr   * aFuncPtr,
                                                  stlErrorStack           * aErrorStack )
{
    /**
     * output 설정
     */
    
     *aFuncPtr = knlMultiLessThanEqualAny;
    
    return STL_SUCCESS;
}

/**
 * @brief Multi GreaterThanAny pointer 얻기
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus knlGetListFuncPtrMultiGreaterThanAny( knlBuiltInListFuncPtr   * aFuncPtr,
                                             stlErrorStack           * aErrorStack )
{
    /**
     * output 설정
     */
    
     *aFuncPtr = knlMultiGreaterThanAny;
    
    return STL_SUCCESS;
}

/**
 * @brief Multi LessThanEqualAny pointer 얻기
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus knlGetListFuncPtrMultiGreaterThanEqualAny( knlBuiltInListFuncPtr   * aFuncPtr,
                                                     stlErrorStack           * aErrorStack )
{
    /**
     * output 설정
     */
    
     *aFuncPtr = knlMultiGreaterThanEqualAny;
    
    return STL_SUCCESS;
}


/**
 * @brief Not Equal Any
 *        <BR> (expr) <> ANY (expr, expr)
 *        <BR> ( <> any ) Not Equal Any => [ P, M, S ]
 *        <BR> ex) (9) <> ANY (10,11)  =>  true
 *                                      <BR> (EXPR) <> ANY (VALUE, VALUE)
 * @param[in]  aInputArgumentCnt        input argument count
 * @param[in]  aRowValueCnt             Row Value Count
 * @param[in]  aInputArgument           input argument
 * @param[in]  aContextInfo             Context info
 * @param[in]  aBlockIdx                EvalInfo Block Idx
 * @param[out] aIsResult                List Function Result
 * @param[out] aResultValue             연산 결과 (결과타입 BOOLEAN)
 * @param[out] aEnv                     environment (stlErrorStack)
 */
stlStatus knlNotEqualAny( stlUInt16              aInputArgumentCnt,
                          stlUInt16              aRowValueCnt,
                          knlListEntry         * aInputArgument,
                          knlExprContextInfo   * aContextInfo,
                          stlInt32               aBlockIdx,
                          stlBool              * aIsResult,
                          void                 * aResultValue,
                          void                 * aEnv )
{
    dtlDataValue       * sLeft;
    dtlDataValue       * sRight;
    knlExprContext     * sListContext = NULL;
    knlExprContext     * sLeftCast    = NULL;
    stlBool              sIsNull;
    stlInt32             sIdx         = 0;
    stlBool              sResultValue = STL_FALSE;

    STL_PARAM_VALIDATE( aInputArgumentCnt <= KNL_LIST_FUNC_MAX_VALUE_CNT, KNL_ERROR_STACK( aEnv ) );

    sIsNull = (  DTL_IS_NULL( (dtlBoolean*)aResultValue ) ) ? STL_TRUE : STL_FALSE;

    while( sIdx < aInputArgumentCnt )
    {
        sListContext =
            & aContextInfo->mContexts[ aInputArgument[sIdx].mOffset ];

        KNL_LIST_FUNC_GET_LEFT_VALUE( sLeft,
                                      aInputArgument[sIdx].mType,
                                      sListContext,
                                      sLeftCast,
                                      aBlockIdx,
                                      aEnv  );
        sListContext =
            & aContextInfo->mContexts[ aInputArgument[sIdx + 1].mOffset ];

        KNL_LIST_FUNC_GET_RIGHT_VALUE( sRight,
                                       aInputArgument[sIdx + 1].mType,
                                       sListContext,
                                       aBlockIdx,
                                       aEnv  );               
        
        sIdx += 2;
        
        if( DTL_IS_NULL( sLeft ) || DTL_IS_NULL( sRight ) )
        {
            sIsNull = STL_TRUE;
        }
        else
        {
            sResultValue =
                ( dtlPhysicalFuncArg2FuncList[ sLeft->mType ][ sRight->mType ]
                  [ DTL_PHYSICAL_FUNC_IS_NOT_EQUAL ]
                  ( sLeft->mValue,
                    sLeft->mLength,
                    sRight->mValue,
                    sRight->mLength ) );

            if( sResultValue == STL_TRUE )
            {
                *aIsResult = STL_TRUE;
                DTL_BOOLEAN( aResultValue ) = STL_TRUE;
                DTL_SET_BOOLEAN_LENGTH( aResultValue );

                STL_THROW( RAMP_LIST_EXIT ); 
            }
        }
    }
    *aIsResult = STL_FALSE;
    if( sIsNull == STL_TRUE )
    {
        DTL_SET_NULL( (dtlBoolean*)aResultValue );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = STL_FALSE;
        DTL_SET_BOOLEAN_LENGTH( aResultValue );
    }
    
    STL_RAMP( RAMP_LIST_EXIT );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Less Than Any
 *        <BR> (expr) < ANY (expr, expr)
 *        <BR> ( < any ) Less Than Any => [ P, M, S ]
 *        <BR> ex) (10) < ANY (10,11)  =>  true
 *                                      <BR> (EXPR) < ANY (VALUE, VALUE)
 * @param[in]  aInputArgumentCnt        input argument count
 * @param[in]  aRowValueCnt             Row Value Count
 * @param[in]  aInputArgument           input argument
 * @param[in]  aContextInfo             Context info
 * @param[in]  aBlockIdx                EvalInfo Block Idx
 * @param[out] aIsResult                List Function Result
 * @param[out] aResultValue             연산 결과 (결과타입 BOOLEAN)
 * @param[out] aEnv                     environment (stlErrorStack)
 */
stlStatus knlLessThanAny( stlUInt16              aInputArgumentCnt,
                          stlUInt16              aRowValueCnt,
                          knlListEntry         * aInputArgument,
                          knlExprContextInfo   * aContextInfo,
                          stlInt32               aBlockIdx,
                          stlBool              * aIsResult,
                          void                 * aResultValue,
                          void                 * aEnv )
{
    dtlDataValue       * sLeft;
    dtlDataValue       * sRight;
    knlExprContext     * sListContext = NULL;
    knlExprContext     * sLeftCast    = NULL;
    stlBool              sIsNull;
    stlInt32             sIdx         = 0;
    stlBool              sResultValue = STL_FALSE;
    

    STL_PARAM_VALIDATE( aInputArgumentCnt <= KNL_LIST_FUNC_MAX_VALUE_CNT, KNL_ERROR_STACK( aEnv ) );

    sIsNull = (  DTL_IS_NULL( (dtlBoolean*)aResultValue ) ) ? STL_TRUE : STL_FALSE;

    while( sIdx < aInputArgumentCnt )
    {
       
        sListContext =
            & aContextInfo->mContexts[ aInputArgument[sIdx].mOffset ];

        KNL_LIST_FUNC_GET_LEFT_VALUE( sLeft,
                                      aInputArgument[sIdx].mType,
                                      sListContext,
                                      sLeftCast,
                                      aBlockIdx,
                                      aEnv  );
        sListContext =
            & aContextInfo->mContexts[ aInputArgument[sIdx + 1].mOffset ];

        KNL_LIST_FUNC_GET_RIGHT_VALUE( sRight,
                                       aInputArgument[sIdx + 1].mType,
                                       sListContext,
                                       aBlockIdx,
                                       aEnv  );               
        
        sIdx += 2;

        if( DTL_IS_NULL( sLeft ) || DTL_IS_NULL( sRight ) )
        {
            sIsNull = STL_TRUE;
        }
        else
        {
            sResultValue =
                ( dtlPhysicalFuncArg2FuncList[ sLeft->mType ][ sRight->mType ]
                  [ DTL_PHYSICAL_FUNC_IS_LESS_THAN ]
                  ( sLeft->mValue,
                    sLeft->mLength,
                    sRight->mValue,
                    sRight->mLength ) );

            if( sResultValue == STL_TRUE )
            {
                *aIsResult = STL_TRUE;
                DTL_BOOLEAN( aResultValue ) = STL_TRUE;
                DTL_SET_BOOLEAN_LENGTH( aResultValue );

                STL_THROW( RAMP_LIST_EXIT ); 
            }
        }
    }
    *aIsResult = STL_FALSE;
    if( sIsNull == STL_TRUE )
    {
        DTL_SET_NULL( (dtlBoolean*)aResultValue );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = STL_FALSE;
        DTL_SET_BOOLEAN_LENGTH( aResultValue );
    }
    
    STL_RAMP( RAMP_LIST_EXIT );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Less Than EqualAny
 *        <BR> (expr) <= ANY (expr, expr)
 *        <BR> ( <= any ) Less Than Any => [ P, O, M, S ]
 *        <BR> ex) (10) <= ANY (10,11)  =>  true
 *                                      <BR> (EXPR) <= ANY (VALUE, VALUE)
 * @param[in]  aInputArgumentCnt        input argument count
 * @param[in]  aRowValueCnt             Row Value Count
 * @param[in]  aInputArgument           input argument
 * @param[in]  aContextInfo             Context info
 * @param[in]  aBlockIdx                EvalInfo Block Idx
 * @param[out] aIsResult                List Function Result
 * @param[out] aResultValue             연산 결과 (결과타입 BOOLEAN)
 * @param[out] aEnv                     environment (stlErrorStack)
 */
stlStatus knlLessThanEqualAny( stlUInt16              aInputArgumentCnt,
                               stlUInt16              aRowValueCnt,
                               knlListEntry         * aInputArgument,
                               knlExprContextInfo   * aContextInfo,
                               stlInt32               aBlockIdx,
                               stlBool              * aIsResult,
                               void                 * aResultValue,
                               void                 * aEnv )
{
    dtlDataValue       * sLeft;
    dtlDataValue       * sRight;
    knlExprContext     * sListContext = NULL;
    knlExprContext     * sLeftCast    = NULL;
    stlBool              sIsNull;
    stlInt32             sIdx         = 0;
    stlBool              sResultValue = STL_FALSE;
    
    *aIsResult = STL_FALSE;
    STL_PARAM_VALIDATE( aInputArgumentCnt <= KNL_LIST_FUNC_MAX_VALUE_CNT, KNL_ERROR_STACK( aEnv ) );

    sIsNull = (  DTL_IS_NULL( (dtlBoolean*)aResultValue ) ) ? STL_TRUE : STL_FALSE;
    
    while( sIdx < aInputArgumentCnt )
    {
        sListContext =
            & aContextInfo->mContexts[ aInputArgument[sIdx].mOffset ];

        KNL_LIST_FUNC_GET_LEFT_VALUE( sLeft,
                                      aInputArgument[sIdx].mType,
                                      sListContext,
                                      sLeftCast,
                                      aBlockIdx,
                                      aEnv  );
        sListContext =
            & aContextInfo->mContexts[ aInputArgument[sIdx + 1].mOffset ];

        KNL_LIST_FUNC_GET_RIGHT_VALUE( sRight,
                                       aInputArgument[sIdx + 1].mType,
                                       sListContext,
                                       aBlockIdx,
                                       aEnv  );               
        
        sIdx += 2;

        if( DTL_IS_NULL( sLeft ) || DTL_IS_NULL( sRight ) )
        {
            sIsNull = STL_TRUE;
        }
        else
        {
            sResultValue =
                ( dtlPhysicalFuncArg2FuncList[ sLeft->mType ][ sRight->mType ]
                  [ DTL_PHYSICAL_FUNC_IS_LESS_THAN_EQUAL ]
                  ( sLeft->mValue,
                    sLeft->mLength,
                    sRight->mValue,
                    sRight->mLength ) );

            if( sResultValue == STL_TRUE )
            {
                *aIsResult = STL_TRUE;
                DTL_BOOLEAN( aResultValue ) = STL_TRUE;
                DTL_SET_BOOLEAN_LENGTH( aResultValue );

                STL_THROW( RAMP_LIST_EXIT ); 
            }
        }
    }
    *aIsResult = STL_FALSE;
    if( sIsNull == STL_TRUE )
    {
        DTL_SET_NULL( (dtlBoolean*)aResultValue );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = STL_FALSE;
        DTL_SET_BOOLEAN_LENGTH( aResultValue );
    }
    
    STL_RAMP( RAMP_LIST_EXIT );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Greater Than Any
 *        <BR> (expr) > ANY (expr, expr)
 *        <BR> ( > any ) Greater Than Any => [ P, O, M, S ]
 *        <BR> ex) (10) > ANY (10,11)  =>  true
 *                                      <BR> (EXPR) > ANY (VALUE, VALUE)
 * @param[in]  aInputArgumentCnt        input argument count
 * @param[in]  aRowValueCnt             Row Value Count
 * @param[in]  aInputArgument           input argument
 * @param[in]  aContextInfo             Context info
 * @param[in]  aBlockIdx                EvalInfo Block Idx
 * @param[out] aIsResult                List Function Result
 * @param[out] aResultValue             연산 결과 (결과타입 BOOLEAN)
 * @param[out] aEnv                     environment (stlErrorStack)
 */
stlStatus knlGreaterThanAny( stlUInt16              aInputArgumentCnt,
                             stlUInt16              aRowValueCnt,
                             knlListEntry         * aInputArgument,
                             knlExprContextInfo   * aContextInfo,
                             stlInt32               aBlockIdx,
                             stlBool              * aIsResult,
                             void                 * aResultValue,
                             void                 * aEnv )
{
    dtlDataValue       * sLeft;
    dtlDataValue       * sRight;
    knlExprContext     * sListContext = NULL;
    knlExprContext     * sLeftCast    = NULL;
    stlBool              sIsNull;
    stlInt32             sIdx         = 0;
    stlBool              sResultValue = STL_FALSE;
    
    *aIsResult = STL_FALSE;
    STL_PARAM_VALIDATE( aInputArgumentCnt <= KNL_LIST_FUNC_MAX_VALUE_CNT, KNL_ERROR_STACK( aEnv ) );

    sIsNull = (  DTL_IS_NULL( (dtlBoolean*)aResultValue ) ) ? STL_TRUE : STL_FALSE;
    
    while( sIdx < aInputArgumentCnt )
    {
        sListContext =
            & aContextInfo->mContexts[ aInputArgument[sIdx].mOffset ];

        KNL_LIST_FUNC_GET_LEFT_VALUE( sLeft,
                                      aInputArgument[sIdx].mType,
                                      sListContext,
                                      sLeftCast,
                                      aBlockIdx,
                                      aEnv  );
        sListContext =
            & aContextInfo->mContexts[ aInputArgument[sIdx + 1].mOffset ];

        KNL_LIST_FUNC_GET_RIGHT_VALUE( sRight,
                                       aInputArgument[sIdx + 1].mType,
                                       sListContext,
                                       aBlockIdx,
                                       aEnv  );               
        
        sIdx += 2;

        if( DTL_IS_NULL( sLeft ) || DTL_IS_NULL( sRight ) )
        {
            sIsNull = STL_TRUE;
        }
        else
        {
            sResultValue =
                ( dtlPhysicalFuncArg2FuncList[ sLeft->mType ][ sRight->mType ]
                  [ DTL_PHYSICAL_FUNC_IS_GREATER_THAN ]
                  ( sLeft->mValue,
                    sLeft->mLength,
                    sRight->mValue,
                    sRight->mLength ) );

            if( sResultValue == STL_TRUE )
            {
                *aIsResult = STL_TRUE;
                DTL_BOOLEAN( aResultValue ) = STL_TRUE;
                DTL_SET_BOOLEAN_LENGTH( aResultValue );

                STL_THROW( RAMP_LIST_EXIT ); 
            }
        }
    }
    *aIsResult = STL_FALSE;
    if( sIsNull == STL_TRUE )
    {
        DTL_SET_NULL( (dtlBoolean*)aResultValue );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = STL_FALSE;
        DTL_SET_BOOLEAN_LENGTH( aResultValue );
    }
    
    STL_RAMP( RAMP_LIST_EXIT );

    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief Greater Than Equal Any
 *        <BR> (expr) >= ANY (expr, expr)
 *        <BR> ( >= any ) Greater Than Any => [ P, O, M, S ]
 *        <BR> ex) (10) >= ANY (10,11)  =>  true
 *                                      <BR> (EXPR) >= ANY (VALUE, VALUE)
 * @param[in]  aInputArgumentCnt        input argument count
 * @param[in]  aRowValueCnt             Row Value Count
 * @param[in]  aInputArgument           input argument
 * @param[in]  aContextInfo             Context info
 * @param[in]  aBlockIdx                EvalInfo Block Idx
 * @param[out] aIsResult                List Function Result
 * @param[out] aResultValue             연산 결과 (결과타입 BOOLEAN)
 * @param[out] aEnv                     environment (stlErrorStack)
 */
stlStatus knlGreaterThanEqualAny( stlUInt16              aInputArgumentCnt,
                                  stlUInt16              aRowValueCnt,
                                  knlListEntry         * aInputArgument,
                                  knlExprContextInfo   * aContextInfo,
                                  stlInt32               aBlockIdx,
                                  stlBool              * aIsResult,
                                  void                 * aResultValue,
                                  void                 * aEnv )
{
    dtlDataValue       * sLeft;
    dtlDataValue       * sRight;
    knlExprContext     * sListContext = NULL;
    knlExprContext     * sLeftCast    = NULL;
    stlBool              sIsNull;
    stlInt32             sIdx         = 0;
    stlBool              sResultValue = STL_FALSE;
    
    *aIsResult = STL_FALSE;
    STL_PARAM_VALIDATE( aInputArgumentCnt <= KNL_LIST_FUNC_MAX_VALUE_CNT, KNL_ERROR_STACK( aEnv ) );

    sIsNull = (  DTL_IS_NULL( (dtlBoolean*)aResultValue ) ) ? STL_TRUE : STL_FALSE;
    
    while( sIdx < aInputArgumentCnt )
    {
        sListContext =
            & aContextInfo->mContexts[ aInputArgument[sIdx].mOffset ];

        KNL_LIST_FUNC_GET_LEFT_VALUE( sLeft,
                                      aInputArgument[sIdx].mType,
                                      sListContext,
                                      sLeftCast,
                                      aBlockIdx,
                                      aEnv  );
        sListContext =
            & aContextInfo->mContexts[ aInputArgument[sIdx + 1].mOffset ];

        KNL_LIST_FUNC_GET_RIGHT_VALUE( sRight,
                                       aInputArgument[sIdx + 1].mType,
                                       sListContext,
                                       aBlockIdx,
                                       aEnv  );               
        
        sIdx += 2;

        if( DTL_IS_NULL( sLeft ) || DTL_IS_NULL( sRight ) )
        {
            sIsNull = STL_TRUE;
        }
        else
        {
            sResultValue =
                ( dtlPhysicalFuncArg2FuncList[ sLeft->mType ][ sRight->mType ]
                  [ DTL_PHYSICAL_FUNC_IS_GREATER_THAN_EQUAL ]
                  ( sLeft->mValue,
                    sLeft->mLength,
                    sRight->mValue,
                    sRight->mLength ) );

            if( sResultValue == STL_TRUE )
            {
                *aIsResult = STL_TRUE;
                DTL_BOOLEAN( aResultValue ) = STL_TRUE;
                DTL_SET_BOOLEAN_LENGTH( aResultValue );

                STL_THROW( RAMP_LIST_EXIT ); 
            }
        }
    }
    *aIsResult = STL_FALSE;
    if( sIsNull == STL_TRUE )
    {
        DTL_SET_NULL( (dtlBoolean*)aResultValue );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = STL_FALSE;
        DTL_SET_BOOLEAN_LENGTH( aResultValue );
    }
    
    STL_RAMP( RAMP_LIST_EXIT );

    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * MULTI
 */

/**
 * @brief <> ANY
 *        <BR> (expr, expr) <> ANY ( (expr, expr), ( expr,expr) )
 *        <BR>   <> ANY => [ P, M, S ]
 *        <BR> ex) (9,11) <> ANY ( (10,11), ('11',12) )  =>  true
 *                                      <BR> (EXPR, EXPR) <> ANY ( (VALUE, VALUE), (VALUE ...) )
 * @param[in]  aInputArgumentCnt        input argument count
 * @param[in]  aRowValueCnt             Row Value Count
 * @param[in]  aInputArgument           input argument
 * @param[in]  aContextInfo             Context info
 * @param[in]  aBlockIdx                EvalInfo Block Idx
 * @param[out] aIsResult                List Function Result
 * @param[out] aResultValue             연산 결과 (결과타입 BOOLEAN)
 * @param[out] aEnv                     environment (stlErrorStack)
 */
stlStatus knlMultiNotEqualAny( stlUInt16              aInputArgumentCnt,
                               stlUInt16              aRowValueCnt,
                               knlListEntry         * aInputArgument,
                               knlExprContextInfo   * aContextInfo,
                               stlInt32               aBlockIdx,
                               stlBool              * aIsResult,
                               void                 * aResultValue,
                               void                 * aEnv )
{
    dtlDataValue       * sLeft;
    dtlDataValue       * sRight;
    knlExprContext     * sListContext = NULL;
    stlBool              sIsNull;
    stlInt32             sIdx         = 0;
    stlUInt16            sValueCount;
    stlBool              sResultValue = STL_FALSE;

    STL_PARAM_VALIDATE( aInputArgumentCnt <= KNL_LIST_FUNC_MAX_VALUE_CNT, KNL_ERROR_STACK( aEnv ) );

    sIsNull = (  DTL_IS_NULL( (dtlBoolean*)aResultValue ) ) ? STL_TRUE : STL_FALSE;

    while( sIdx < aInputArgumentCnt )
    {
        for( sValueCount = 0 ; sValueCount < aRowValueCnt ; sValueCount ++ )
        {
            sListContext =
                & aContextInfo->mContexts[ aInputArgument[sIdx].mOffset ];

            KNL_LIST_FUNC_GET_MULTI_VALUE( sLeft,
                                           aInputArgument[sIdx].mType,
                                           sListContext,
                                           aBlockIdx,
                                           aEnv  );
            sListContext =
                & aContextInfo->mContexts[ aInputArgument[sIdx + 1].mOffset ];

            KNL_LIST_FUNC_GET_MULTI_VALUE( sRight,
                                           aInputArgument[sIdx + 1].mType,
                                           sListContext,
                                           aBlockIdx,
                                           aEnv  );               
        
            sIdx += 2;
        
            if( DTL_IS_NULL( sLeft ) || DTL_IS_NULL( sRight ) )
            {
                sIsNull = STL_TRUE;
            }
            else
            {
                sResultValue =
                    ( dtlPhysicalFuncArg2FuncList[ sLeft->mType ][ sRight->mType ]
                      [ DTL_PHYSICAL_FUNC_IS_NOT_EQUAL ]
                      ( sLeft->mValue,
                        sLeft->mLength,
                        sRight->mValue,
                        sRight->mLength ) );

                if( sResultValue == STL_TRUE )
                {
                    *aIsResult = STL_TRUE;
                    DTL_BOOLEAN( aResultValue ) = STL_TRUE;
                    DTL_SET_BOOLEAN_LENGTH( aResultValue );
                    STL_THROW( RAMP_LIST_EXIT );
                }
                else
                {
                    /* Do Nothing */
                }
            }
        }
    }
    *aIsResult = STL_FALSE;
    if( sIsNull == STL_TRUE )
    {
        DTL_SET_NULL( (dtlBoolean*)aResultValue );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = STL_FALSE;
        DTL_SET_BOOLEAN_LENGTH( aResultValue );
    }
    
    STL_RAMP( RAMP_LIST_EXIT );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief < ANY
 *        <BR> (expr, expr) < ANY ( (expr, expr), ( expr,expr) )
 *        <BR>   < ANY => [ P, M, S ]
 *        <BR> ex) (9,11) < ANY ( (10,11), ('11',12) )  =>  true
 *                                      <BR> (EXPR, EXPR) < ANY ( (VALUE, VALUE), (VALUE ...) )
 * @param[in]  aInputArgumentCnt        input argument count
 * @param[in]  aRowValueCnt             Row Value Count
 * @param[in]  aInputArgument           input argument
 * @param[in]  aContextInfo             Context info
 * @param[in]  aBlockIdx                EvalInfo Block Idx
 * @param[out] aIsResult                List Function Result
 * @param[out] aResultValue             연산 결과 (결과타입 BOOLEAN)
 * @param[out] aEnv                     environment (stlErrorStack)
 */
stlStatus knlMultiLessThanAny( stlUInt16              aInputArgumentCnt,
                               stlUInt16              aRowValueCnt,
                               knlListEntry         * aInputArgument,
                               knlExprContextInfo   * aContextInfo,
                               stlInt32               aBlockIdx,
                               stlBool              * aIsResult,
                               void                 * aResultValue,
                               void                 * aEnv )
{
    stlBool              sIsNull;
    stlUInt16            sValueCount;
    stlUInt16            sInputArgumentCnt = aInputArgumentCnt;
    knlListEntry       * sInputArgument    = aInputArgument; 
    
    STL_PARAM_VALIDATE( aInputArgumentCnt <= KNL_LIST_FUNC_MAX_VALUE_CNT, KNL_ERROR_STACK( aEnv ) );

    sIsNull = (  DTL_IS_NULL( (dtlBoolean*)aResultValue ) ) ? STL_TRUE : STL_FALSE;

    sValueCount = aRowValueCnt * 2;
    while( 0 < sInputArgumentCnt )
    {
        STL_TRY( knlLessThanRow( sValueCount,
                                 aRowValueCnt,
                                 sInputArgument,
                                 aContextInfo,
                                 aBlockIdx,
                                 aIsResult,
                                 aResultValue,
                                 aEnv ) == STL_SUCCESS );

        if( DTL_IS_NULL( (dtlBoolean*)aResultValue ) == STL_TRUE )
        {
            sIsNull = STL_TRUE;
        }
        else
        {
            if( DTL_BOOLEAN( aResultValue ) == STL_TRUE )
            {
                STL_THROW( RAMP_LIST_EXIT ); 
            }
            else
            {
                /* Do Nothing */
            }
        }
        sInputArgumentCnt -= sValueCount;
        sInputArgument    += sValueCount;
    }
    *aIsResult = STL_FALSE;
    if( sIsNull == STL_TRUE )
    {
        DTL_SET_NULL( (dtlBoolean*)aResultValue );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = STL_FALSE;
        DTL_SET_BOOLEAN_LENGTH( aResultValue );
    }
    
    STL_RAMP( RAMP_LIST_EXIT );
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief < ANY
 *        <BR> (expr, expr) <= ANY ( (expr, expr), ( expr,expr) )
 *        <BR>   <= ANY => [ P, M, S ]
 *        <BR> ex) (9,11) <= ANY ( (10,11), ('11',12) )  =>  true
 *                                      <BR> (EXPR, EXPR) <= ANY ( (VALUE, VALUE), (VALUE ...) )
 * @param[in]  aInputArgumentCnt        input argument count
 * @param[in]  aRowValueCnt             Row Value Count
 * @param[in]  aInputArgument           input argument
 * @param[in]  aContextInfo             Context info
 * @param[in]  aBlockIdx                EvalInfo Block Idx
 * @param[out] aIsResult                List Function Result
 * @param[out] aResultValue             연산 결과 (결과타입 BOOLEAN)
 * @param[out] aEnv                     environment (stlErrorStack)
 */
stlStatus knlMultiLessThanEqualAny( stlUInt16              aInputArgumentCnt,
                                    stlUInt16              aRowValueCnt,
                                    knlListEntry         * aInputArgument,
                                    knlExprContextInfo   * aContextInfo,
                                    stlInt32               aBlockIdx,
                                    stlBool              * aIsResult,
                                    void                 * aResultValue,
                                    void                 * aEnv )
{
    stlBool              sIsNull;
    stlUInt16            sValueCount;
    stlUInt16            sInputArgumentCnt = aInputArgumentCnt;
    knlListEntry       * sInputArgument    = aInputArgument; 
    
    STL_PARAM_VALIDATE( aInputArgumentCnt <= KNL_LIST_FUNC_MAX_VALUE_CNT, KNL_ERROR_STACK( aEnv ) );

    sIsNull = (  DTL_IS_NULL( (dtlBoolean*)aResultValue ) ) ? STL_TRUE : STL_FALSE;

    sValueCount = aRowValueCnt * 2;
    while( 0 < sInputArgumentCnt )
    {
        STL_TRY( knlLessThanEqualRow( sValueCount,
                                      aRowValueCnt,
                                      sInputArgument,
                                      aContextInfo,
                                      aBlockIdx,
                                      aIsResult,
                                      aResultValue,
                                      aEnv ) == STL_SUCCESS );

        if( DTL_IS_NULL( (dtlBoolean*)aResultValue ) == STL_TRUE )
        {
            sIsNull = STL_TRUE;
        }
        else
        {
            if( DTL_BOOLEAN( aResultValue ) == STL_TRUE )
            {
                STL_THROW( RAMP_LIST_EXIT ); 
            }
            else
            {
                /* Do Nothing */
            }
        }
        sInputArgumentCnt -= sValueCount;
        sInputArgument    += sValueCount;
    }
    *aIsResult = STL_FALSE;
    if( sIsNull == STL_TRUE )
    {
        DTL_SET_NULL( (dtlBoolean*)aResultValue );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = STL_FALSE;
        DTL_SET_BOOLEAN_LENGTH( aResultValue );
    }
    
    STL_RAMP( RAMP_LIST_EXIT );
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief > ANY
 *        <BR> (expr, expr) > ANY ( (expr, expr), ( expr,expr) )
 *        <BR>   > ANY => [ P, M, S ]
 *        <BR> ex) (13,15) > ANY ( (10,11), ('11',12) )  =>  true
 *                                      <BR> (EXPR, EXPR) > ANY ( (VALUE, VALUE), (VALUE ...) )
 * @param[in]  aInputArgumentCnt        input argument count
 * @param[in]  aRowValueCnt             Row Value Count
 * @param[in]  aInputArgument           input argument
 * @param[in]  aContextInfo             Context info
 * @param[in]  aBlockIdx                EvalInfo Block Idx
 * @param[out] aIsResult                List Function Result
 * @param[out] aResultValue             연산 결과 (결과타입 BOOLEAN)
 * @param[out] aEnv                     environment (stlErrorStack)
 */
stlStatus knlMultiGreaterThanAny( stlUInt16              aInputArgumentCnt,
                                  stlUInt16              aRowValueCnt,
                                  knlListEntry         * aInputArgument,
                                  knlExprContextInfo   * aContextInfo,
                                  stlInt32               aBlockIdx,
                                  stlBool              * aIsResult,
                                  void                 * aResultValue,
                                  void                 * aEnv )
{
    stlBool              sIsNull;
    stlUInt16            sValueCount;
    stlUInt16            sInputArgumentCnt = aInputArgumentCnt;
    knlListEntry       * sInputArgument    = aInputArgument; 
    
    STL_PARAM_VALIDATE( aInputArgumentCnt <= KNL_LIST_FUNC_MAX_VALUE_CNT, KNL_ERROR_STACK( aEnv ) );

    sIsNull = (  DTL_IS_NULL( (dtlBoolean*)aResultValue ) ) ? STL_TRUE : STL_FALSE;

    sValueCount = aRowValueCnt * 2;
    while( 0 < sInputArgumentCnt )
    {
        STL_TRY( knlGreaterThanRow( sValueCount,
                                    aRowValueCnt,
                                    sInputArgument,
                                    aContextInfo,
                                    aBlockIdx,
                                    aIsResult,
                                    aResultValue,
                                    aEnv ) == STL_SUCCESS );

        if( DTL_IS_NULL( (dtlBoolean*)aResultValue ) == STL_TRUE )
        {
            sIsNull = STL_TRUE;
        }
        else
        {
            if( DTL_BOOLEAN( aResultValue ) == STL_TRUE )
            {
                STL_THROW( RAMP_LIST_EXIT ); 
            }
            else
            {
                /* Do Nothing */
            }
        }
        sInputArgumentCnt -= sValueCount;
        sInputArgument    += sValueCount;
    }
    *aIsResult = STL_FALSE;
    if( sIsNull == STL_TRUE )
    {
        DTL_SET_NULL( (dtlBoolean*)aResultValue );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = STL_FALSE;
        DTL_SET_BOOLEAN_LENGTH( aResultValue );
    }
    
    STL_RAMP( RAMP_LIST_EXIT );
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief > ANY
 *        <BR> (expr, expr) >= ANY ( (expr, expr), ( expr,expr) )
 *        <BR>   >= ANY => [ P, M, S ]
 *        <BR> ex) (12,11) >= ANY ( (10,11), ('11',12) )  =>  true
 *                                      <BR> (EXPR, EXPR) >= ANY ( (VALUE, VALUE), (VALUE ...) )
 * @param[in]  aInputArgumentCnt        input argument count
 * @param[in]  aRowValueCnt             Row Value Count
 * @param[in]  aInputArgument           input argument
 * @param[in]  aContextInfo             Context info
 * @param[in]  aBlockIdx                EvalInfo Block Idx
 * @param[out] aIsResult                List Function Result
 * @param[out] aResultValue             연산 결과 (결과타입 BOOLEAN)
 * @param[out] aEnv                     environment (stlErrorStack)
 */
stlStatus knlMultiGreaterThanEqualAny( stlUInt16              aInputArgumentCnt,
                                       stlUInt16              aRowValueCnt,
                                       knlListEntry         * aInputArgument,
                                       knlExprContextInfo   * aContextInfo,
                                       stlInt32               aBlockIdx,
                                       stlBool              * aIsResult,
                                       void                 * aResultValue,
                                       void                 * aEnv )
{
    stlBool              sIsNull;
    stlUInt16            sValueCount;
    stlUInt16            sInputArgumentCnt = aInputArgumentCnt;
    knlListEntry       * sInputArgument    = aInputArgument; 
    
    STL_PARAM_VALIDATE( aInputArgumentCnt <= KNL_LIST_FUNC_MAX_VALUE_CNT, KNL_ERROR_STACK( aEnv ) );

    sIsNull = (  DTL_IS_NULL( (dtlBoolean*)aResultValue ) ) ? STL_TRUE : STL_FALSE;

    sValueCount = aRowValueCnt * 2;
    while( 0 < sInputArgumentCnt )
    {
        STL_TRY( knlGreaterThanEqualRow( sValueCount,
                                         aRowValueCnt,
                                         sInputArgument,
                                         aContextInfo,
                                         aBlockIdx,
                                         aIsResult,
                                         aResultValue,
                                         aEnv ) == STL_SUCCESS );

        if( DTL_IS_NULL( (dtlBoolean*)aResultValue ) == STL_TRUE )
        {
            sIsNull = STL_TRUE;
        }
        else
        {
            if( DTL_BOOLEAN( aResultValue ) == STL_TRUE )
            {
                STL_THROW( RAMP_LIST_EXIT ); 
            }
            else
            {
                /* Do Nothing */
            }
        }
        sInputArgumentCnt -= sValueCount;
        sInputArgument    += sValueCount;
    }
    *aIsResult = STL_FALSE;
    if( sIsNull == STL_TRUE )
    {
        DTL_SET_NULL( (dtlBoolean*)aResultValue );
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = STL_FALSE;
        DTL_SET_BOOLEAN_LENGTH( aResultValue );
    }
    
    STL_RAMP( RAMP_LIST_EXIT );
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/************************************************
 * LIST OPERATION ( ALL )
 ************************************************/

/*******************************************************************************************
 * LIST OPERATION
 * GET FUNCTION POINTER 
 ******************************************************************************************/

/**
 * @brief  Equal All pointer 얻기
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus knlGetListFuncPtrEqualAll( knlBuiltInListFuncPtr   * aFuncPtr,
                                     stlErrorStack           * aErrorStack )
{
    /**
     * output 설정
     */
    
    *aFuncPtr = knlEqualAll;
    
    return STL_SUCCESS;
}

/**
 * @brief Not Equal All pointer 얻기
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus knlGetListFuncPtrNotEqualAll( knlBuiltInListFuncPtr   * aFuncPtr,
                                        stlErrorStack           * aErrorStack )
{
    /**
     * output 설정
     */
    
    *aFuncPtr = knlNotEqualAll;
    
    return STL_SUCCESS;
}

/**
 * @brief LessThanAll pointer 얻기
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus knlGetListFuncPtrLessThanAll( knlBuiltInListFuncPtr   * aFuncPtr,
                                        stlErrorStack           * aErrorStack )
{
    /**
     * output 설정
     */
    
    *aFuncPtr = knlLessThanAll;
    
    return STL_SUCCESS;
}

/**
 * @brief LessThanEqualAll pointer 얻기
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus knlGetListFuncPtrLessThanEqualAll( knlBuiltInListFuncPtr   * aFuncPtr,
                                             stlErrorStack           * aErrorStack )
{
    /**
     * output 설정
     */
    
    *aFuncPtr = knlLessThanEqualAll;
    
    return STL_SUCCESS;
}

/**
 * @brief GreaterThanAll pointer 얻기
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus knlGetListFuncPtrGreaterThanAll( knlBuiltInListFuncPtr   * aFuncPtr,
                                           stlErrorStack           * aErrorStack )
{
    /**
     * output 설정
     */
    
    *aFuncPtr = knlGreaterThanAll;
    
    return STL_SUCCESS;
}

/**
 * @brief GreaterThanEqualAll pointer 얻기
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus knlGetListFuncPtrGreaterThanEqualAll( knlBuiltInListFuncPtr   * aFuncPtr,
                                                stlErrorStack           * aErrorStack )
{
    /**
     * output 설정
     */
    
    *aFuncPtr = knlGreaterThanEqualAll;
    
    return STL_SUCCESS;
}

/**
 * MULTI
 */

/**
 * @brief Multi  Equal All pointer 얻기
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus knlGetListFuncPtrMultiEqualAll( knlBuiltInListFuncPtr   * aFuncPtr,
                                          stlErrorStack           * aErrorStack )
{
    /**
     * output 설정
     */
    
    *aFuncPtr = knlMultiEqualAll;
    
    return STL_SUCCESS;
}


/**
 * @brief Multi Not Equal All pointer 얻기
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus knlGetListFuncPtrMultiNotEqualAll( knlBuiltInListFuncPtr   * aFuncPtr,
                                             stlErrorStack           * aErrorStack )
{
    /**
     * output 설정
     */
    
    *aFuncPtr = knlMultiNotEqualAll;
    
    return STL_SUCCESS;
}

/**
 * @brief Multi Less Than All pointer 얻기
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus knlGetListFuncPtrMultiLessThanAll( knlBuiltInListFuncPtr   * aFuncPtr,
                                             stlErrorStack           * aErrorStack )
{
    /**
     * output 설정
     */
    
    *aFuncPtr = knlMultiLessThanAll;
    
    return STL_SUCCESS;
}

/**
 * @brief Multi Less Than Equal All pointer 얻기
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus knlGetListFuncPtrMultiLessThanEqualAll( knlBuiltInListFuncPtr   * aFuncPtr,
                                                  stlErrorStack           * aErrorStack )
{
    /**
     * output 설정
     */
    
    *aFuncPtr = knlMultiLessThanEqualAll;
    
    return STL_SUCCESS;
}

/**
 * @brief Multi Greater Than All pointer 얻기
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus knlGetListFuncPtrMultiGreaterThanAll( knlBuiltInListFuncPtr   * aFuncPtr,
                                                stlErrorStack           * aErrorStack )
{
    /**
     * output 설정
     */
    
    *aFuncPtr = knlMultiGreaterThanAll;
    
    return STL_SUCCESS;
}

/**
 * @brief Multi Greater Than Equal All pointer 얻기
 * @param[out] aFuncPtr          function pointer
 * @param[out] aErrorStack       에러 스택
 */
stlStatus knlGetListFuncPtrMultiGreaterThanEqualAll( knlBuiltInListFuncPtr   * aFuncPtr,
                                                     stlErrorStack           * aErrorStack )
{
    /**
     * output 설정
     */
    
    *aFuncPtr = knlMultiGreaterThanEqualAll;
    
    return STL_SUCCESS;
}

/**
 * @brief  Equal Any
 *        <BR> (expr) = ALL (expr, expr)
 *        <BR> ( = all ) Equal all => [ P, O, M, S ]
 *        <BR> ex) (9) = ALL (9,9)  =>  true
 *                                      <BR> (EXPR) = ALL (VALUE, VALUE)
 * @param[in]  aInputArgumentCnt        input argument count
 * @param[in]  aRowValueCnt             Row Value Count
 * @param[in]  aInputArgument           input argument
 * @param[in]  aContextInfo             Context info
 * @param[in]  aBlockIdx                EvalInfo Block Idx
 * @param[out] aIsResult                List Function Result
 * @param[out] aResultValue             연산 결과 (결과타입 BOOLEAN)
 * @param[out] aEnv                     environment (stlErrorStack)
 */
stlStatus knlEqualAll( stlUInt16              aInputArgumentCnt,
                       stlUInt16              aRowValueCnt,
                       knlListEntry         * aInputArgument,
                       knlExprContextInfo   * aContextInfo,
                       stlInt32               aBlockIdx,
                       stlBool              * aIsResult,
                       void                 * aResultValue,
                       void                 * aEnv )
{
    STL_TRY( knlNotEqualAny( aInputArgumentCnt,
                             aRowValueCnt,
                             aInputArgument,
                             aContextInfo,
                             aBlockIdx,
                             aIsResult,
                             aResultValue,
                             aEnv ) == STL_SUCCESS );

    if( DTL_IS_NULL( (dtlBoolean*)aResultValue ) == STL_TRUE )
    {
        /* Do Nothing */
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( ( DTL_BOOLEAN( aResultValue ) == STL_TRUE ) ?
                                        STL_FALSE : STL_TRUE );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief  Not Equal Any
 *        <BR> (expr) <> ALL (expr, expr)
 *        <BR> ( <> all ) Not Equal all => [ P, O, M, S ]
 *        <BR> ex) (9) <> ALL (1,2)  =>  true
 *                                      <BR> (EXPR) <> ALL (VALUE, VALUE)
 * @param[in]  aInputArgumentCnt        input argument count
 * @param[in]  aRowValueCnt             Row Value Count
 * @param[in]  aInputArgument           input argument
 * @param[in]  aContextInfo             Context info
 * @param[in]  aBlockIdx                EvalInfo Block Idx
 * @param[out] aIsResult                List Function Result
 * @param[out] aResultValue             연산 결과 (결과타입 BOOLEAN)
 * @param[out] aEnv                     environment (stlErrorStack)
 */
stlStatus knlNotEqualAll( stlUInt16              aInputArgumentCnt,
                          stlUInt16              aRowValueCnt,
                          knlListEntry         * aInputArgument,
                          knlExprContextInfo   * aContextInfo,
                          stlInt32               aBlockIdx,
                          stlBool              * aIsResult,
                          void                 * aResultValue,
                          void                 * aEnv )
{
    STL_TRY( knlSingleIn( aInputArgumentCnt,
                          aRowValueCnt,
                          aInputArgument,
                          aContextInfo,
                          aBlockIdx,
                          aIsResult,
                          aResultValue,
                          aEnv ) == STL_SUCCESS );

    if( DTL_IS_NULL( (dtlBoolean*)aResultValue ) == STL_TRUE )
    {
        /* Do Nothing */
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( ( DTL_BOOLEAN( aResultValue ) == STL_TRUE ) ?
                                        STL_FALSE : STL_TRUE );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief  Less Than All
 *        <BR> (expr) < ALL (expr, expr)
 *        <BR> ( < all ) Less Than all => [ P, O, M, S ]
 *        <BR> ex) (0) < ALL (1,2)  =>  true
 *                                      <BR> (EXPR) < ALL (VALUE, VALUE)
 * @param[in]  aInputArgumentCnt        input argument count
 * @param[in]  aRowValueCnt             Row Value Count
 * @param[in]  aInputArgument           input argument
 * @param[in]  aContextInfo             Context info
 * @param[in]  aBlockIdx                EvalInfo Block Idx
 * @param[out] aIsResult                List Function Result
 * @param[out] aResultValue             연산 결과 (결과타입 BOOLEAN)
 * @param[out] aEnv                     environment (stlErrorStack)
 */
stlStatus knlLessThanAll( stlUInt16              aInputArgumentCnt,
                          stlUInt16              aRowValueCnt,
                          knlListEntry         * aInputArgument,
                          knlExprContextInfo   * aContextInfo,
                          stlInt32               aBlockIdx,
                          stlBool              * aIsResult,
                          void                 * aResultValue,
                          void                 * aEnv )
{
    STL_TRY( knlGreaterThanEqualAny( aInputArgumentCnt,
                                     aRowValueCnt,
                                     aInputArgument,
                                     aContextInfo,
                                     aBlockIdx,
                                     aIsResult,
                                     aResultValue,
                                     aEnv ) == STL_SUCCESS );

    if( DTL_IS_NULL( (dtlBoolean*)aResultValue ) == STL_TRUE )
    {
        /* Do Nothing */
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( ( DTL_BOOLEAN( aResultValue ) == STL_TRUE ) ?
                                        STL_FALSE : STL_TRUE );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief  Less Than Equal All
 *        <BR> (expr) <= ALL (expr, expr)
 *        <BR> ( <= all ) Less Than Equal all => [ P, O, M, S ]
 *        <BR> ex) (1) <= ALL (1,2)  =>  true
 *                                      <BR> (EXPR) <= ALL (VALUE, VALUE)
 * @param[in]  aInputArgumentCnt        input argument count
 * @param[in]  aRowValueCnt             Row Value Count
 * @param[in]  aInputArgument           input argument
 * @param[in]  aContextInfo             Context info
 * @param[in]  aBlockIdx                EvalInfo Block Idx
 * @param[out] aIsResult                List Function Result
 * @param[out] aResultValue             연산 결과 (결과타입 BOOLEAN)
 * @param[out] aEnv                     environment (stlErrorStack)
 */
stlStatus knlLessThanEqualAll( stlUInt16              aInputArgumentCnt,
                               stlUInt16              aRowValueCnt,
                               knlListEntry         * aInputArgument,
                               knlExprContextInfo   * aContextInfo,
                               stlInt32               aBlockIdx,
                               stlBool              * aIsResult,
                               void                 * aResultValue,
                               void                 * aEnv )
{

    STL_TRY( knlGreaterThanAny( aInputArgumentCnt,
                                aRowValueCnt,
                                aInputArgument,
                                aContextInfo,
                                aBlockIdx,
                                aIsResult,
                                aResultValue,
                                aEnv ) == STL_SUCCESS );

    if( DTL_IS_NULL( (dtlBoolean*)aResultValue ) == STL_TRUE )
    {
        /* Do Nothing */
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( ( DTL_BOOLEAN( aResultValue ) == STL_TRUE ) ?
                                        STL_FALSE : STL_TRUE );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief  Greater Than All
 *        <BR> (expr) > ALL (expr, expr)
 *        <BR> ( > all ) Greater Than all => [ P, O, M, S ]
 *        <BR> ex) (3) > ALL (1,2)  =>  true
 *                                      <BR> (EXPR) > ALL (VALUE, VALUE)
 * @param[in]  aInputArgumentCnt        input argument count
 * @param[in]  aRowValueCnt             Row Value Count
 * @param[in]  aInputArgument           input argument
 * @param[in]  aContextInfo             Context info
 * @param[in]  aBlockIdx                EvalInfo Block Idx
 * @param[out] aIsResult                List Function Result
 * @param[out] aResultValue             연산 결과 (결과타입 BOOLEAN)
 * @param[out] aEnv                     environment (stlErrorStack)
 */
stlStatus knlGreaterThanAll( stlUInt16              aInputArgumentCnt,
                             stlUInt16              aRowValueCnt,
                             knlListEntry         * aInputArgument,
                             knlExprContextInfo   * aContextInfo,
                             stlInt32               aBlockIdx,
                             stlBool              * aIsResult,
                             void                 * aResultValue,
                             void                 * aEnv )
{
    STL_TRY( knlLessThanEqualAny( aInputArgumentCnt,
                                  aRowValueCnt,
                                  aInputArgument,
                                  aContextInfo,
                                  aBlockIdx,
                                  aIsResult,
                                  aResultValue,
                                  aEnv ) == STL_SUCCESS );

    if( DTL_IS_NULL( (dtlBoolean*)aResultValue ) == STL_TRUE )
    {
        /* Do Nothing */
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( ( DTL_BOOLEAN( aResultValue ) == STL_TRUE ) ?
                                        STL_FALSE : STL_TRUE );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief  Greater Than Equal All
 *        <BR> (expr) >= ALL (expr, expr)
 *        <BR> ( >= all ) Greater Than Equal all => [ P, O, M, S ]
 *        <BR> ex) (3) >= ALL (1,2)  =>  true
 *                                      <BR> (EXPR) >= ALL (VALUE, VALUE)
 * @param[in]  aInputArgumentCnt        input argument count
 * @param[in]  aRowValueCnt             Row Value Count
 * @param[in]  aInputArgument           input argument
 * @param[in]  aContextInfo             Context info
 * @param[in]  aBlockIdx                EvalInfo Block Idx
 * @param[out] aIsResult                List Function Result
 * @param[out] aResultValue             연산 결과 (결과타입 BOOLEAN)
 * @param[out] aEnv                     environment (stlErrorStack)
 */
stlStatus knlGreaterThanEqualAll( stlUInt16              aInputArgumentCnt,
                                  stlUInt16              aRowValueCnt,
                                  knlListEntry         * aInputArgument,
                                  knlExprContextInfo   * aContextInfo,
                                  stlInt32               aBlockIdx,
                                  stlBool              * aIsResult,
                                  void                 * aResultValue,
                                  void                 * aEnv )
{
    STL_TRY( knlLessThanAny( aInputArgumentCnt,
                             aRowValueCnt,
                             aInputArgument,
                             aContextInfo,
                             aBlockIdx,
                             aIsResult,
                             aResultValue,
                             aEnv ) == STL_SUCCESS );

    if( DTL_IS_NULL( (dtlBoolean*)aResultValue ) == STL_TRUE )
    {
        /* Do Nothing */
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( ( DTL_BOOLEAN( aResultValue ) == STL_TRUE ) ?
                                        STL_FALSE : STL_TRUE );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * MULTI
 */

/**
 * @brief Multi Equal Any
 *        <BR> (expr, expr) = ALL ((expr, expr), (expr, expr ) ) 
 *        <BR> ( = all ) Equal all => [ P, M, S ]
 *        <BR> ex) (9,10) = ALL ( (9,10), (9,10) )  =>  true
 *                                      <BR> (EXPR, EXPR) = ALL ((EXPR, EXPR), (EXPR, .. ) )
 * @param[in]  aInputArgumentCnt        input argument count
 * @param[in]  aRowValueCnt             Row Value Count
 * @param[in]  aInputArgument           input argument
 * @param[in]  aContextInfo             Context info
 * @param[in]  aBlockIdx                EvalInfo Block Idx
 * @param[out] aIsResult                List Function Result
 * @param[out] aResultValue             연산 결과 (결과타입 BOOLEAN)
 * @param[out] aEnv                     environment (stlErrorStack)
 */
stlStatus knlMultiEqualAll( stlUInt16              aInputArgumentCnt,
                            stlUInt16              aRowValueCnt,
                            knlListEntry         * aInputArgument,
                            knlExprContextInfo   * aContextInfo,
                            stlInt32               aBlockIdx,
                            stlBool              * aIsResult,
                            void                 * aResultValue,
                            void                 * aEnv )
{
    STL_TRY( knlMultiNotEqualAny( aInputArgumentCnt,
                                  aRowValueCnt,
                                  aInputArgument,
                                  aContextInfo,
                                  aBlockIdx,
                                  aIsResult,
                                  aResultValue,
                                  aEnv ) == STL_SUCCESS );

    if( DTL_IS_NULL( (dtlBoolean*)aResultValue ) == STL_TRUE )
    {
        /* Do Nothing */
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( ( DTL_BOOLEAN( aResultValue ) == STL_TRUE ) ?
                                        STL_FALSE : STL_TRUE );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Multi Not Equal Any
 *        <BR> (expr, expr) <> ALL ((expr, expr), (expr, expr ) ) 
 *        <BR> ( <> all ) Not Equal all => [ P, M, S ]
 *        <BR> ex) (9,10) <> ALL ( (0,0), (1,1) )  =>  true
 *                                      <BR> (EXPR, EXPR) <> ALL ((EXPR, EXPR), (EXPR, .. ) )
 * @param[in]  aInputArgumentCnt        input argument count
 * @param[in]  aRowValueCnt             Row Value Count
 * @param[in]  aInputArgument           input argument
 * @param[in]  aContextInfo             Context info
 * @param[in]  aBlockIdx                EvalInfo Block Idx
 * @param[out] aIsResult                List Function Result
 * @param[out] aResultValue             연산 결과 (결과타입 BOOLEAN)
 * @param[out] aEnv                     environment (stlErrorStack)
 */
stlStatus knlMultiNotEqualAll( stlUInt16              aInputArgumentCnt,
                               stlUInt16              aRowValueCnt,
                               knlListEntry         * aInputArgument,
                               knlExprContextInfo   * aContextInfo,
                               stlInt32               aBlockIdx,
                               stlBool              * aIsResult,
                               void                 * aResultValue,
                               void                 * aEnv )
{
    STL_TRY( knlMultiIn( aInputArgumentCnt,
                         aRowValueCnt,
                         aInputArgument,
                         aContextInfo,
                         aBlockIdx,
                         aIsResult,
                         aResultValue,
                         aEnv ) == STL_SUCCESS );

    if( DTL_IS_NULL( (dtlBoolean*)aResultValue ) == STL_TRUE )
    {
        /* Do Nothing */
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( ( DTL_BOOLEAN( aResultValue ) == STL_TRUE ) ?
                                        STL_FALSE : STL_TRUE );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief Multi Less Than All
 *        <BR> (expr, expr) < ALL ((expr, expr), (expr, expr ) ) 
 *        <BR> ( < all ) Less Than all => [ P, M, S ]
 *        <BR> ex) (9,10) < ALL ( (10,11), (12,13) )  =>  true
 *                                      <BR> (EXPR, EXPR) < ALL ((EXPR, EXPR), (EXPR, .. ) )
 * @param[in]  aInputArgumentCnt        input argument count
 * @param[in]  aRowValueCnt             Row Value Count
 * @param[in]  aInputArgument           input argument
 * @param[in]  aContextInfo             Context info
 * @param[in]  aBlockIdx                EvalInfo Block Idx
 * @param[out] aIsResult                List Function Result
 * @param[out] aResultValue             연산 결과 (결과타입 BOOLEAN)
 * @param[out] aEnv                     environment (stlErrorStack)
 */
stlStatus knlMultiLessThanAll( stlUInt16              aInputArgumentCnt,
                               stlUInt16              aRowValueCnt,
                               knlListEntry         * aInputArgument,
                               knlExprContextInfo   * aContextInfo,
                               stlInt32               aBlockIdx,
                               stlBool              * aIsResult,
                               void                 * aResultValue,
                               void                 * aEnv )
{
    STL_TRY( knlMultiGreaterThanEqualAny( aInputArgumentCnt,
                                          aRowValueCnt,
                                          aInputArgument,
                                          aContextInfo,
                                          aBlockIdx,
                                          aIsResult,
                                          aResultValue,
                                          aEnv ) == STL_SUCCESS );

    if( DTL_IS_NULL( (dtlBoolean*)aResultValue ) == STL_TRUE )
    {
        /* Do Nothing */
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( ( DTL_BOOLEAN( aResultValue ) == STL_TRUE ) ?
                                        STL_FALSE : STL_TRUE );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
    
}



/**
 * @brief Multi Less Than Equal All
 *        <BR> (expr, expr) <= ALL ((expr, expr), (expr, expr ) ) 
 *        <BR> ( <= all ) Less Than Equal all => [ P, M, S ]
 *        <BR> ex) (10,11) <= ALL ( (10,11), (12,13) )  =>  true
 *                                      <BR> (EXPR, EXPR) <= ALL ((EXPR, EXPR), (EXPR, .. ) )
 * @param[in]  aInputArgumentCnt        input argument count
 * @param[in]  aRowValueCnt             Row Value Count
 * @param[in]  aInputArgument           input argument
 * @param[in]  aContextInfo             Context info
 * @param[in]  aBlockIdx                EvalInfo Block Idx
 * @param[out] aIsResult                List Function Result
 * @param[out] aResultValue             연산 결과 (결과타입 BOOLEAN)
 * @param[out] aEnv                     environment (stlErrorStack)
 */
stlStatus knlMultiLessThanEqualAll( stlUInt16              aInputArgumentCnt,
                                    stlUInt16              aRowValueCnt,
                                    knlListEntry         * aInputArgument,
                                    knlExprContextInfo   * aContextInfo,
                                    stlInt32               aBlockIdx,
                                    stlBool              * aIsResult,
                                    void                 * aResultValue,
                                    void                 * aEnv )
{
    STL_TRY( knlMultiGreaterThanAny( aInputArgumentCnt,
                                     aRowValueCnt,
                                     aInputArgument,
                                     aContextInfo,
                                     aBlockIdx,
                                     aIsResult,
                                     aResultValue,
                                     aEnv ) == STL_SUCCESS );

    if( DTL_IS_NULL( (dtlBoolean*)aResultValue ) == STL_TRUE )
    {
        /* Do Nothing */
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( ( DTL_BOOLEAN( aResultValue ) == STL_TRUE ) ?
                                        STL_FALSE : STL_TRUE );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Multi Greater Than All
 *        <BR> (expr, expr) > ALL ((expr, expr), (expr, expr ) ) 
 *        <BR> ( > all ) Greater Than all => [ P, M, S ]
 *        <BR> ex) (13,14) > ALL ( (10,11), (12,13) )  =>  true
 *                                      <BR> (EXPR, EXPR) > ALL ((EXPR, EXPR), (EXPR, .. ) )
 * @param[in]  aInputArgumentCnt        input argument count
 * @param[in]  aRowValueCnt             Row Value Count
 * @param[in]  aInputArgument           input argument
 * @param[in]  aContextInfo             Context info
 * @param[in]  aBlockIdx                EvalInfo Block Idx
 * @param[out] aIsResult                List Function Result
 * @param[out] aResultValue             연산 결과 (결과타입 BOOLEAN)
 * @param[out] aEnv                     environment (stlErrorStack)
 */
stlStatus knlMultiGreaterThanAll( stlUInt16              aInputArgumentCnt,
                                  stlUInt16              aRowValueCnt,
                                  knlListEntry         * aInputArgument,
                                  knlExprContextInfo   * aContextInfo,
                                  stlInt32               aBlockIdx,
                                  stlBool              * aIsResult,
                                  void                 * aResultValue,
                                  void                 * aEnv )
{
    STL_TRY( knlMultiLessThanEqualAny( aInputArgumentCnt,
                                       aRowValueCnt,
                                       aInputArgument,
                                       aContextInfo,
                                       aBlockIdx,
                                       aIsResult,
                                       aResultValue,
                                       aEnv ) == STL_SUCCESS );

    if( DTL_IS_NULL( (dtlBoolean*)aResultValue ) == STL_TRUE )
    {
        /* Do Nothing */
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( ( DTL_BOOLEAN( aResultValue ) == STL_TRUE ) ?
                                        STL_FALSE : STL_TRUE );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
    
}

/**
 * @brief Multi Greater Than Equal All
 *        <BR> (expr, expr) >= ALL ((expr, expr), (expr, expr ) ) 
 *        <BR> ( >= all ) Greater Than Equal all => [ P, M, S ]
 *        <BR> ex) (13,14) >= ALL ( (10,11), (12,13) )  =>  true
 *                                      <BR> (EXPR, EXPR) >= ALL ((EXPR, EXPR), (EXPR, .. ) )
 * @param[in]  aInputArgumentCnt        input argument count
 * @param[in]  aRowValueCnt             Row Value Count
 * @param[in]  aInputArgument           input argument
 * @param[in]  aContextInfo             Context info
 * @param[in]  aBlockIdx                EvalInfo Block Idx
 * @param[out] aIsResult                List Function Result
 * @param[out] aResultValue             연산 결과 (결과타입 BOOLEAN)
 * @param[out] aEnv                     environment (stlErrorStack)
 */
stlStatus knlMultiGreaterThanEqualAll( stlUInt16              aInputArgumentCnt,
                                       stlUInt16              aRowValueCnt,
                                       knlListEntry         * aInputArgument,
                                       knlExprContextInfo   * aContextInfo,
                                       stlInt32               aBlockIdx,
                                       stlBool              * aIsResult,
                                       void                 * aResultValue,
                                       void                 * aEnv )
{
    STL_TRY( knlMultiLessThanAny( aInputArgumentCnt,
                                  aRowValueCnt,
                                  aInputArgument,
                                  aContextInfo,
                                  aBlockIdx,
                                  aIsResult,
                                  aResultValue,
                                  aEnv ) == STL_SUCCESS );

    if( DTL_IS_NULL( (dtlBoolean*)aResultValue ) == STL_TRUE )
    {
        /* Do Nothing */
    }
    else
    {
        DTL_BOOLEAN( aResultValue ) = ( ( DTL_BOOLEAN( aResultValue ) == STL_TRUE ) ?
                                        STL_FALSE : STL_TRUE );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
    
}


/** @} knlFunction */
