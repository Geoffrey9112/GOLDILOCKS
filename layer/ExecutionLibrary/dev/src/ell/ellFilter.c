/*******************************************************************************
 * ellFilter.c
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
 * @file ellFilter.c
 * @brief SCAN & FILTER 관리 모듈
 */

#include <ell.h>

/**
 * @addtogroup ellFilter
 * @{
 */



/**
 * @brief filter 생성
 * @param[in]      aFuncID          function ID 
 * @param[in]      aArgList         function arguments
 * @param[out]     aFilterPredList  filter predicate 
 * @param[out]     aFilterContext   context for filter
 * @param[in]      aMemMgr          영역 기반 메모리 할당자 (knlRegionMem)
 * @param[in,out]  aEnv             Environment
 *
 * @remark dictionary table 검색을 위한 함수
 */
stlStatus ellMakeOneFilter( knlBuiltInFunc         aFuncID,
                            knlValueBlockList    * aArgList,
                            knlPredicateList    ** aFilterPredList,
                            knlExprContextInfo  ** aFilterContext,
                            knlRegionMem         * aMemMgr,  
                            ellEnv               * aEnv )
{
    knlPredicateList    * sPredList      = NULL;
    knlExprStack        * sFilter        = NULL;
    knlExprContextInfo  * sFilterContext = NULL;
    knlExprContext      * sContext       = NULL;
    knlFunctionData     * sFuncData      = NULL;
    knlValueBlockList   * sArgList       = NULL;
    dtlBuiltInFuncInfo  * sFuncInfo      = NULL;

    stlUInt16             sArgIdx       = 0;
    stlUInt16             sArgCount     = 0;
    stlUInt32             sJumpEntryNo  = 0;
    stlUInt32             sGroupEntryNo = 0;
    stlUInt32             sFuncPtrIdx   = 0;
    
    stlBool               sIsConstantData[ DTL_FUNC_INPUTARG_VARIABLE_CNT ];

    dtlDataType           sResultType   = DTL_TYPE_NA;
    dtlDataType           sInputArgDataTypes[ DTL_FUNC_INPUTARG_VARIABLE_CNT ];
    dtlDataType           sResultArgDataTypes[ DTL_FUNC_INPUTARG_VARIABLE_CNT ];
    
    dtlDataTypeDefInfo    sResultTypeDefInfo;
    dtlDataTypeDefInfo    sInputArgDataTypeDefInfo[ DTL_FUNC_INPUTARG_VARIABLE_CNT ];
    dtlDataTypeDefInfo    sResultArgDataTypeDefInfo[ DTL_FUNC_INPUTARG_VARIABLE_CNT ];

    stlInt64              sBufferSize = 0;

    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( ( aFuncID >= KNL_BUILTIN_FUNC_IS_EQUAL ) &&
                        ( aFuncID < KNL_BUILTIN_FUNC_IS_NOT_NULL ),
                        ELL_ERROR_STACK(aEnv) );

    
    /**
     * filter context 생성
     */

    sArgList  = aArgList;
    sArgCount = 0;
    
    while( sArgList )
    {
        STL_ASSERT( sArgCount < DTL_FUNC_INPUTARG_VARIABLE_CNT );
        sArgCount++;
        sArgList = sArgList->mNext;
    }
    
    STL_TRY( knlCreateContextInfo( sArgCount + 1,
                                   & sFilterContext,
                                   aMemMgr,
                                   KNL_ENV( aEnv ) )
             == STL_SUCCESS );


    /**
     * function 분석
     */
    
    sFuncInfo = & gBuiltInFuncInfoArr[ aFuncID ];
    sArgList  = aArgList;
    sArgCount = 0;
    
    while( sArgList )
    {
        /* Data Type 정보 얻기 */
        sIsConstantData[ sArgCount ] = STL_FALSE;
        sInputArgDataTypes[ sArgCount ] = KNL_GET_BLOCK_DB_TYPE( sArgList );

        /* ValueBlock 정보 얻기 */
        sInputArgDataTypeDefInfo[ sArgCount ].mArgNum1 = KNL_GET_BLOCK_ARG_NUM1( sArgList );
        sInputArgDataTypeDefInfo[ sArgCount ].mArgNum2 = KNL_GET_BLOCK_ARG_NUM2( sArgList );
        sInputArgDataTypeDefInfo[ sArgCount ].mStringLengthUnit =
            KNL_GET_BLOCK_STRING_LENGTH_UNIT( sArgList );
        sInputArgDataTypeDefInfo[ sArgCount ].mIntervalIndicator =
            KNL_GET_BLOCK_INTERVAL_INDICATOR( sArgList );
            
        /* add context */
        sContext = & sFilterContext->mContexts[ sArgCount ];
        sContext->mInfo.mValueInfo = sArgList->mValueBlock;
        sContext->mCast = NULL;
        
        sArgCount++;
        sArgList = sArgList->mNext;
    }


    /**
     * check argument count
     */

    sFuncInfo = & gBuiltInFuncInfoArr[ aFuncID ];
    
    STL_TRY_THROW( ( sArgCount >= sFuncInfo->mArgumentCntMin ) &&
                   ( sArgCount <= sFuncInfo->mArgumentCntMax ),
                   ERROR_INVALID_ARGUMENT );


    /**
     * get function info
     */
                   
    STL_TRY( sFuncInfo->mGetFuncDetailInfo( sArgCount,
                                            sIsConstantData,
                                            sInputArgDataTypes,
                                            sInputArgDataTypeDefInfo,
                                            sArgCount,
                                            sResultArgDataTypes,
                                            sResultArgDataTypeDefInfo,
                                            & sResultType,
                                            & sResultTypeDefInfo,
                                            & sFuncPtrIdx,
                                            KNL_DT_VECTOR(aEnv),
                                            aEnv,                                            
                                            ELL_ERROR_STACK(aEnv) )
             == STL_SUCCESS );
    
    
    /**
     * filter predicate list 생성
     */

    STL_TRY( knlAllocRegionMem( aMemMgr,
                                STL_SIZEOF( knlPredicateList ),
                                (void**) & sPredList,
                                KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    stlMemset( sPredList,
               0x00,
               STL_SIZEOF( knlPredicateList ) );


    /**
     * filter 생성 : expression stack 생성
     */

    STL_TRY( knlExprCreate( & sFilter,
                            sArgCount + 1,
                            aMemMgr,
                            KNL_ENV( aEnv ) )
             == STL_SUCCESS );
                            

    /**
     * function entry 추가
     */

    sFuncInfo = & gBuiltInFuncInfoArr[ KNL_BUILTIN_FUNC_INVALID ];
    
    STL_TRY( knlExprAddFunction( sFilter,
                                 aFuncID,
                                 gBuiltInFuncInfoArr[ aFuncID ].mIsExprCompositionComplex,
                                 sArgCount,
                                 sArgCount,
                                 sArgCount,
                                 sFuncInfo->mExceptionType,
                                 sFuncInfo->mExceptionResult,
                                 sJumpEntryNo,
                                 & sGroupEntryNo,
                                 aMemMgr,
                                 KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    /**
     * function context 정보 설정
     */

    /* function data 공간 할당 */
    STL_TRY( knlAllocRegionMem( aMemMgr,
                                STL_SIZEOF( knlFunctionData ),
                                (void**) & sFuncData,
                                KNL_ENV( aEnv ) )
             == STL_SUCCESS );

    stlMemset( sFuncData,
               0x00,
               STL_SIZEOF( knlFunctionData ) );

    /* function pointer 설정 */
    STL_TRY( gBuiltInFuncInfoArr[ aFuncID ].mGetFuncPointer(
                 sFuncPtrIdx,
                 & sFuncData->mFuncPtr,
                 ELL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    /* result value 설정 */
    STL_DASSERT( sResultType == DTL_TYPE_BOOLEAN );
    STL_TRY( knlAllocDataValueArray( & sFuncData->mResultValue,
                                     sResultType,
                                     sResultTypeDefInfo.mArgNum1,
                                     sResultTypeDefInfo.mStringLengthUnit,
                                     1,
                                     aMemMgr,
                                     & sBufferSize,
                                     KNL_ENV( aEnv ) )
             == STL_SUCCESS );
    
    /* context 설정 */
    sContext = & sFilterContext->mContexts[ sArgCount ];
    sContext->mInfo.mFuncData = sFuncData;
    sContext->mCast = NULL;

    
    /**
     * argument entries 추가
     */

    sFuncInfo = & gBuiltInFuncInfoArr[ aFuncID ];
    
    for( sArgIdx = sArgCount ; sArgIdx > 0 ; )
    {
        /**
         * add argument
         */

        sArgIdx--;
        
        STL_TRY( knlExprAddValue( sFilter,
                                  sGroupEntryNo,
                                  sArgIdx,
                                  sFuncInfo->mExceptionType,
                                  sFuncInfo->mExceptionResult,
                                  sJumpEntryNo,
                                  aMemMgr,
                                  KNL_ENV( aEnv ) )
                 == STL_SUCCESS );
    }

    
    /**
     * @todo internal cast 설정 : dictionary 호출시 발생하면 추가 
     */

    
    /**
     * filter predicate list 설정
     */
    
    sPredList->mPredicate.mCond.mFilter = sFilter;
    sPredList->mPredicate.mType         = KNL_CONDITION_TYPE_FILTER;
    sPredList->mNext                    = NULL;
    

    /**
     * OUTPUT 설정
     */

    *aFilterPredList = sPredList;
    *aFilterContext  = sFilterContext;
        
    
    return STL_SUCCESS;

    STL_CATCH( ERROR_INVALID_ARGUMENT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ELL_ERRCODE_INVALID_ARGUMENT,
                      NULL,
                      ELL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief Key Compare를 위한 Key Compare List 구성
 * @param[in]     aIndexHandle     Index Handle
 * @param[in]     aRegionMem       Region Memory
 * @param[out]    aKeyCompList     Key Compare List
 * @param[in]     aEnv             Environment
 */
stlStatus ellGetKeyCompareList( ellDictHandle       * aIndexHandle,
                                knlRegionMem        * aRegionMem,
                                knlKeyCompareList  ** aKeyCompList,
                                ellEnv              * aEnv )
{
    knlKeyCompareList  * sKeyCompList;
    ellIndexKeyDesc    * sIndexKeyDesc;
    stlInt32             sIndexKeyCnt;
    stlInt32             sColID;
    stlInt32             sLoop;
    stlInt32             sCurPos;
    dtlDataType          sDataType[ DTL_INDEX_COLUMN_MAX_COUNT ];
    dtlDataType          sType;
    knlValueBlockList  * sValueList     = NULL;
    knlValueBlockList  * sPrevValueList = NULL;
    
    /**
     * Paramter Validation
     */

    STL_PARAM_VALIDATE( aIndexHandle != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, ELL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aKeyCompList != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * Index 정보 획득
     */
    
    sIndexKeyCnt  = ellGetIndexKeyCount( aIndexHandle );
    sIndexKeyDesc = ellGetIndexKeyDesc( aIndexHandle );

    
    /**
     * Key Compare List 공간 할당
     */

    STL_TRY( knlCreateKeyCompareList( sIndexKeyCnt,
                                      & sKeyCompList,
                                      aRegionMem,
                                      KNL_ENV(aEnv) )
             == STL_SUCCESS );    


    /**
     * Table Column Order 정보 구성 - SORT
     */

    for( sLoop = 0; sLoop < sIndexKeyCnt ; sLoop++ )
    {
        /* table column order */
        sKeyCompList->mTableColOrder[ sLoop ] =
            ellGetColumnIdx( & sIndexKeyDesc[ sLoop ].mKeyColumnHandle );

        sDataType[ sLoop ] = ellGetColumnDBType( & sIndexKeyDesc[ sLoop ].mKeyColumnHandle );
    }    

    /* insertion sort */
    for( sLoop = 1 ; sLoop < sIndexKeyCnt ; sLoop++ )
    {
        if( sKeyCompList->mTableColOrder[ sLoop - 1 ] <= sKeyCompList->mTableColOrder[ sLoop ] )
        {
            continue;
        }

        sColID    = sKeyCompList->mTableColOrder[ sLoop ];
        sType = sDataType[ sLoop ];
        
        sKeyCompList->mTableColOrder[ sLoop ] = sKeyCompList->mTableColOrder[ sLoop - 1 ];
        sDataType[ sLoop ] = sDataType[ sLoop - 1 ];
            
        for( sCurPos = sLoop - 1 ; sCurPos > 0 ; sCurPos-- )
        {
            if( sKeyCompList->mTableColOrder[ sCurPos - 1 ] <= sColID )
            {
                break;
            }

            sKeyCompList->mTableColOrder[ sCurPos ] = sKeyCompList->mTableColOrder[ sCurPos - 1 ];
            sDataType[ sCurPos ] = sDataType[ sCurPos - 1 ];
        }
        sKeyCompList->mTableColOrder[ sCurPos ] = sColID;
        sDataType[ sCurPos ] = sType;
    }


    /**
     * Index Column Offset, Function Pointer 와 Datavalue Buffer 설정
     */
    sKeyCompList->mValueList = NULL;
    
    /* Index Column Offset */
    for( sCurPos = 0 ; sCurPos < sIndexKeyCnt ; sCurPos++ )
    {
        for( sLoop = 0; sLoop < sIndexKeyCnt ; sLoop++ )
        {
            sColID = ellGetColumnIdx( & sIndexKeyDesc[ sLoop ].mKeyColumnHandle );
            sType  = sDataType[ sLoop ];
            if( sColID == sKeyCompList->mTableColOrder[ sCurPos ] )
            {
                sKeyCompList->mIndexColOffset[ sLoop ] = sCurPos;
                break;
            }
        }
        
        /* function pointer */
        sKeyCompList->mCompFunc[ sLoop ] =
            dtlPhysicalFuncArg2FuncList[ sType ][ sType ][ DTL_PHYSICAL_FUNC_IS_EQUAL ];

        STL_DASSERT( (sType != DTL_TYPE_LONGVARCHAR) &&
                     (sType != DTL_TYPE_CLOB) &&
                     (sType != DTL_TYPE_LONGVARBINARY) &&
                     (sType != DTL_TYPE_BLOB) );
        
        /* value block list */
        STL_TRY( knlInitBlock( & sValueList,
                               1,
                               STL_TRUE,
                               STL_LAYER_SQL_PROCESSOR,
                               ellGetColumnTableID( & sIndexKeyDesc[0].mKeyColumnHandle ),
                               sColID,
                               sType,
                               gDefaultDataTypeDef[ sType ].mArgNum1,
                               gDefaultDataTypeDef[ sType ].mArgNum2,
                               gDefaultDataTypeDef[ sType ].mStringLengthUnit,
                               gDefaultDataTypeDef[ sType ].mIntervalIndicator,
                               aRegionMem,
                               KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        if( sKeyCompList->mValueList == NULL )
        {
            sKeyCompList->mValueList = sValueList;
        }
        else
        {
            KNL_LINK_BLOCK_LIST( sPrevValueList, sValueList );
        }
        sPrevValueList = sValueList;
    }

    
    /**
     * OUTPUT 설정
     */

    *aKeyCompList = sKeyCompList;
        
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}

/** @} ellFilter */
